#include "settings.h"


Settings * settings = nullptr;


//------------------------------------------------------------------------------
Settings * Settings::getInstance() {
    if(!settings) {
        settings = new Settings();
    }
    return settings;
}


//------------------------------------------------------------------------------
void Settings::setServerUrl(QUrl url)
{
    setValue("serverUrl", url.toString());
}

QUrl Settings::serverUrl()
{
    return QUrl(value("serverUrl", "").toString());
}


//------------------------------------------------------------------------------
void Settings::setClientToken(QByteArray token)
{
    setValue("clientToken", QString(token));
}

QByteArray Settings::clientToken()
{
    return value("clientToken", "").toByteArray();
}


//------------------------------------------------------------------------------
void Settings::setLastId(int id)
{
    setValue("lastId", id);
}

int Settings::lastId()
{
    return value("lastId", 0).toInt();
}


//------------------------------------------------------------------------------
void Settings::setNotifyMissed(bool mode)
{
    setValue("notifyMissed", mode);
}


bool Settings::notifyMissed()
{
    return value("notifyMissed", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setNotifyMissedStart(bool mode)
{
    setValue("notifyMissedStart", mode);
}


bool Settings::notifyMissedStart()
{
    return value("notifyMissedStart", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setNotifyMissedLimit(int value)
{
    setValue("notifyMissedLimit", value);
}


int Settings::notifyMissedLimit()
{
    return value("notifyMissedLimit", 20).toInt();
}


//------------------------------------------------------------------------------
void Settings::setNotificationPriority(int priority)
{
    setValue("notificationPriority", priority);
}

int Settings::notificationPriority()
{
    return value("notificationPriority", 4).toInt();
}


//------------------------------------------------------------------------------
void Settings::setNotificationDurationMs(int ms)
{
    setValue("notificationDuration", ms);
}

int Settings::notificationDurationMs()
{
    return value("notificationDuration", 5000).toInt();
}
