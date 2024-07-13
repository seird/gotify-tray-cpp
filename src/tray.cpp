#include "tray.h"

#include <QApplication>


Tray::Tray(QObject * parent) : QSystemTrayIcon(parent)
{
    if (!isSystemTrayAvailable()) {
        qFatal() << "System tray is not available.";
    }

    if (!supportsMessages()) {
        qFatal() << "System does not support notifications.";
    }

    setToolTip(qApp->applicationName());
    setIcon(QIcon(":/res/icons/gotify-tray++.ico"));

    actionSettings = new QAction("Settings");
    actionReconnect = new QAction("Reconnect");
    actionQuit = new QAction("Quit");

    menu.addAction(actionSettings);
    menu.addAction(actionReconnect);
    menu.addAction(actionQuit);

    setContextMenu(&menu);
}


Tray::~Tray()
{
    delete actionSettings;
    delete actionReconnect;
    delete actionQuit;
}


void Tray::setActive()
{
    iconError = false;
    setIcon(QIcon(":/res/icons/tray.png"));
}


void Tray::setError()
{
    iconError = true;
    setIcon(QIcon(":/res/icons/tray-error.png"));
}


void Tray::setUnread()
{
    setIcon(QIcon(":/res/icons/tray-unread.png"));
}


void Tray::revertIcon()
{
    if (iconError) {
        setError();
    } else {
        setActive();
    }
}
