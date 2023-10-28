#ifndef UTILS_H
#define UTILS_H


#include <QWidget>


namespace Utils
{

QString getTheme();
void updateWidgetProperty(QWidget * widget, const char *name, const QVariant &value);
QString readFile(QString fileName);
void writeFile(QString fileName, QByteArray data);
QString extractImage(QString text);
QString getUuid();
bool isImage(const QString& fileName);
qint64 dirSize(const QString& dirName);

}


#endif // UTILS_H
