#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include "cache.h"
#include "utils.h"
#include "serverinfodialog.h"

#include <QApplication>
#include <QUrl>
#include <QDesktopServices>
#include <QFontDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QThread>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    bChanged(false),
    bFontChanged(false),
    bSizeChanged(false),
    bShowPriorityChanged(false)
{
    ui->setupUi(this);
    setWindowTitle("Preferences — " + qApp->applicationName());

    ui->label_app_version->setText(QApplication::applicationVersion());
    ui->label_qt_version->setText(qVersion());
    ui->label_app_icon->setPixmap(QIcon(":/res/icons/gotify-tray++.ico").pixmap(22, 22));
    ui->label_qt_icon->setPixmap(QIcon(":/res/icons/qt.png").pixmap(22, 16));

    QString theme = Utils::getTheme();
    ui->label_status->setPixmap(QPixmap(":/res/themes/" + theme + "/status_active.svg"));
    ui->label_delete_all->setPixmap(QPixmap(":/res/themes/" + theme + "/trashcan.svg"));
    ui->label_delete->setPixmap(QPixmap(":/res/themes/" + theme + "/trashcan.svg"));
    ui->label_refresh->setPixmap(QPixmap(":/res/themes/" + theme + "/refresh.svg"));
    ui->label_priority->setStyleSheet("background-color: #b3e67e22;");

    cacheThread = QThread::create([this]{ui->label_cache->setText(QString::number(cache->size()/1e6, 'f', 2) + " MB");});
    cacheThread->start();

#ifdef Q_OS_WIN
    ui->label_notification_duration->hide();
    ui->label_notification_duration_ms->hide();
    ui->spin_duration->hide();
#endif

    ui->groupBox_watchdog->hide();
    ui->groupBox_logging->hide();

    readSettings();
    connectComponents();
    
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
    cacheThread->deleteLater();
}


void SettingsDialog::serverInfo()
{
    QUrl url = settings->serverUrl();
    QByteArray clientToken = settings->clientToken();

    ServerInfoDialog dialog(this, url, clientToken, false);
    dialog.exec();
}


void SettingsDialog::applicationIcon()
{
    int currentSize = ui->label_application_icon1->width();
    int size = QInputDialog::getInt(this, "Application Icon Size", "Size in pixels", currentSize, 10, 100);
    if (currentSize != size) {
        ui->label_application_icon1->setFixedSize(QSize(size, size));
        ui->label_application_icon2->setFixedSize(QSize(size, size));
        sizeChanged();
    }
}


void SettingsDialog::messageApplicationIcon()
{
    int currentSize = ui->label_icon->width();
    int size = QInputDialog::getInt(this, "Message Application Icon Size", "Size in pixels", currentSize, 10, 100);
    if (currentSize != size) {
        ui->label_icon->setFixedSize(QSize(size, size));
        sizeChanged();
    }
}


void SettingsDialog::mainButton()
{
    int currentSize = ui->label_refresh->width()/0.7; // TODO
    int size = QInputDialog::getInt(this, "Main Button Size", "Size in pixels", currentSize, 10, 100);
    if (currentSize != size) {
        ui->label_refresh->setFixedSize(0.7*QSize(size, size));
        ui->label_delete_all->setFixedSize(0.9*QSize(size, size));
        sizeChanged();
    }
}


void SettingsDialog::statusLabel()
{
    int currentSize = ui->label_status->width();
    int size = QInputDialog::getInt(this, "Status Label Size", "Size in pixels", currentSize, 10, 100);
    if (currentSize != size) {
        ui->label_status->setFixedSize(QSize(size, size));
        sizeChanged();
    }
}


void SettingsDialog::messageButton()
{
    int currentSize = ui->label_delete->width();
    int size = QInputDialog::getInt(this, "Message Button Size", "Size in pixels", currentSize, 10, 100);
    if (currentSize != size) {
        ui->label_delete->setFixedSize(QSize(size, size));
        sizeChanged();
    }
}


