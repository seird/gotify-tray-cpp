#include "utils.h"

#include <QApplication>
#include <QStyleHints>
#include <QStyle>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QUuid>

#ifdef USE_KDE
#include <KNotification>
#endif

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


QString extractImage(QString text)
{
    text = text.trimmed();

    // Check for markdown image format: ![alt text](image_url)
    static QRegularExpression reMD(R"(^\!\[.*?\]\((https?://[^\s]+\.(jpg|jpeg|png|gif|svg|webp|ico|tiff|bmp))\)$)");
    QRegularExpressionMatch matchMD = reMD.match(text);
    if (matchMD.hasMatch()) {
        return matchMD.captured(1); // Return the image URL
    }

    // Check for plain image URL format
    static QRegularExpression rePlain(R"(^https?://[^\s]+\.(jpg|jpeg|png|gif|svg|webp|ico|tiff|bmp)$)");
    QRegularExpressionMatch matchPlain = rePlain.match(text);
    if (matchPlain.hasMatch()) {
        return matchPlain.captured(0); // Return the image URL
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

#ifdef USE_KDE
KNotification::Urgency
priorityToUrgency(int priority)
{
    switch (priority) {
        case 0 ... 3:
            return KNotification::Urgency::LowUrgency;
        case 4 ... 6:
            return KNotification::Urgency::NormalUrgency;
        case 7 ... 9:
            return KNotification::Urgency::HighUrgency;
        case 10:
            return KNotification::Urgency::CriticalUrgency;
        default:
            return KNotification::Urgency::NormalUrgency;
    }
}
#endif // USE_KDE

}
