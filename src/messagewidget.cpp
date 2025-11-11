#include "messagewidget.h"
#include "cache.h"
#include "requesthandler.h"
#include "settings.h"
#include "ui_messagewidget.h"
#include "utils.h"

#include <QClipboard>
#include <QEventLoop>
#include <QFileInfo>
#include <QListView>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSize>

#define MAX(x, y) ((x) > (y) ? (x) : (y))


MessageWidget::MessageWidget(MessageItem * item, QIcon icon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWidget),
    manager(nullptr)
{
    ui->setupUi(this);

    setFonts();
    setIcons();
    setPriorityColor(item->priority());

    // Application Icon
    QPixmap pixmap = icon.pixmap(settings->messageWidgetImageSize());
    ui->label_image->setPixmap(pixmap);

    // Title
    ui->label_title->setText(item->title());

    // Date
    ui->label_date->setText((settings->useLocale() ? QLocale::system().toString(item->date(), QLocale::FormatType::ShortFormat) : item->date().toString("yyyy-MM-dd, hh:mm")) + " ");

    // Message contents
    ui->frame_content_image->hide();
    ui->label_message_fallback->hide();
    if (settings->forcePlainText()) {
        ui->label_message->setText(item->message());
        ui->label_message->setTextFormat(Qt::PlainText);
    } else {
        // Message image: if the text contains an image url, display it in the content_image label
        QString image = Utils::extractImage(item->message());
        if (settings->showImageUrl() && !image.isNull())
            setImage(image);

        // Message text
        int allowedWidth = parentWidget()->width() - ui->label_image->width() - ui->label_priority->width() - 40;
        QString text = item->message();
        if (settings->showImageUrl() && imageUrl == text)
            ui->label_message->hide();
        else if (settings->messageFallback() && Utils::violatesWidth(text, ui->label_message->font(), allowedWidth)) {
            ui->label_message_fallback->setText(text);
            ui->label_message_fallback->show();
            ui->label_message->hide();
        } else {
            if (settings->renderMarkdown() && item->markdown()) {
                ui->label_message->setTextFormat(Qt::MarkdownText);
                ui->label_message->setText(text);
            } else {
                if (!Utils::containsHtml(item->message()))
                    text = Utils::replaceLinks(item->message());
                ui->label_message->setText(text.replace("\n", "<br>"));
            }
        }
    }

    // Size
    adjustSize();
    int minHeight = settings->messageWidgetHeightMin();
    item->setSizeHint(QSize(item->sizeHint().width(), MAX(minHeight, height())));

    connect(ui->label_message, &QLabel::linkHovered, this, &MessageWidget::linkHoveredCallback);
}

MessageWidget::~MessageWidget()
{
    delete ui;
    if (manager) delete manager;
}


void MessageWidget::setFonts()
{
    ui->label_title->setFont(settings->titleFont());
    ui->label_date->setFont(settings->dateFont());
    ui->label_message->setFont(settings->messageFont());
}


void MessageWidget::setIcons()
{
    ui->pb_delete->setIcon(QIcon(":/res/themes/" + Utils::getTheme() + "/trashcan.svg"));
    ui->label_image->setFixedSize(settings->messageApplicationIconSize());
    ui->pb_delete->setIconSize(0.9*settings->messageButtonSize());
    ui->pb_delete->setFixedSize(settings->messageButtonSize());
}

bool
MessageWidget::setImage(QString url)
{
    QString filePath = cache->getFile(url);
    if (filePath.isNull()) {
        // Make a get request
        if (!manager) manager = new QNetworkAccessManager();
        QNetworkRequest request;
        QEventLoop eventLoop;
        request.setUrl(QUrl(url));
        QNetworkReply* reply = manager->get(request);
        if (QUrl(url).scheme() == "https" && !settings->selfSignedCertificatePath().isEmpty())
            reply->ignoreSslErrors(Utils::getSelfSignedExpectedErrors(settings->selfSignedCertificatePath()));
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec(); // TODO: this should run in a non-blocking way. Note that adjustSize() must be called AFTER setting the content_image label

        if (reply->error() != QNetworkReply::NetworkError::NoError) {
            qDebug() << reply->errorString();
            reply->deleteLater();
            return false;
        }

        // Write the file and store in cache
        QString fileName = Utils::getUuid();
        filePath = cache->getFilesDir() + fileName;
        Utils::writeFile(filePath, reply->readAll());
        cache->store(url, fileName);
        reply->deleteLater();
    }

    // Set the image
    QPixmap pixmap(filePath);
    float W = settings->messageWidgetContentImageWidth();
    float H = settings->messageWidgetContentImageHeight();
    QListView* lv = static_cast<QListView*>(parent());
    W *= lv->width() - ui->label_content_image->width();
    H *= lv->height();

    if (pixmap.width() > W || pixmap.height() > H)
        ui->label_content_image->setPixmap(pixmap.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        ui->label_content_image->setPixmap(pixmap);

    ui->frame_content_image->show();
    imageUrl = url;

    return true;
}


void MessageWidget::deleteCallback()
{
    emit deletionRequested();
}

void
MessageWidget::clickedContentImage()
{
    QGuiApplication::clipboard()->setText(imageUrl);
}

void MessageWidget::linkHoveredCallback(const QString& link)
{
    if (!settings->popupEnabled())
        return;

    QUrl url(link);
    if (Utils::isImage(url.fileName())) {
        QPoint pos = QCursor::pos();

        QString filePath = cache->getFile(link);
        if (!filePath.isNull()) {
            emit requestHandler->finishedImagePopup(filePath, url, pos);
        } else {
            if (!manager) manager = new QNetworkAccessManager();
            QNetworkRequest request;
            request.setUrl(url);
            QNetworkReply* reply = manager->get(request);
            if (url.scheme() == "https" && !settings->selfSignedCertificatePath().isEmpty())
                reply->ignoreSslErrors(Utils::getSelfSignedExpectedErrors(settings->selfSignedCertificatePath()));
            connect(reply, &QNetworkReply::finished, requestHandler, [this, pos] { requestHandler->imagePopup(pos); });
        }
    }
}


void MessageWidget::showPriority(bool enabled)
{
    ui->label_priority->setFixedWidth(settings->priorityColorWidth()*enabled);
}


void MessageWidget::setPriorityColor(int priority)
{
    showPriority(settings->priorityColor());

    if (priority >= 4 && priority <= 7)
        ui->label_priority->setStyleSheet("background-color: #b3e67e22;");
    else if (priority > 7)
        ui->label_priority->setStyleSheet("background-color: #e74c3c;");
}
