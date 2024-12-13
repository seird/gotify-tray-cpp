#include "mainapplication.h"
#include "applicationitem.h"
#include "serverinfodialog.h"
#include "settingsdialog.h"
#include "settings.h"
#include "cache.h"
#include "appversion.h"
#include "processthread.h"
#include "requesthandler.h"
#include "utils.h"

#include <QShortcut>
#include <QKeySequence>
#include <QStyleHints>
#include <QStyle>
#include <QSystemTrayIcon>

#ifdef USE_KDE
#include <KNotification>
#endif


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
    if (applicationProxyModel) delete applicationProxyModel;
    if (mainWindow) delete mainWindow;
    if (tray) delete tray;
}


void MainApplication::init()
{
    initComponents();
    initGui();
    connectComponents();
    initShortcuts();

    QNetworkReply * reply = gotifyApi->applications();
    connect(reply, &QNetworkReply::finished, requestHandler, &RequestHandler::applications);

    listener->startListening();
}


void MainApplication::initComponents()
{
    QUrl serverUrl = settings->serverUrl();
    QByteArray clientToken = settings->clientToken();
    gotifyApi = new GotifyApi(serverUrl, clientToken);
    listener = new Listener(serverUrl, clientToken);
    applicationProxyModel = new ApplicationProxyModel(&applicationItemModel);
    mainWindow = new MainWindow(&messageItemModel, &applicationItemModel, applicationProxyModel);
    tray = new Tray();
    imagePopup = new ImagePopup(mainWindow);
}


void MainApplication::initGui()
{
    mainWindow->show();
    QTimer::singleShot(0, this, [this]{mainWindow->hide();});

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
    connect(mainWindow, &MainWindow::activated, tray, &Tray::revertIcon);
    connect(mainWindow, &MainWindow::hidden, this, &MainApplication::mainWindowHidden);
    connect(mainWindow, &MainWindow::refresh, this, &MainApplication::refreshCallback);
    connect(mainWindow, &MainWindow::deleteAll, this, &MainApplication::deleteAllCallback);
    connect(mainWindow, &MainWindow::deleteMessage, this, &MainApplication::deleteMessageCallback);
    connect(mainWindow, &MainWindow::applicationChanged, this, &MainApplication::applicationChangedCallback);

    connect(tray->actionShowWindow, &QAction::triggered, mainWindow, &MainWindow::bringToFront);
    connect(tray->actionSettings, &QAction::triggered, this, &MainApplication::showSettings);
    connect(tray->actionReconnect, &QAction::triggered, this, &MainApplication::reconnectCallback);
    connect(tray->actionQuit, &QAction::triggered, this, &MainApplication::quit);
    connect(tray, &Tray::activated, this, &MainApplication::trayActivated);
    connect(tray, &Tray::messageClicked, mainWindow, [this]{if (settings->notificationClick()) mainWindow->bringToFront();});

    connect(styleHints(), &QStyleHints::colorSchemeChanged, this, &MainApplication::themeChangedCallback);

    connect(requestHandler, &RequestHandler::finishedMessages, this, &MainApplication::messagesCallback);
    connect(requestHandler, &RequestHandler::finishedMissedMessages, this, &MainApplication::missedMessagesCallback);
    connect(requestHandler, &RequestHandler::finishedApplications, this, &MainApplication::applicationsCallback);
    connect(requestHandler, &RequestHandler::finishedImagePopup, this, &MainApplication::showImagePopup);

    connect(&processApplicationsThread, &ProcessThread::Applications::processed, this, &MainApplication::insertApplications);
    connect(&processMessageThread, &ProcessThread::Message::processed, this, &MainApplication::showKNotification);

    connect(listener, &Listener::connected, this, &MainApplication::listenerConnectedCallback);
    connect(listener, &Listener::disconnected, this, &MainApplication::listenerDisconnectedCallback);
    connect(listener, &Listener::messageReceived, this, &MainApplication::messageReceivedCallback);

    connect(settings, &Settings::serverChanged, this, &MainApplication::serverChangedCallback);
    connect(settings, &Settings::quitRequested, this, &MainApplication::quit);
}


void MainApplication::initShortcuts()
{
    new QShortcut(Qt::CTRL | Qt::Key_Q, mainWindow, this, &MainApplication::quit);
    new QShortcut(Qt::Key_F5, mainWindow, this, &MainApplication::refreshCallback);
}


void MainApplication::showSettings()
{
    SettingsDialog dialog(mainWindow);
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


void MainApplication::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
        mainWindow->bringToFront();
    }
}


void MainApplication::mainWindowHidden()
{
    imagePopup->close();
}


void MainApplication::refreshCallback()
{
    mainWindow->disableApplications();
    mainWindow->disableButtons();
    messageItemModel.clear();
    applicationItemModel.clear();

    QNetworkReply * reply = gotifyApi->applications();
    connect(reply, &QNetworkReply::finished, requestHandler, &RequestHandler::applications);
}


void MainApplication::messagesCallback(GotifyModel::Messages * messages)
{
    messageItemModel.clear();
    for (auto message : messages->messages) {
        messageItemModel.appendMessage(message);
        message->deleteLater();
    }
    messages->deleteLater();
    mainWindow->enableButtons();
    mainWindow->enableApplications(false);
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
            } else {
                addMessageToModel(message);
            }  
        }
        message->deleteLater();
    }
    messages->deleteLater();
}


