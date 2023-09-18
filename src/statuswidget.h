#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H


#include <QWidget>
#include <QLabel>
#include <QSize>
#include <QString>


class StatusWidget : public QLabel
{
public:
    StatusWidget(QWidget * parent = nullptr);
    void setActive();
    void setConnecting();
    void setInactive();
    void setError();
    void refresh();

private:
    QString image;
    void setStatus(QString image);
};


#endif // STATUSWIDGET_H
