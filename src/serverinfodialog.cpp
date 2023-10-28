#include "serverinfodialog.h"
#include "ui_serverinfodialog.h"
#include "utils.h"
#include "requesthandler.h"
#include "settings.h"

#include <QDialogButtonBox>


ServerInfoDialog::ServerInfoDialog(QWidget * parent, QUrl url, QByteArray clientToken, bool import) :
    QDialog(parent),
    ui(new Ui::ServerInfoDialog)
{
    ui->setupUi(this);
    ui->line_url->setText(url.toString());
    ui->line_token->setText(clientToken);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    ui->pb_import->setVisible(import);
    setStyleSheet(Utils::readFile(":/res/themes/" + Utils::getTheme() + "/ServerInfoDialog.qss"));

    gotifyApi = new GotifyApi(QUrl(), "");

    connect(requestHandler, &RequestHandler::serverOk, this, &ServerInfoDialog::testSuccessCallback);
    connect(requestHandler, &RequestHandler::replyError, this, &ServerInfoDialog::testErrorCallback);
    connect(requestHandler, &RequestHandler::finished, this, [this]{ui->pb_test->setEnabled(true);});
    connect(this, &QDialog::accepted, this, &ServerInfoDialog::acceptedCallback);
}


ServerInfoDialog::~ServerInfoDialog()
{
    delete ui;
    delete gotifyApi;
}


void ServerInfoDialog::acceptedCallback()
{
    settings->setServerUrl(ui->line_url->text());
    settings->setClientToken(ui->line_token->text().toUtf8());
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


void ServerInfoDialog::importCallback()
{
}


void ServerInfoDialog::testCallback()
{
    Utils::updateWidgetProperty(ui->pb_test, "state", "");
    Utils::updateWidgetProperty(ui->line_url, "state", "");
    Utils::updateWidgetProperty(ui->line_token, "state", "");

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

    gotifyApi->updateAuth(url, clientToken);

    disableInputs();
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);

    // Verify url and clientToken
    QNetworkReply * reply = gotifyApi->messages(1);
    connect(reply, &QNetworkReply::finished, requestHandler, &RequestHandler::testServer);
}


void ServerInfoDialog::testSuccessCallback()
{
    Utils::updateWidgetProperty(ui->pb_test, "state", "success");
    Utils::updateWidgetProperty(ui->line_token, "state", "success");
    Utils::updateWidgetProperty(ui->line_url, "state", "success");

    enableInputs();
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setFocus();
}


void ServerInfoDialog::testErrorCallback(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::AuthenticationRequiredError:
        Utils::updateWidgetProperty(ui->pb_test, "state", "failed");
        Utils::updateWidgetProperty(ui->line_token, "state", "failed");
        Utils::updateWidgetProperty(ui->line_url, "state", "success");
        ui->line_token->setFocus();
        break;
    default:
        Utils::updateWidgetProperty(ui->pb_test, "state", "failed");
        Utils::updateWidgetProperty(ui->line_token, "state", "");
        Utils::updateWidgetProperty(ui->line_url, "state", "failed");
        ui->line_url->setFocus();
        break;
    }
    enableInputs();
}
