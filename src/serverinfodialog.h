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
    explicit ServerInfoDialog(QUrl url = QUrl(), QByteArray clientToken = nullptr, bool import = true, QWidget * parent = nullptr);
    ~ServerInfoDialog();
    QUrl getUrl();
    QByteArray getToken();

private slots:
    void inputChangedCallback();
    void importCallback();
    void testCallback();
    void testSuccessCallback();
    void testErrorCallback(QNetworkReply::NetworkError error);

private:
    GotifyApi * gotifyApi;
    Ui::ServerInfoDialog *ui;

    void enableInputs();
    void disableInputs();
    
};

#endif // SERVERINFODIALOG_H
