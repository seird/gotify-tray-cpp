#include "cache.h"

#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>


Cache * cache = nullptr;


Cache::Cache(QObject * parent)
    : QObject(parent)
{
    directory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";
    if (!QDir().mkpath(directory)) {
        qWarning() << "Cache directory could not be created: " + directory;
        directory = "./cache";
        if (!QDir().mkpath(directory)) {
            qFatal() << "Cache directory could not be created: " + directory;
        }
    }

    filesDirectory = directory + "files/";
    if (!QDir().mkpath(filesDirectory)) {
        qFatal() << "Cache files directory could not be created: " + filesDirectory;
    }

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(directory + "cache.db.sqlite3");
    database.open();
    database.exec("CREATE TABLE IF NOT EXISTS cache ("
                  "id        INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "key       TEXT UNIQUE,"
                  "value     TEXT)");}


Cache::~Cache()
{
    database.removeDatabase("QSQLITE");
    database.close();
}


Cache * Cache::getInstance()
{
    if(!cache) {
        cache = new Cache();
    }
    return cache;
}


void Cache::clear()
{
    database.exec("DELETE FROM cache");

    QDir dir(filesDirectory);
    dir.removeRecursively();
    dir.mkdir(filesDirectory);
}


bool Cache::store(QVariant key, QString value)
{
    QSqlQuery query(database);
    query.prepare("INSERT OR REPLACE INTO cache (key, value)"
                  "VALUES(:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);
    bool result = query.exec();
    return result;
}


QString Cache::lookup(QVariant key)
{
    QSqlQuery query(database);
    query.prepare("SELECT value FROM cache WHERE key=:key");
    query.bindValue(":key", key);
    query.exec();
    // QSqlQuery's internal pointer is located one position before the first record.
    return query.next() ? query.value(0).toString() : QString();
}


QString Cache::getFile(QVariant key)
{
    QString result = lookup(key);
    return result.isNull() ? QString() : filesDirectory + result;
}