void SettingsDialog::titleFont()
{
    bool accepted = false;
    QFont font = QFontDialog::QFontDialog::getFont(&accepted, ui->label_title->font(), this, "Choose a title font");
    if (accepted) {
        ui->label_title->setFont(font);
        fontChanged();
    }
}


void SettingsDialog::dateFont()
{
    bool accepted = false;
    QFont font = QFontDialog::QFontDialog::getFont(&accepted, ui->label_date->font(), this, "Choose a date font");
    if (accepted) {
        ui->label_date->setFont(font);
        fontChanged();
    }
}


void SettingsDialog::messageFont()
{
    bool accepted = false;
    QFont font = QFontDialog::QFontDialog::getFont(&accepted, ui->label_message->font(), this, "Choose a message font");
    if (accepted) {
        ui->label_message->setFont(font);
        fontChanged();
    }
}


void SettingsDialog::applicationFont()
{
    bool accepted = false;
    QFont font = QFontDialog::QFontDialog::getFont(&accepted, ui->label_application1->font(), this, "Choose an application font");
    if (accepted) {
        ui->label_application1->setFont(font);
        ui->label_application2->setFont(font);
        fontChanged();
    }
}


void SettingsDialog::selectedApplicationFont()
{
    bool accepted = false;
    QFont font = QFontDialog::QFontDialog::getFont(&accepted, ui->label_selected_application->font(), this, "Choose a selected application font");
    if (accepted) {
        ui->label_selected_application->setFont(font);
        fontChanged();
    }
}


void SettingsDialog::messagePriority()
{
    int currentWidth = ui->label_priority->width();
    int width = QInputDialog::getInt(this, "Priority Color Width", "Width in pixels", currentWidth, 1, 50);
    if (currentWidth != width) {
        ui->label_priority->setFixedWidth(width);
        showPriorityChanged();
    }
}


void SettingsDialog::loadSizes()
{
    ui->label_application_icon1->setFixedSize(settings->applicationIconSize());
    ui->label_application_icon2->setFixedSize(settings->applicationIconSize());
    ui->label_refresh->setFixedSize(0.7*settings->mainButtonSize());
    ui->label_delete_all->setFixedSize(0.9*settings->mainButtonSize());
    ui->label_delete->setFixedSize(settings->messageButtonSize());
    ui->label_status->setFixedSize(settings->statusLabelSize());
    ui->label_icon->setFixedSize(settings->messageApplicationIconSize());
    ui->label_priority->setFixedWidth(settings->priorityColorWidth());
}


void SettingsDialog::loadFonts()
{
    ui->label_title->setFont(settings->titleFont());
    ui->label_date->setFont(settings->dateFont());
    ui->label_message->setFont(settings->messageFont());
    ui->label_application1->setFont(settings->applicationFont());
    ui->label_application2->setFont(settings->applicationFont());
    ui->label_selected_application->setFont(settings->selectedApplicationFont());
}


void SettingsDialog::resetFontsSizes()
{
    if (QMessageBox::warning(this, "Are you sure?", "Reset all fonts and sizes?",
                             QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel,
                             QMessageBox::StandardButton::Cancel)
        == QMessageBox::StandardButton::Ok) {
        settings->remove("font");
        settings->remove("size");
        loadFonts();
        loadSizes();
        emit settings->fontChanged();
        emit settings->sizeChanged();
        emit settings->showPriorityChanged(settings->priorityColor());
    }
}


void SettingsDialog::changed()
{
    bChanged = true;
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}


void SettingsDialog::fontChanged()
{
    bFontChanged = true;
    changed();
}


void SettingsDialog::sizeChanged()
{
    bSizeChanged = true;
    changed();
}


void SettingsDialog::showPriorityChanged()
{
    bShowPriorityChanged = true;
    changed();
}


void SettingsDialog::clearCache()
{
    cache->clear();
    ui->label_cache->setText("0 MB");
}


