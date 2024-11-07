#ifndef UTILS_H
#define UTILS_H

#include <QWidget>

#ifdef USE_KDE
#include <KNotification>
#endif

namespace Utils {

QString
getTheme();

void
updateWidgetProperty(QWidget* widget, const char* name, const QVariant& value);

QString
readFile(QString fileName);

void
writeFile(QString fileName, QByteArray data);

qint64
dirSize(const QString& dirName);

#ifdef USE_KDE
KNotification::Urgency
priorityToUrgency(int priority);
#endif

}

#endif // UTILS_H
