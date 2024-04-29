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

    setFonts();
    setIcons();
    setPriorityColor(item->priority());

    // Application Icon
    QPixmap pixmap = icon.pixmap(settings->messageWidgetImageSize());
    ui->label_image->setPixmap(pixmap);

    // Title
    ui->label_title->setText(item->title());

    // Date
    ui->label_date->setText(
        (settings->useLocale() ?
            QLocale::system().toString(item->date(), QLocale::FormatType::ShortFormat) :
            item->date().toString("yyyy-MM-dd, hh:mm"))
        + " "
    );

    // Message -- if it's an image, display it in the message label
    QString image = settings->showImageUrl() ? Utils::extractImage(item->message()) : QString();
    if (!image.isNull()) {
        setImage(image);
    } else {
        ui->label_message->setText(replaceLinks(item->message()));
        if (item->markdown())
            ui->label_message->setTextFormat(Qt::MarkdownText);
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


void MessageWidget::setImage(QString url)
{
    QString filePath = cache->getFile(url);
    if (filePath.isNull()) {
        // Make a get request
        if (!manager) manager = new QNetworkAccessManager();
        QNetworkRequest request;
        QEventLoop eventLoop;
        request.setUrl(QUrl(url));
        QNetworkReply * reply = manager->get(request);
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

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


QString MessageWidget::replaceLinks(QString text)
{
    static QRegularExpression re("(https?)(://\\S+)");
    return text.replace(re, "<a href=\"\\1\\2\">\\1\\2</a>");
}
