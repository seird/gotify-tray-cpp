#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QThread>


namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget * parent = nullptr);
    ~SettingsDialog();

private:
    Ui::SettingsDialog * ui;
    QThread * cacheThread;

    bool bChanged;
    bool bFontChanged;
    bool bSizeChanged;
    bool bShowPriorityChanged;

    void readSettings();
    void loadSizes();
    void loadFonts();
    void connectComponents();

private slots:
    void saveSettings();
    void resetSettings();

    void changed();
    void fontChanged();
    void sizeChanged();
    void showPriorityChanged();

    void serverInfo();
    void resetFontsSizes();
    void titleFont();
    void dateFont();
    void messageFont();
    void applicationFont();
    void selectedApplicationFont();
    void applicationIcon();
    void messageApplicationIcon();
    void mainButton();
    void statusLabel();
    void messageButton();
    void messagePriority();

    void clearCache();
    void openCache();

    void openCustomTray();
    void openCustomTrayUnread();
    void openCustomTrayError();

    void trayPathChanged(QString path);
    void trayPathUnreadChanged(QString path);
    void trayPathErrorChanged(QString path);
};

#endif // SETTINGSDIALOG_H
