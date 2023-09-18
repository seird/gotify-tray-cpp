#ifndef IMAGEPOPUP_H
#define IMAGEPOPUP_H

#include <QLabel>
#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QMouseEvent>
#include <QUrl>


class ImagePopup : public QLabel
{
    Q_OBJECT
public:
    ImagePopup(QWidget * parent=nullptr);
    void display(const QString& filePath, const QUrl& url, QPoint pos);

    virtual bool eventFilter(QObject * watched, QEvent * event);
    void mousePressEvent(QMouseEvent * event);

public slots:
    void close();

private:
    QTimer timer;
    QUrl url;
    void checkMouse();
};


#endif // IMAGEPOPUP_H
