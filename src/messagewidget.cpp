#include "messagewidget.h"
#include "ui_messagewidget.h"
#include "settings.h"
#include "utils.h"
#include "cache.h"
#include "requesthandler.h"

#include <QRegularExpression>
#include <QLocale>
#include <QSize>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QListView>


#define MAX(x, y) ((x) > (y) ? (x) : (y))


MessageWidget::MessageWidget(MessageItem * item, QIcon icon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWidget),
    manager(nullptr)
{
    ui->setupUi(this);

    ui->label_title->setFont(settings->titleFont());
    ui->label_date->setFont(settings->dateFont());
    ui->label_message->setFont(settings->messageFont());

    ui->label_title->setText(item->title());

    QString dateStr;
    if (settings->useLocale())
        dateStr = QLocale::system().toString(item->date(), QLocale::FormatType::ShortFormat);
    else
        dateStr = item->date().toString("yyyy-MM-dd, hh:mm");
    ui->label_date->setText(dateStr + " ");

    QString image = Utils::extractImage(item->message());
    if (!image.isNull()) {
        QString filePath = cache->getFile(image);
        if (!filePath.isNull()) {
            setImage(filePath);
        } else {
            // Make a get request
            if (!manager) manager = new QNetworkAccessManager();
            QNetworkRequest request;
            QEventLoop eventLoop;
            request.setUrl(QUrl(image));
            QNetworkReply * reply = manager->get(request);
            connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
            eventLoop.exec();

            // Write the file and store in cache
            QString fileName = Utils::getUuid();
            QString filePath = cache->getFilesDir() + fileName;
            Utils::writeFile(filePath, reply->readAll());
            cache->store(image, fileName);
            reply->deleteLater();

            // Set the image
            setImage(filePath);
        }
    } else {
        ui->label_message->setText(replaceLinks(item->message()));
        if (item->markdown())
            ui->label_message->setTextFormat(Qt::MarkdownText);
    }

    QPixmap pixmap = icon.pixmap(settings->messageWidgetImageSize());
    ui->label_image->setPixmap(pixmap);

    adjustSize();
    int minHeight = settings->messageWidgetHeightMin();
    item->setSizeHint(QSize(item->sizeHint().width(), MAX(minHeight, height())));

    setPriorityColor(item->priority());

    connect(ui->label_message, &QLabel::linkHovered, this, &MessageWidget::linkHoveredCallback);
}

MessageWidget::~MessageWidget()
{
    delete ui;
    if (manager) delete manager;
}


void MessageWidget::setIcons(QString theme)
{
    ui->pb_delete->setIcon(QIcon(":/res/themes/" + theme + "/trashcan.svg"));
    ui->label_image->setFixedSize(settings->messageApplicationIconSize());
    ui->pb_delete->setIconSize(0.9*settings->messageButtonSize());
    ui->pb_delete->setFixedSize(settings->messageButtonSize());
}


void MessageWidget::setImage(QString fileName)
{
    QPixmap pixmap(fileName);
    float W = settings->messageWidgetContentImageWidth();
    float H = settings->messageWidgetContentImageHeight();
    QListView * lv = static_cast<QListView *>(parent());
    W *= lv->width() - ui->label_image->width();
    H *= lv->height();

    if (pixmap.width() > W || pixmap.height() > H)
        ui->label_message->setPixmap(pixmap.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        ui->label_message->setPixmap(pixmap);

    adjustSize();
}


void MessageWidget::deleteCallback()
{
    emit deletionRequested();
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
            QNetworkReply * reply = manager->get(request);
            connect(reply, &QNetworkReply::finished, requestHandler, [this, pos]{requestHandler->imagePopup(pos);});
        }
    }
}


void MessageWidget::setPriorityColor(int priority)
{
    if (!settings->priorityColor()) {
        ui->label_priority->setFixedWidth(0);
        return;
    }

    if (priority >= 4 && priority <= 7)
        ui->label_priority->setStyleSheet("background-color: #b3e67e22;");
    else if (priority > 7)
        ui->label_priority->setStyleSheet("background-color: #e74c3c;");
}


QString MessageWidget::replaceLinks(QString text)
{
    static QRegularExpression re("(https?)(://\\S+)");
    return text.replace(re, "<a href=\"\\1\\2\">\\1\\2</a>");
}
