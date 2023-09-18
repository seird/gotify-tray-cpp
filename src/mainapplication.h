#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H


#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QLockFile>
#include <QStandardPaths>
#include <QString>
#include <QShortcut>
#include <QPoint>

#include "mainwindow.h"
#include "tray.h"
#include "gotifyapi.h"
#include "messageitemmodel.h"
#include "applicationitemmodel.h"
#include "listener.h"
#include "processthread.h"
#include "imagepopup.h"


class MainApplication : public QApplication
{
public:
    MainApplication(int &argc, char *argv[]);
    ~MainApplication();
    void init();
    bool acquireLock();
    bool verifyServer(bool forceNew = false, bool import = false);
    void quit();

private:
    MainWindow * mainWindow;
    Tray * tray;
    MessageItemModel messageItemModel;
    ApplicationItemModel applicationItemModel;
    ApplicationProxyModel * applicationProxyModel;
    QLockFile * lockfile;
    QShortcut * shortcut_quit;
    GotifyApi * gotifyApi;
    Listener * listener;
    ImagePopup * imagePopup;
    bool firstConnect;
    ProcessThread::Applications processApplicationsThread;

    void initGui();
    void initComponents();
    void connectComponents();
    void applyStyle();
    void initShortcuts();
    void addMessageToModel(GotifyModel::Message * message);

private slots:
    void showSettings();
    void reconnectCallback();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void mainWindowHidden();
    void refreshCallback();
    void deleteAllCallback(ApplicationItem * applicationItem);
    void deleteMessageCallback(MessageItem * item);
    void applicationChangedCallback(ApplicationItem * item);
    void showImagePopup(const QString& fileName, const QUrl& url, QPoint pos);
    void themeChangedCallback(Qt::ColorScheme colorScheme);
    void serverChangedCallback();

    void listenerConnectedCallback();
    void listenerDisconnectedCallback();
    void messageReceivedCallback(GotifyModel::Message * message);

    void messagesCallback(GotifyModel::Messages * messages);
    void missedMessagesCallback(GotifyModel::Messages * messages);
    void applicationsCallback(GotifyModel::Applications * applications);

    void insertApplications(GotifyModel::Applications * applications);
};

#endif // MAINAPPLICATION_H
