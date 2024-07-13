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

#include "tray.h"
#include "gotifyapi.h"
#include "listener.h"
#include "processthread.h"


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
    Tray * tray;
    QLockFile * lockfile;
    QShortcut * shortcut_quit;
    GotifyApi * gotifyApi;
    Listener * listener;
    bool firstConnect;
    ProcessThread::Applications processApplicationsThread;

    void initGui();
    void initComponents();
    void connectComponents();
    void applyStyle();

private slots:
    void showSettings();
    void reconnectCallback();
    void resfreshApplications();
    void themeChangedCallback(Qt::ColorScheme colorScheme);
    void serverChangedCallback();

    void listenerConnectedCallback();
    void listenerDisconnectedCallback();
    void messageReceivedCallback(GotifyModel::Message * message);

    void missedMessagesCallback(GotifyModel::Messages * messages);
    void applicationsCallback(GotifyModel::Applications * applications);
};

#endif // MAINAPPLICATION_H
