#include "statuswidget.h"
#include "settings.h"


StatusWidget::StatusWidget(QWidget * parent) : QLabel(parent)
{
    setFixedSize(QSize(20, 20));
    setScaledContents(true);
    image = "";
}


void
StatusWidget::setStatus(QString image)
{
    this->image = image;
    QString resource = ":/res/themes/" + settings->theme() + "/" + image;
    setPixmap(QPixmap(resource));
}


void
StatusWidget::setActive()
{
    setToolTip("Listening for new messages");
    setStatus("status_active.svg");
}


void
StatusWidget::setConnecting()
{
    setToolTip("Connecting...");
    setStatus("status_connecting.svg");
}


void
StatusWidget::setInactive()
{
    setToolTip("Listener inactive.");
    setStatus("status_inactive.svg");
}


void
StatusWidget::setError()
{
    setToolTip("Listener error");
    setStatus("status_error.svg");
}


void
StatusWidget::refresh()
{
    if (image != "") {
        setStatus(image);
    }
}
