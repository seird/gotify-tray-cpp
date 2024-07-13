#ifndef TRAY_H
#define TRAY_H


#include <QSystemTrayIcon>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QDebug>


class Tray : public QSystemTrayIcon
{
public:
    Tray(QObject * parent = nullptr);
    ~Tray();
    
    void setActive();
    void setError();
    void setUnread();
    void revertIcon();

    QAction * actionSettings;
    QAction * actionReconnect;
    QAction * actionQuit;
private:
    QMenu menu;
    bool iconError = false;
};


#endif // TRAY_H
