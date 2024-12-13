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

    void setStatusLabelSize(QSize size);
    QSize statusLabelSize();

    void setMainButtonSize(QSize size);
    QSize mainButtonSize();

    void setMessageButtonSize(QSize size);
    QSize messageButtonSize();

    void setApplicationIconSize(QSize size);
    QSize applicationIconSize();

    void setMessageApplicationIconSize(QSize size);
    QSize messageApplicationIconSize();

    void setWindowGeometry(QByteArray geometry);
    QByteArray windowGeometry();

    void setWindowState(QByteArray state);
    QByteArray windowState();

    void setSplitterState(QByteArray state);
    QByteArray splitterState();

    void setMessageWidgetHeightMin(int height);
    int messageWidgetHeightMin();

    void setMessageWidgetImageSize(QSize size);
    QSize messageWidgetImageSize();

    void setMessageWidgetContentImageWidth(float width);
    float messageWidgetContentImageWidth();

    void setMessageWidgetContentImageHeight(float height);
    float messageWidgetContentImageHeight();

    void setShowImageUrl(bool mode);
    bool showImageUrl();

    void setShowNotificationImage(bool mode);
    bool showNotificationImage();

    void setPriorityColor(bool mode);
    bool priorityColor();

    void setPriorityColorWidth(int w);
    int priorityColorWidth();

    void setNotificationPriority(int priority);
    int notificationPriority();

    void setNotificationDurationMs(int ms);
    int notificationDurationMs();

    void setNotificationShowIcon(bool mode);
    bool notificationShowIcon();

    void setNotificationClick(bool mode);
    bool notificationClick();

    void setTrayUnreadEnabled(bool mode);
    bool trayUnreadEnabled();

    void setPopupEnabled(bool mode);
    bool popupEnabled();

    void setPopupHeight(int height);
    int popupHeight();

    void setPopupWidth(int width);
    int popupWidth();

    void setWatchdogEnabled(bool mode);
    bool watchdogEnabled();

    void setWatchdogInterval(int s);
    int watchdogInterval();

    void setSelectedApplicationFont(QFont font);
    QFont selectedApplicationFont();

    void setApplicationFont(QFont font);
    QFont applicationFont();

    void setTitleFont(QFont font);
    QFont titleFont();

    void setDateFont(QFont font);
    QFont dateFont();

    void setMessageFont(QFont font);
    QFont messageFont();

    void setUseLocale(bool mode);
    bool useLocale();

    void setSortApplications(bool mode);
    bool sortApplications();

signals:
    void serverChanged();
    void sizeChanged();
    void fontChanged();
    void showPriorityChanged(bool mode);
    void quitRequested();

};


extern Settings * settings;


#endif // SETTINGS_H
