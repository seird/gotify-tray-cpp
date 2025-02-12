#include "utils.h"

#include <QApplication>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStyle>
#include <QStyleHints>
#include <QUuid>

#ifdef USE_KDE
#include <KNotification>
#endif

namespace Utils {

QString
getTheme()
{
    switch (qApp->styleHints()->colorScheme()) {
        case Qt::ColorScheme::Dark:
            return QStringLiteral("dark");
        case Qt::ColorScheme::Light:
            return QStringLiteral("light");
        default:
            // fallback to custom method
            return (qApp->palette().color(QPalette::Active, QPalette::Base).lightness() < 127) ? QStringLiteral("dark") : QStringLiteral("light");
    }
}

void
updateWidgetProperty(QWidget* widget, const char* name, const QVariant& value)
{
    widget->setProperty(name, value);
    QStyle* style = widget->style();
    style->unpolish(widget);
    style->polish(widget);
    widget->update();
}

QString
readFile(QString fileName)
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

bool
writeFile(QString fileName, QByteArray data)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        file.write(data);
        file.close();
        return true;
    }
    return false;
}

QString
extractImage(QString text)
{
    text = text.trimmed();

    // Check for markdown image format: ![alt text](image_url)
    QRegularExpression reMD(R"(!\[\]\((https?://[^\s\)]+)\))");
    QRegularExpressionMatch matchMD = reMD.match(text);
    if (matchMD.hasMatch()) {
        return matchMD.captured(1).trimmed(); // Return the image URL
    }

    // Check for plain image URL format
    static QRegularExpression rePlain(R"(\s*(https?://[^\s]+\.(jpg|jpeg|png|gif|svg|webp|ico|tiff|bmp))\s*)");
    QRegularExpressionMatch matchPlain = rePlain.match(text);
    if (matchPlain.hasMatch()) {
        return matchPlain.captured(0).trimmed(); // Return the image URL
    }

    return QString();
}


QString replaceLinks(QString text)
{
    static QRegularExpression re("(https?)(://[^\\s)]+)(?=\\)?)");
    return text.replace(re, "<a href='\\1\\2'>\\1\\2</a>");
}


bool containsHtml(QString text)
{
    static QRegularExpression regex(R"(<([a-zA-Z][a-zA-Z0-9]*)\b[^>]*>.*<\/\1>)");
    QRegularExpressionMatch matchHTML = regex.match(text);
    return matchHTML.hasMatch();
}


QString getUuid()
{
    return QUuid::createUuid().toString(QUuid::Id128);
}

bool
isImage(const QString& fileName)
{
    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    static QStringList imageExts = { "jpg", "jpeg", "png", "gif", "svg", "webp", "ico", "tiff", "bmp" };
    return imageExts.contains(ext);
}

qint64
dirSize(const QString& dirName)
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
