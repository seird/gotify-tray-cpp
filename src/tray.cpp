#include "tray.h"
#include "settings.h"

#include <QApplication>
#include <QFile>


Tray::Tray(QObject * parent) : QSystemTrayIcon(parent)
{
    if (!isSystemTrayAvailable()) {
        qFatal() << "System tray is not available.";
    }

    if (!supportsMessages()) {
        qFatal() << "System does not support notifications.";
    }

    setToolTip(qApp->applicationName());
    setIcon(QIcon(":/res/icons/tray.png"));

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

void
Tray::setActive()
{
    iconError = false;

    QString path = ":/res/icons/tray.png";
    if (settings->customTray() && QFile(settings->customTrayPath()).exists())
        path = settings->customTrayPath();
    setIcon(QIcon(path));
}

void
Tray::setError()
{
    iconError = true;

    QString path = ":/res/icons/tray-error.png";
    if (settings->customTrayError() && QFile(settings->customTrayErrorPath()).exists())
        path = settings->customTrayErrorPath();
    setIcon(QIcon(path));
}

void
Tray::setUnread()
{
    QString path = ":/res/icons/tray-unread.png";
    if (settings->customTrayUnread() && QFile(settings->customTrayUnreadPath()).exists())
        path = settings->customTrayUnreadPath();
    setIcon(QIcon(path));
}

void
Tray::revertIcon()
{
    if (iconError) {
        setError();
    } else {
        setActive();
    }
}
