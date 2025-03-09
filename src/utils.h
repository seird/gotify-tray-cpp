#ifndef UTILS_H
#define UTILS_H

#include <QSslError>
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

bool
writeFile(QString fileName, QByteArray data);

QString
extractImage(QString text);

QString
replaceLinks(QString text);

bool
containsHtml(QString text);

bool
violatesWidth(const QString& text, const QFont& font, int allowedWidth);

QString
getUuid();

bool
isImage(const QString& fileName);

qint64
dirSize(const QString& dirName);

QList<QSslError>
getSelfSignedExpectedErrors(QString certPath);

#ifdef USE_KDE
KNotification::Urgency
priorityToUrgency(int priority);
#endif

}

#endif // UTILS_H
