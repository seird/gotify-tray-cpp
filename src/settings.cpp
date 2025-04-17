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
void Settings::setStatusLabelSize(QSize size)
{
    setValue("size/statusLabelSize", size);
}

QSize Settings::statusLabelSize()
{
    return value("size/statusLabelSize", QSize(25, 25)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setMainButtonSize(QSize size)
{
    setValue("size/mainButtonSize", size);
}

QSize Settings::mainButtonSize()
{
    return value("size/mainButtonSize", QSize(33, 33)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setMessageButtonSize(QSize size)
{
    setValue("size/messageButtonSize", size);
}

QSize Settings::messageButtonSize()
{
    return value("size/messageButtonSize", QSize(25, 25)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setApplicationIconSize(QSize size)
{
    setValue("size/applicationIconSize", size);
}

QSize Settings::applicationIconSize()
{
    return value("size/applicationIconSize", QSize(40, 40)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setMessageApplicationIconSize(QSize size)
{
    setValue("size/messageApplicationIconSize", size);
}

QSize Settings::messageApplicationIconSize()
{
    return value("size/messageApplicationIconSize", QSize(33, 33)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setWindowGeometry(QByteArray geometry)
{
    setValue("state/geometry", geometry);
}

QByteArray Settings::windowGeometry()
{
    return value("state/geometry").toByteArray();
}


//------------------------------------------------------------------------------
void Settings::setWindowState(QByteArray state)
{
    setValue("state/state", state);
}

QByteArray Settings::windowState()
{
    return value("state/state").toByteArray();
}


//------------------------------------------------------------------------------
void Settings::setSplitterState(QByteArray state)
{
    setValue("state/splitterState", state);
}

QByteArray Settings::splitterState()
{
    return value("state/splitterState").toByteArray();
}


//------------------------------------------------------------------------------
void Settings::setMessageWidgetHeightMin(int height)
{
    setValue("messageWidgetHeightMin", height);
}

int Settings::messageWidgetHeightMin()
{
    return value("messageWidgetHeightMin", 100).toInt();
}


//------------------------------------------------------------------------------
void Settings::setMessageWidgetImageSize(QSize size)
{
    setValue("size/messageWidgetImageSizeMax", size);
}

QSize Settings::messageWidgetImageSize()
{
    return value("size/messageWidgetImageSizeMax", QSize(33, 33)).toSize();
}


//------------------------------------------------------------------------------
void Settings::setMessageWidgetContentImageWidth(float width)
{
    setValue("messageWidgetContentImageWidth", width);
}

float Settings::messageWidgetContentImageWidth()
{
    return value("messageWidgetContentImageWidth", 0.8f).toFloat();
}


//------------------------------------------------------------------------------
void Settings::setMessageWidgetContentImageHeight(float height)
{
    setValue("messageWidgetContentImageHeight", height);
}

float Settings::messageWidgetContentImageHeight()
{
    return value("messageWidgetContentImageHeight", 0.3f).toFloat();
}

//------------------------------------------------------------------------------
void
Settings::setMessageFallback(bool mode)
{
    setValue("messageFallback", mode);
}

bool
Settings::messageFallback()
{
    return value("messageFallback", false).toBool();
}

//------------------------------------------------------------------------------
void
Settings::setRenderMarkdown(bool mode)
{
    setValue("renderMarkdown", mode);
}

bool
Settings::renderMarkdown()
{
    return value("renderMarkdown", false).toBool();
}

//------------------------------------------------------------------------------
void Settings::setShowImageUrl(bool mode)
{
    setValue("showImageUrl", mode);
}


bool Settings::showImageUrl()
{
    return value("showImageUrl", true).toBool();
}

//------------------------------------------------------------------------------
void
Settings::setForcePlainText(bool mode)
{
    setValue("forcePlainText", mode);
}

bool
Settings::forcePlainText()
{
    return value("forcePlainText", false).toBool();
}

//------------------------------------------------------------------------------
void Settings::setPriorityColor(bool mode)
{
    setValue("priorityColor", mode);
}


bool Settings::priorityColor()
{
    return value("priorityColor", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setPriorityColorWidth(int w)
{
    setValue("size/priorityColorWidth", w);
}


int Settings::priorityColorWidth()
{
    return value("size/priorityColorWidth", 6).toInt();
}


//------------------------------------------------------------------------------
void Settings::setNotificationPriority(int priority)
{
    setValue("notificationPriority", priority);
}

int Settings::notificationPriority()
{
    return value("notificationPriority", 0).toInt();
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


//------------------------------------------------------------------------------
void Settings::setNotificationShowIcon(bool mode)
{
    setValue("notificationShowIcon", mode);
}

bool Settings::notificationShowIcon()
{
    return value("notificationShowIcon", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setNotificationClick(bool mode)
{
    setValue("notificationClick", mode);
}

bool Settings::notificationClick()
{
    return value("notificationClick", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setTrayUnreadEnabled(bool mode)
{
    setValue("trayUnread", mode);
}

bool Settings::trayUnreadEnabled()
{
    return value("trayUnread", false).toBool();
}


//------------------------------------------------------------------------------
void Settings::setPopupEnabled(bool mode)
{
    setValue("popupEnabled", mode);
}

bool Settings::popupEnabled()
{
    return value("popupEnabled", false).toBool();
}


//------------------------------------------------------------------------------
void Settings::setPopupHeight(int height)
{
    setValue("popupHeight", height);
}

int Settings::popupHeight()
{
    return value("popupHeight", 400).toInt();
}


//------------------------------------------------------------------------------
void Settings::setPopupWidth(int width)
{
    setValue("popupWidth", width);
}

int Settings::popupWidth()
{
    return value("popupWidth", 400).toInt();
}


//------------------------------------------------------------------------------
void
Settings::setSelectedApplicationFont(QFont font)
{
    setValue("font/selectedApplication", font.toString());
}


QFont Settings::selectedApplicationFont()
{
    QFont font;
    QString s = value("font/selectedApplication", "").toString();
    if (s.isEmpty()) {
        font.setPointSize(font.pointSize()+3);
        font.setBold(true);
    } else {
        font.fromString(s);
    }
    return font;
}


//------------------------------------------------------------------------------
void Settings::setApplicationFont(QFont font)
{
    setValue("font/application", font.toString());
}


QFont Settings::applicationFont()
{
    QFont font;
    QString s = value("font/application", "").toString();
    if (!s.isEmpty()) {
        font.fromString(s);
    }
    return font;
}


//------------------------------------------------------------------------------
void Settings::setTitleFont(QFont font)
{
    setValue("font/title", font.toString());
}


QFont Settings::titleFont()
{
    QFont font;
    QString s = value("font/title", "").toString();
    if (s.isEmpty()) {
        font.setBold(true);
    } else {
        font.fromString(s);
    }
    return font;
}


//------------------------------------------------------------------------------
void Settings::setDateFont(QFont font)
{
    setValue("font/date", font.toString());
}


QFont Settings::dateFont()
{
    QFont font;
    QString s = value("font/date", "").toString();
    if (s.isEmpty()) {
        font.setItalic(true);
    } else {
        font.fromString(s);
    }
    return font;
}


//------------------------------------------------------------------------------
void Settings::setMessageFont(QFont font)
{
    setValue("font/message", font.toString());
}


QFont Settings::messageFont()
{
    QFont font;
    QString s = value("font/message", "").toString();
    if (!s.isEmpty()) {
        font.fromString(s);
    }
    return font;
}


//------------------------------------------------------------------------------
void Settings::setUseLocale(bool mode)
{
    setValue("useLocale", mode);
}


bool Settings::useLocale()
{
    return value("useLocale", true).toBool();
}


//------------------------------------------------------------------------------
void Settings::setSortApplications(bool mode)
{
    setValue("sortApplications", mode);
}


bool Settings::sortApplications()
{
    return value("sortApplications", true).toBool();
}

//------------------------------------------------------------------------------
int
Settings::heartbeatInterval()
{
    return value("heartbeatInterval", 60000).toInt();
}

//------------------------------------------------------------------------------
void
Settings::setSelfSignedCertificatePath(QString path)
{
    setValue("selfSignedCertificatePath", path);
}

QString
Settings::selfSignedCertificatePath()
{
    return value("selfSignedCertificatePath", "").toString();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTray(bool mode)
{
    setValue("customTray", mode);
}

bool
Settings::customTray()
{
    return value("customTray", false).toBool();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTrayUnread(bool mode)
{
    setValue("customTrayUnread", mode);
}

bool
Settings::customTrayUnread()
{
    return value("customTrayUnread", false).toBool();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTrayError(bool mode)
{
    setValue("customTrayError", mode);
}

bool
Settings::customTrayError()
{
    return value("customTrayError", true).toBool();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTrayPath(QString path)
{
    setValue("customTrayPath", path);
}
QString
Settings::customTrayPath()
{
    return value("customTrayPath", ":/res/icons/tray.png").toString();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTrayUnreadPath(QString path)
{
    setValue("customTrayUnreadPath", path);
}
QString
Settings::customTrayUnreadPath()
{
    return value("customTrayUnreadPath", ":/res/icons/tray-unread.png").toString();
}

//------------------------------------------------------------------------------
void
Settings::setCustomTrayErrorPath(QString path)
{
    setValue("customTrayErrorPath", path);
}
QString
Settings::customTrayErrorPath()
{
    return value("customTrayErrorPath", ":/res/icons/tray-error.png").toString();
}
