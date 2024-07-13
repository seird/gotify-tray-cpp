#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include "cache.h"
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
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Preferences — " + qApp->applicationName());

    ui->label_app_version->setText(QApplication::applicationVersion());
    ui->label_qt_version->setText(qVersion());
    ui->label_app_icon->setPixmap(QIcon(":/res/icons/gotify-tray++.ico").pixmap(22, 22));
    ui->label_qt_icon->setPixmap(QIcon(":/res/icons/qt.png").pixmap(22, 16));

    cacheThread = QThread::create([this]{ui->label_cache->setText(QString::number(cache->size()/1e6, 'f', 2) + " MB");});
    cacheThread->start();

#ifdef Q_OS_WIN
    ui->label_notification_duration->hide();
    ui->label_notification_duration_ms->hide();
    ui->spin_duration->hide();
#endif

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


void SettingsDialog::changed()
{
    bChanged = true;
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
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
    ui->cb_notify_start->setChecked(settings->notifyMissedStart());
}


void SettingsDialog::saveSettings()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);

    // -------------------------- General --------------------------
    settings->setNotificationPriority(ui->spin_priority->value());
    settings->setNotificationDurationMs(ui->spin_duration->value());
    settings->setNotifyMissed(ui->cb_notify->isChecked());
    settings->setNotifyMissedStart(ui->cb_notify_start->isChecked());
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
