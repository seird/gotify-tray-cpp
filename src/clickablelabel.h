#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H


#include <QLabel>
#include <QObject>


class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget * parent = nullptr, Qt::WindowFlags f=Qt::WindowFlags());

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

};


#endif // CLICKABLELABEL_H
