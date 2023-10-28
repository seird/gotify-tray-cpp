#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QStandardItem>
#include <QNetworkAccessManager>

#include "messageitem.h"


namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(MessageItem * item, QIcon icon, QWidget *parent = nullptr);
    ~MessageWidget();
    void setIcons(QString theme);

signals:
    void deletionRequested();

private slots:
    void deleteCallback();
    void linkHoveredCallback(const QString& link);

private:
    Ui::MessageWidget * ui;
    QNetworkAccessManager * manager;

    void setImage(QString filePath);
    QString replaceLinks(QString text);
    void setPriorityColor(int priority);
};

#endif // MESSAGEWIDGET_H