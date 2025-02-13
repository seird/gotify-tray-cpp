#ifndef SERVERINFODIALOG_H
#define SERVERINFODIALOG_H

#include <QDialog>
#include <QUrl>

#include "gotifyapi.h"


namespace Ui {
class ServerInfoDialog;
}

class ServerInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerInfoDialog(QWidget* parent = nullptr, QUrl url = QUrl(), QByteArray clientToken = nullptr, QString certPath = "");
    ~ServerInfoDialog();

private slots:
    void acceptedCallback();
    void inputChangedCallback();
    void urlChangedCallback(QString text);
    void certificateCallback();
    void testCallback();
    void testSuccessCallback();
    void testErrorCallback(QNetworkReply::NetworkError error, QString errorString);

private:
    GotifyApi * gotifyApi;
    Ui::ServerInfoDialog *ui;
    QString certPath;

    void enableInputs();
    void disableInputs();
    
};

#endif // SERVERINFODIALOG_H
