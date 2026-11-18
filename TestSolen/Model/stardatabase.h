#ifndef STARDATABASE_H
#define STARDATABASE_H

#include <QSqlDatabase>
#include <QList>
#include "Star.h"

class StarDatabase {
public:
    StarDatabase(const QString& path);

    bool openConnection();
    QList<Star> fetchAllStars();
    QList<Star> fetchStars(int limit);
    Star findStarByName(const QString &name) const;
    int addStar(const Star &star);
    bool updateStar(const Star &star);
    bool deleteStar(int id);

private:
    QString m_dbPath;
    QString m_connectionName;
    QSqlDatabase m_db;
};

#endif
