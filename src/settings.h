#ifndef SETTINGS_H
#define SETTINGS_H


#include <QApplication>
#include <QSettings>
#include <QSize>
#include <QFont>
#include <QUrl>


class Settings : public QSettings
{
    Q_OBJECT

public:
    Settings() : QSettings(qApp->applicationName()) {};

    static Settings * getInstance();

    void setServerUrl(QUrl url);
    QUrl serverUrl();

    void setClientToken(QByteArray token);
    QByteArray clientToken();

    void setLastId(int id);
    int lastId();

    void setNotifyMissed(bool mode);
    bool notifyMissed();

    void setNotifyMissedStart(bool mode);
    bool notifyMissedStart();

    void setNotifyMissedLimit(int value);
    int notifyMissedLimit();

    void setNotificationPriority(int priority);
    int notificationPriority();

    void setNotificationDurationMs(int ms);
    int notificationDurationMs();

signals:
    void serverChanged();
    void quitRequested();

};


extern Settings * settings;


#endif // SETTINGS_H
