#include "serverinfodialog.h"
#include "requesthandler.h"
#include "settings.h"
#include "ui_serverinfodialog.h"
#include "utils.h"

#include <QDialogButtonBox>
#include <QFileDialog>

ServerInfoDialog::ServerInfoDialog(QWidget* parent, QUrl url, QByteArray clientToken, QString certPath)
  : QDialog(parent)
  , ui(new Ui::ServerInfoDialog)
{
    this->certPath = certPath;

    ui->setupUi(this);
    ui->pb_certificate->hide();
    ui->line_url->setText(url.toString());
    ui->line_token->setText(clientToken);
    if (url.scheme() == "https" && !certPath.isEmpty())
        ui->label_status->setText("Certificate: " + certPath);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    setStyleSheet(Utils::readFile(":/res/themes/" + Utils::getTheme() + "/ServerInfoDialog.qss"));

    gotifyApi = new GotifyApi(QUrl(), "", "");

    connect(requestHandler, &RequestHandler::serverOk, this, &ServerInfoDialog::testSuccessCallback);
    connect(requestHandler, &RequestHandler::replyError, this, &ServerInfoDialog::testErrorCallback);
    connect(requestHandler, &RequestHandler::finished, this, [this] { ui->pb_test->setEnabled(true); });
    connect(this, &QDialog::accepted, this, &ServerInfoDialog::acceptedCallback);
}

ServerInfoDialog::~ServerInfoDialog()
{
    delete ui;
    delete gotifyApi;
}


void ServerInfoDialog::acceptedCallback()
{
    QUrl serverUrl(ui->line_url->text());
    settings->setServerUrl(serverUrl);
    settings->setClientToken(ui->line_token->text().toUtf8());
    settings->setSelfSignedCertificatePath(serverUrl.scheme() == "https" ? certPath : "");
    emit settings->serverChanged();
}


void ServerInfoDialog::enableInputs()
{
    ui->line_url->setEnabled(true);
    ui->line_token->setEnabled(true);
    ui->pb_test->setEnabled(true);
}


void ServerInfoDialog::disableInputs()
{
    ui->line_url->setDisabled(true);
    ui->line_token->setDisabled(true);
    ui->pb_test->setDisabled(true);
}


void ServerInfoDialog::inputChangedCallback()
{
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    Utils::updateWidgetProperty(ui->pb_test, "state", "");
}

void
ServerInfoDialog::urlChangedCallback(QString text)
{
    ui->pb_certificate->setVisible(QUrl(text).scheme() == "https");
}

void
ServerInfoDialog::certificateCallback()
{
    QString path = QFileDialog::getOpenFileName(this, "Import self-signed server certificate", QDir::homePath(), "Certificates (*.pem *.crt);;*");
    if (path.isEmpty())
        return;

    QList<QSslError> expectedErrors = Utils::getSelfSignedExpectedErrors(path);
    if (expectedErrors.size()) {
        certPath = path;
        ui->label_status->setText("Certificate: " + certPath);
        Utils::updateWidgetProperty(ui->pb_certificate, "state", "");
    } else {
        certPath = "";
        ui->label_status->setText("ERROR: Invalid certificate");
        Utils::updateWidgetProperty(ui->pb_certificate, "state", "failed");
    }

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    Utils::updateWidgetProperty(ui->pb_test, "state", "");
}

void
ServerInfoDialog::testCallback()
{
    Utils::updateWidgetProperty(ui->pb_test, "state", "");
    Utils::updateWidgetProperty(ui->line_url, "state", "");
    Utils::updateWidgetProperty(ui->line_token, "state", "");
    Utils::updateWidgetProperty(ui->pb_certificate, "state", "");

    // Clean the url
    QString text = ui->line_url->text();
    while (text.size() > 0 && text.at(text.size()-1) == '/')
        text.chop(1);
    ui->line_url->setText(text);

    QUrl url(text);
    QByteArray clientToken(ui->line_token->text().toUtf8());
    if (url.isEmpty() || clientToken.isEmpty()) {
        return;
    }

    delete gotifyApi;
    gotifyApi = new GotifyApi(url, clientToken, certPath);

    disableInputs();
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);

    // Verify url and clientToken
    QNetworkReply * reply = gotifyApi->messages(1);
    connect(reply, &QNetworkReply::finished, requestHandler, &RequestHandler::testServer);
}

void
ServerInfoDialog::testSuccessCallback()
{
    Utils::updateWidgetProperty(ui->pb_test, "state", "success");
    Utils::updateWidgetProperty(ui->line_token, "state", "success");
    Utils::updateWidgetProperty(ui->line_url, "state", "success");

    ui->label_status->setText("Server information verified");

    setMinimumWidth(width());
    adjustSize();

    enableInputs();
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setFocus();
}

void
ServerInfoDialog::testErrorCallback(QNetworkReply::NetworkError error, QString errorString)
{
    switch (error) {
        case QNetworkReply::AuthenticationRequiredError:
            Utils::updateWidgetProperty(ui->pb_test, "state", "failed");
            Utils::updateWidgetProperty(ui->line_token, "state", "failed");
            Utils::updateWidgetProperty(ui->line_url, "state", "success");
            ui->line_token->setFocus();
            ui->label_status->setText(errorString);
            break;
        default:
            Utils::updateWidgetProperty(ui->pb_test, "state", "failed");
            Utils::updateWidgetProperty(ui->line_token, "state", "");
            Utils::updateWidgetProperty(ui->line_url, "state", "failed");
            ui->line_url->setFocus();
            ui->label_status->setText(errorString);
            break;
    }

    // Resize the widget to fit the error string
    setMinimumWidth(width());
    adjustSize();

    enableInputs();
}
