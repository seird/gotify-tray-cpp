#ifndef UTILS_H
#define UTILS_H


#include <QWidget>


namespace Utils
{

void updateWidgetProperty(QWidget * widget, const char *name, const QVariant &value);
QString readFile(QString fileName);
void writeFile(QString fileName, QByteArray data);
QString extractImage(QString text);
QString getUuid();
bool isImage(const QString& fileName);

}


#endif // UTILS_H
