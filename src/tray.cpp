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

    actionShowWindow = new QAction("Show Window");
    actionSettings = new QAction("Settings");
    actionReconnect = new QAction("Reconnect");
    actionQuit = new QAction("Quit");

    menu.addAction(actionShowWindow);
    menu.addSeparator();
    menu.addAction(actionSettings);
    menu.addSeparator();
    menu.addAction(actionReconnect);
    menu.addSeparator();
    menu.addAction(actionQuit);

    setContextMenu(&menu);
}


Tray::~Tray()
{
    delete actionShowWindow;
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
