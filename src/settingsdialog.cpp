#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include "cache.h"
#include "serverinfodialog.h"

#include <QApplication>
#include <QUrl>
#include <QDesktopServices>
#include <QFontDialog>
#include <QMessageBox>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Preferences — " + qApp->applicationName());

    ui->label_app_version->setText(QApplication::applicationVersion());
    ui->label_qt_version->setText(qVersion());
    ui->label_app_icon->setPixmap(QIcon(":/res/icons/gotify-tray++.ico").pixmap(22, 22));
    ui->label_qt_icon->setPixmap(QIcon(":/res/icons/qt.png").pixmap(22, 16));

#ifdef Q_OS_WIN
    ui->label_notification_duration->hide();
    ui->label_notification_duration_ms->hide();
    ui->spin_duration->hide();
#endif

    ui->label_cache->hide();
    ui->groupBox_watchdog->hide();
    ui->groupBox_logging->hide();

    readSettings();
    connectComponents();
    
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::serverInfo()
{
    QUrl url = settings->serverUrl();
    QByteArray clientToken = settings->clientToken();

    ServerInfoDialog dialog(url, clientToken, false);
    if (dialog.exec()) {
        settings->setServerUrl(dialog.getUrl());
        settings->setClientToken(dialog.getToken());
        emit settings->serverChanged();
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


void SettingsDialog::loadFonts()
{
    ui->label_title->setFont(settings->titleFont());
    ui->label_date->setFont(settings->dateFont());
    ui->label_message->setFont(settings->messageFont());
    ui->label_application1->setFont(settings->applicationFont());
    ui->label_application2->setFont(settings->applicationFont());
    ui->label_selected_application->setFont(settings->selectedApplicationFont());
}


void SettingsDialog::resetFonts()
{
    if (QMessageBox::warning(this, "Are you sure?", "Reset all fonts?",
                             QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel,
                             QMessageBox::StandardButton::Cancel)
        == QMessageBox::StandardButton::Ok) {
        settings->remove("font");
        loadFonts();
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


void SettingsDialog::clearCache()
{
    cache->clear();
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

    // --------------------------- Fonts ---------------------------
    loadFonts();

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
    settings->setPriorityColor(ui->cb_priority_colors->isChecked());
    settings->setUseLocale(ui->cb_locale->isChecked());
    settings->setSortApplications(ui->cb_sort_applications->isChecked());

    // --------------------------- Fonts ---------------------------
    if (bFontChanged) {
        settings->setTitleFont(ui->label_title->font());
        settings->setDateFont(ui->label_date->font());
        settings->setMessageFont(ui->label_message->font());
        settings->setApplicationFont(ui->label_application1->font());
        settings->setSelectedApplicationFont(ui->label_selected_application->font());
        emit settings->fontChanged();
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
