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

    bool bChanged = false;
    bool bFontChanged = false;

    void readSettings();
    void loadFonts();
    void connectComponents();

private slots:
    void saveSettings();
    void resetSettings();

    void changed();
    void fontChanged();

    void serverInfo();
    void titleFont();
    void dateFont();
    void messageFont();
    void applicationFont();
    void selectedApplicationFont();
    void resetFonts();

    void clearCache();
    void openCache();
};


#endif // SETTINGSDIALOG_H
