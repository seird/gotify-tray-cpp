#include "imagepopup.h"
#include "settings.h"

#include <QDesktopServices>


ImagePopup::ImagePopup(QWidget * parent)
    : QLabel(parent, Qt::Popup),
      url(QString())
{
    installEventFilter(this);
    connect(&timer, &QTimer::timeout, this, &ImagePopup::checkMouse);
}


void ImagePopup::display(const QString& filePath, const QUrl& url, QPoint pos)
{
    close();

    this->url = url;

    QPixmap pixmap(filePath);
    int W = settings->popupWidth();
    int H = settings->popupHeight();
    if (pixmap.height() > H || pixmap.width() > W)
        pixmap = pixmap.scaled(W, H, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(pixmap);
    move(pos - QPoint(25, 25));

    timer.start(500);

    show();
}


void ImagePopup::checkMouse()
{
    if (!underMouse())
        close();
}


void ImagePopup::close()
{
    timer.stop();
    url = QString();
    QLabel::close();
}


void ImagePopup::mousePressEvent(QMouseEvent * event)
{
    if (!url.isEmpty() && event->button() == Qt::MouseButton::LeftButton)
        QDesktopServices::openUrl(url);

    QLabel::mousePressEvent(event);
}


bool ImagePopup::eventFilter(QObject * obj, QEvent * event)
{
    switch (event->type()) {
#ifndef Q_OS_MAC
    case QEvent::Leave:
        close();
        break;
#endif
    case QEvent::KeyPress: {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            close();
        }
        break;
    }
    default:
        break;
    }

    return QLabel::eventFilter(obj, event);
}