void SettingsDialog::openCache()
{
    QDesktopServices::openUrl(QUrl("file:///" + cache->getDir(), QUrl::TolerantMode));
}


void SettingsDialog::connectComponents()
{
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::saveSettings);
}


void SettingsDialog::readSettings()
{
    // -------------------------- General --------------------------
    ui->spin_priority->setValue(settings->notificationPriority());
    ui->spin_duration->setValue(settings->notificationDurationMs());
    ui->cb_notify->setChecked(settings->notifyMissed());
    ui->cb_notification_click->setChecked(settings->notificationClick());
    ui->cb_tray_icon_unread->setChecked(settings->trayUnreadEnabled());
    ui->cb_priority_colors->setChecked(settings->priorityColor());
    ui->cb_locale->setChecked(settings->useLocale());
    ui->cb_sort_applications->setChecked(settings->sortApplications());
    ui->cb_image_urls->setChecked(settings->showImageUrl());

    // --------------------------- Fonts ---------------------------
    loadFonts();
    loadSizes();

    // -------------------------- Advanced -------------------------
    ui->groupbox_image_popup->setChecked(settings->popupEnabled());
    ui->spin_popup_w->setValue(settings->popupWidth());
    ui->spin_popup_h->setValue(settings->popupHeight());

    ui->groupBox_watchdog->setChecked(settings->watchdogEnabled());
    ui->spin_watchdog_interval->setValue(settings->watchdogInterval());
}


void SettingsDialog::saveSettings()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);

    // -------------------------- General --------------------------
    settings->setNotificationPriority(ui->spin_priority->value());
    settings->setNotificationDurationMs(ui->spin_duration->value());
    settings->setNotifyMissed(ui->cb_notify->isChecked());
    settings->setNotificationClick(ui->cb_notification_click->isChecked());
    settings->setTrayUnreadEnabled(ui->cb_tray_icon_unread->isChecked());
    settings->setUseLocale(ui->cb_locale->isChecked());
    settings->setSortApplications(ui->cb_sort_applications->isChecked());
    settings->setShowImageUrl(ui->cb_image_urls->isChecked());
    if (bShowPriorityChanged) {
        bool mode = ui->cb_priority_colors->isChecked();
        settings->setPriorityColorWidth(ui->label_priority->width());
        settings->setPriorityColor(mode);
        emit settings->showPriorityChanged(mode);
    }

    // --------------------------- Fonts ---------------------------
    if (bFontChanged) {
        settings->setTitleFont(ui->label_title->font());
        settings->setDateFont(ui->label_date->font());
        settings->setMessageFont(ui->label_message->font());
        settings->setApplicationFont(ui->label_application1->font());
        settings->setSelectedApplicationFont(ui->label_selected_application->font());
        emit settings->fontChanged();
    }

    // --------------------------- Sizes ---------------------------
    if (bSizeChanged) {
        settings->setApplicationIconSize(ui->label_application_icon1->size());
        settings->setMessageApplicationIconSize(ui->label_icon->size());
        settings->setMainButtonSize(ui->label_refresh->size() / 0.7);
        settings->setStatusLabelSize(ui->label_status->size());
        settings->setMessageButtonSize(ui->label_delete->size());

        emit settings->sizeChanged();
    }

    // -------------------------- Advanced -------------------------
    settings->setPopupEnabled(ui->groupbox_image_popup->isChecked());
    settings->setPopupWidth(ui->spin_popup_w->value());
    settings->setPopupHeight(ui->spin_popup_h->value());

    settings->setWatchdogEnabled(ui->groupBox_watchdog->isChecked());
    settings->setWatchdogInterval(ui->spin_watchdog_interval->value());
}


void SettingsDialog::resetSettings()
{
    if (QMessageBox::warning(this, "Are you sure?", "Reset all settings?",
                             QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel,
                             QMessageBox::StandardButton::Cancel)
        == QMessageBox::StandardButton::Ok) {
        settings->clear();
        emit settings->quitRequested();
    }
}
