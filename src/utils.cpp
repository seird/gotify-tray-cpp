#include "utils.h"

#include <QApplication>
#include <QStyleHints>
#include <QStyle>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QUuid>
#include <QDirIterator>


namespace Utils
{


QString getTheme()
{
    return qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark ? QStringLiteral("dark") : QStringLiteral("light");
}


void updateWidgetProperty(QWidget * widget, const char *name, const QVariant &value)
{
    widget->setProperty(name, value);
    QStyle * style = widget->style();
    style->unpolish(widget);
    style->polish(widget);
    widget->update();
}


QString readFile(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Could not open " << fileName;
        return QString();
    }
    QString text = file.readAll();
    file.close();
    return text;
}


void writeFile(QString fileName, QByteArray data)
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();
}


QString extractImage(QString text)
{
    static QRegularExpression reMD("(\\[.*?\\]\\((.+?://.+?)\\))");
    QRegularExpressionMatch match = reMD.match(text);
    if (match.hasMatch()) {
        text = match.captured(2);
    }

    static QRegularExpression re("(https?)(://\\S+).(jpg|jpeg|png|gif|svg|webp|ico|tiff|bmp)(\\s|$)");
    match = re.match(text);
    if (match.hasMatch() && match.captured() == text) { // how to regex..
        return match.captured();
    }

    return QString();
}


QString getUuid()
{
    return QUuid::createUuid().toString(QUuid::Id128);
}


bool isImage(const QString& fileName)
{
    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    static QStringList imageExts = {"jpg", "jpeg", "png", "gif", "svg", "webp", "ico", "tiff", "bmp"};
    return imageExts.contains(ext);
}


qint64 dirSize(const QString& dirName)
{
    QDirIterator it(dirName, QDirIterator::Subdirectories);
    qint64 size = 0;
    while (it.hasNext()) {
        it.next();
        size += it.fileInfo().size();
    }
    return size;
}


}
