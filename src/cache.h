#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class Cache : QObject
{
    Q_OBJECT
public:
    Cache(QObject * parent = nullptr);
    ~Cache();
    static Cache * getInstance();

    QString getDir() const { return directory; };
    QString getFilesDir() const { return filesDirectory; };

    QString lookup(QVariant key);
    bool store(QVariant key, QString value);
    void clear();
    /**
     * @brief Get the cache size in bytes
     * @return
     */
    qint64 size();

    /**
     * @brief getFile Returns the associated cache value prepended with the filesDirectory
     * @param key
     * @return
     */
    QString getFile(QVariant key);

private:
    QString directory;
    QString filesDirectory;
    QSqlDatabase database;
};


extern Cache * cache;


#endif // CACHE_H
