#include "utils.h"

#include <QApplication>
#include <QStyleHints>
#include <QStyle>
#include <QFile>
#include <QDirIterator>


namespace Utils
{


QString getTheme()
{
    switch (qApp->styleHints()->colorScheme())
    {
    case Qt::ColorScheme::Dark:
        return QStringLiteral("dark");
    case Qt::ColorScheme::Light:
        return QStringLiteral("light");
    default:
        // fallback to custom method
        return (qApp->palette().color(QPalette::Active, QPalette::Base).lightness() < 127) ? QStringLiteral("dark") : QStringLiteral("light");
    }
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
