#include "mainapplication.h"
#include "serverinfodialog.h"
#include "settingsdialog.h"
#include "settings.h"
#include "cache.h"
#include "appversion.h"
#include "requesthandler.h"

#include <QStyleHints>
#include <QStyle>


MainApplication::MainApplication(int &argc, char *argv[])
    : QApplication(argc, argv),
      firstConnect(true)
{
    setApplicationName("Gotify Tray++");
    setApplicationVersion(appVersion.toString());
    setDesktopFileName(applicationName());
    setQuitOnLastWindowClosed(false);
    setWindowIcon(QIcon(":/res/icons/gotify-tray++.ico"));
    applyStyle();

    settings = Settings::getInstance();
    cache = Cache::getInstance();
    requestHandler = RequestHandler::getInstance();

    lockfile = new QLockFile(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + applicationName() + ".lock");
}


MainApplication::~MainApplication()
{
    delete lockfile;
    if (gotifyApi) delete gotifyApi;
    if (listener) delete listener;
    if (tray) delete tray;
}


void MainApplication::init()
{
    initComponents();
    initGui();
    connectComponents();

    resfreshApplications();

    listener->startListening();
}


void MainApplication::initComponents()
{
    QUrl serverUrl = settings->serverUrl();
    QByteArray clientToken = settings->clientToken();
    gotifyApi = new GotifyApi(serverUrl, clientToken);
    listener = new Listener(serverUrl, clientToken);
    tray = new Tray();
}


void MainApplication::initGui()
{
    tray->setError();
    tray->show();
}


bool MainApplication::acquireLock()
{
#ifdef QT_DEBUG
    return true;
#else
    lockfile->setStaleLockTime(0);
    return lockfile->tryLock();
#endif
}


bool MainApplication::verifyServer(bool forceNew, bool import)
{
    QUrl url = settings->serverUrl();
    QByteArray clientToken = settings->clientToken();

    if (forceNew || url.isEmpty() || clientToken.isEmpty()) {
        ServerInfoDialog dialog(NULL, url, clientToken, import);
        return dialog.exec();
    } else {
        return true;
    }
}


void MainApplication::connectComponents()
{
    connect(tray->actionSettings, &QAction::triggered, this, &MainApplication::showSettings);
    connect(tray->actionReconnect, &QAction::triggered, this, &MainApplication::reconnectCallback);
    connect(tray->actionQuit, &QAction::triggered, this, &MainApplication::quit);

    connect(styleHints(), &QStyleHints::colorSchemeChanged, this, &MainApplication::themeChangedCallback);

    connect(requestHandler, &RequestHandler::finishedMissedMessages, this, &MainApplication::missedMessagesCallback);
    connect(requestHandler, &RequestHandler::finishedApplications, this, &MainApplication::applicationsCallback);
    
    connect(listener, &Listener::connected, this, &MainApplication::listenerConnectedCallback);
    connect(listener, &Listener::disconnected, this, &MainApplication::listenerDisconnectedCallback);
    connect(listener, &Listener::messageReceived, this, &MainApplication::messageReceivedCallback);

    connect(settings, &Settings::serverChanged, this, &MainApplication::serverChangedCallback);
    connect(settings, &Settings::quitRequested, this, &MainApplication::quit);
}


void MainApplication::showSettings()
{
    SettingsDialog dialog;
    dialog.exec();
}


void MainApplication::serverChangedCallback()
{
    QUrl url = settings->serverUrl();
    QByteArray token = settings->clientToken();
    gotifyApi->updateAuth(url, token);
    listener->updateAuth(url, token);
    reconnectCallback();
}


void MainApplication::reconnectCallback()
{
    if (listener->isConnected())
        listener->close();
    else
        listener->startListening();
}


void MainApplication::resfreshApplications()
{
    QNetworkReply * reply = gotifyApi->applications();
    connect(reply, &QNetworkReply::finished, requestHandler, &RequestHandler::applications);
}


void MainApplication::missedMessagesCallback(GotifyModel::Messages * messages)
{
    int lastId = settings->lastId();
    QListIterator it(messages->messages);
    it.toBack();
    while (it.hasPrevious()) {
        GotifyModel::Message * message = it.previous();
        if (message->id > lastId) {
            if (settings->notifyMissed()) {
                messageReceivedCallback(message);
            }
        }
        message->deleteLater();
    }
    messages->deleteLater();
}


void MainApplication::applicationsCallback(GotifyModel::Applications * applications)
{
    qDebug() << "Applications callback";
    processApplicationsThread.process(applications);
}


void MainApplication::listenerConnectedCallback()
{
    qDebug() << "Listener connected";
    tray->setActive();

    if (firstConnect) {
        firstConnect = false;
        return;
    }

    QNetworkReply * reply = gotifyApi->messages();
    connect(reply, &QNetworkReply::finished, requestHandler, [this]{requestHandler->messages(true);});
}


void MainApplication::listenerDisconnectedCallback()
{
    qDebug() << "Listener disconnected";
    tray->setError();
    listener->startListening();
}


void MainApplication::messageReceivedCallback(GotifyModel::Message * message)
{
    // Don't show a notification if it's low priority
    if (message->priority < settings->notificationPriority()) {
        return;
    }

    // Change the tray icon to show there are unread notifications
    if (settings->trayUnreadEnabled()) {
        tray->setUnread();
    }

    QString file = cache->getFile(message->appId);
    if (file.isNull())
        resfreshApplications();

    tray->showMessage(
        message->title,
        message->message,
        QIcon(file),
        settings->notificationDurationMs()
    );

    message->deleteLater();
}


void MainApplication::themeChangedCallback(Qt::ColorScheme colorScheme)
{
    applyStyle();
}


void MainApplication::applyStyle()
{
// Check if this is necessary on other DEs as well
#ifdef Q_OS_WIN
    setStyle("fusion");
    setPalette(style()->standardPalette());
#endif
}


void MainApplication::quit(){
    qDebug() << "Quit requested";

    tray->hide();
    settings->sync();

    if (cache)
        delete cache;

    disconnect(listener, &Listener::disconnected, this, &MainApplication::listenerDisconnectedCallback);
    listener->close();

    lockfile->unlock();

    QApplication::quit();
}