void MainApplication::applicationsCallback(GotifyModel::Applications * applications)
{
    applicationItemModel.clear();
    processApplicationsThread.process(applications);
}


void MainApplication::insertApplications(GotifyModel::Applications * applications)
{
    ApplicationItem * item = new ApplicationItem("ALL MESSAGES");
    applicationItemModel.insertRow(0, item);

    for (auto application : applications->applications) {
        ApplicationItem * item = new ApplicationItem(application->name, application);
        item->setIcon(QIcon(cache->getFile(application->id)));
        applicationItemModel.appendRow(item);
        application->deleteLater();
    }

    mainWindow->enableApplications();
    applications->deleteLater();
}


void MainApplication::listenerConnectedCallback()
{
    qDebug() << "Listener connected";
    mainWindow->setActive();
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
    mainWindow->setConnecting();
    tray->setError();
    listener->startListening();
}


void MainApplication::addMessageToModel(GotifyModel::Message * message)
{
    // Check if the message's appId is in the applicationItemModel
    if (!applicationItemModel.itemFromId(message->appId)) {
        qWarning() << "Application " << message->appId << " is not in applicationItemModel.";
        return;
    }

    // Get the selected ApplicationItem
    ApplicationItem * applicationItem = applicationItemModel.itemFromIndex(applicationProxyModel->mapToSource(mainWindow->selectedApplication()));

    // If the selected ApplicationItem is the 'All messages' item, or if the appIds match
    //      --> insert the message
    if (applicationItem->allMessages() || (message->appId == applicationItem->id())) {
        messageItemModel.insertMessage(0, message);
    }
}


void MainApplication::messageReceivedCallback(GotifyModel::Message * message)
{
    addMessageToModel(message);

    // Don't show a notification if it's low priority or the window is active
    if (message->priority < settings->notificationPriority() || mainWindow->isActiveWindow()) {
        return;
    }

    // Change the tray icon to show there are unread notifications
    if (settings->trayUnreadEnabled() && !mainWindow->isActiveWindow()) {
        tray->setUnread();
    }

#ifdef USE_KDE
    // First process the message in case it's an image, which can be displayed in the notification itself
    if (settings->showNotificationImage())
        processMessageThread.process(message);
    else
        showKNotification(message);
#else
    // QSystemTrayIcon Notification
    tray->showMessage(
      message->title,
      message->message,
      QIcon(cache->getFile(message->appId)),
      settings->notificationDurationMs());
    message->deleteLater();
#endif
}

#ifdef USE_KDE
void
MainApplication::showKNotification(GotifyModel::Message* message)
{
    // KDE KNotification -- https://api.kde.org/frameworks/knotifications/html/classKNotification.html
    KNotification* notification = new KNotification(QStringLiteral("notification"));
    notification->setComponentName(QStringLiteral("plasma_workspace"));

    QString text = message->message;
    if (!Utils::containsHtml(message->message) && !message->markdown)
        text = Utils::replaceLinks(message->message);
    notification->setText(text);
    notification->setTitle(message->title);
    notification->setIconName(cache->getFile(message->appId));
    notification->setUrgency(Utils::priorityToUrgency(message->priority));
    if (settings->notificationClick()) {
        KNotificationAction* action = notification->addDefaultAction(QStringLiteral("Open")); // default action -> triggered when clicking the popup
        QObject::connect(action, &KNotificationAction::activated, this, [this] { mainWindow->bringToFront(); });
    }

    // Set image url on the notification
    if (settings->showNotificationImage()) {
        QList<QUrl> urls;
        for (auto url : Utils::extractURLs(message->message)) {
            QString imageUrl = Utils::extractImage(url);

            QString filePath;
            if (!imageUrl.isNull() && !(filePath = cache->getFile(imageUrl)).isNull()) {
                urls.append(QUrl::fromLocalFile(filePath));
                break; // Only add 1 image url
            }
        }
        notification->setUrls(urls);
    }
    
    notification->sendEvent();
    message->deleteLater();
}
#endif // USE_KDE


void MainApplication::deleteAllCallback(ApplicationItem * applicationItem)
{
    if (applicationItem->allMessages())
        gotifyApi->deleteMessages();
    else
        gotifyApi->deleteApplicaitonMessages(applicationItem->id());

    messageItemModel.clear();
}


void MainApplication::deleteMessageCallback(MessageItem * item)
{
    gotifyApi->deleteMessage(item->id());
    messageItemModel.removeRow(item->row());
}


void MainApplication::applicationChangedCallback(ApplicationItem * item)
{
    mainWindow->disableButtons();
    mainWindow->disableApplications();
    messageItemModel.clear();
    QNetworkReply * reply = item->allMessages() ? gotifyApi->messages() : gotifyApi->applicationMessages(item->id());
    connect(reply, &QNetworkReply::finished, requestHandler, [this]{requestHandler->messages();});
}


void MainApplication::showImagePopup(const QString& fileName, const QUrl& url, QPoint pos)
{
    imagePopup->display(fileName, url, pos);
}


void MainApplication::themeChangedCallback(Qt::ColorScheme colorScheme)
{
    mainWindow->setIcons();
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
    mainWindow->storeWindowState();
    settings->sync();

    if (cache)
        delete cache;

    disconnect(listener, &Listener::disconnected, this, &MainApplication::listenerDisconnectedCallback);
    listener->close();

    lockfile->unlock();

    QApplication::quit();
}
