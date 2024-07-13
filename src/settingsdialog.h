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

    void readSettings();
    void connectComponents();

private slots:
    void saveSettings();
    void resetSettings();

    void changed();

    void serverInfo();

    void clearCache();
    void openCache();
};


#endif // SETTINGSDIALOG_H
