#include "StarDatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QUuid>

StarDatabase::StarDatabase(const QString& path)
    : m_dbPath(path),
      m_connectionName("star_connection_" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(m_dbPath);
}

bool StarDatabase::openConnection() {
    qDebug() << QDir().absolutePath();
    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed" << m_db.lastError();
        return false;
    }
    QStringList tables = m_db.tables();
    qDebug() << "Connection successful!";
    qDebug() << "Database file being used:" << m_db.databaseName();
    qDebug() << "Tables found:" << tables;

    if (tables.isEmpty()) {
        qDebug() << "The database is EMPTY (no tables). Check your file path!";
    }

    return true;
}

QList<Star> StarDatabase::fetchAllStars() {
    QList<Star> stars;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, ra, dec, parallax, v_mag, b_mag, spectral_type FROM stars");

    if (!query.exec()) {
        qDebug() << "SQL ERROR:" << query.lastError().text();
        return stars;
    }

    int count = 0;


    while (query.next()) {
        //qDebug() << "COUNT: " << count;
        count++;
        Star star = Star(
                       query.value(0).toInt(),    //id
                       query.value(1).toString(), // name
                       query.value(2).toFloat(),  // ra
                       query.value(3).toFloat(),  // dec
                       query.value(4).toFloat(),  // parallax
                       query.value(5).toFloat(),  // v_mag
                       query.value(6).toFloat(),  // b_mag
                       query.value(7).toString()  // spectral_type
            );
        star.set_temp();
        star.set_baseColor();
        stars.append(star);

    }
    qDebug() << "Successfully built" << count << "Star objects from DB.";
    return stars;
}

QList<Star> StarDatabase::fetchStars(int limit)
{
    QList<Star> stars;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, name, ra, dec, parallax, v_mag, b_mag, spectral_type "
        "FROM stars "
        "ORDER BY name COLLATE NOCASE "
        "LIMIT :limit"
        );
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qDebug() << "SQL ERROR fetchStars:" << query.lastError().text();
        return stars;
    }

    while (query.next()) {
        Star star(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toFloat(),
            query.value(3).toFloat(),
            query.value(4).toFloat(),
            query.value(5).toFloat(),
            query.value(6).toFloat(),
            query.value(7).toString()
            );
        star.set_temp();
        star.set_baseColor();
        stars.append(star);
    }

    return stars;
}

Star StarDatabase::findStarByName(const QString &name) const
{
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, name, ra, dec, parallax, v_mag, b_mag, spectral_type "
        "FROM stars "
        "WHERE name = :name COLLATE NOCASE"
        );
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "SQL ERROR findStarByName:" << query.lastError().text();
        return Star();
    }

    if (!query.next()) {
        return Star();
    }

    Star star(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toFloat(),
        query.value(3).toFloat(),
        query.value(4).toFloat(),
        query.value(5).toFloat(),
        query.value(6).toFloat(),
        query.value(7).toString()
        );
    star.set_temp();
    star.set_baseColor();
    return star;
}

int StarDatabase::addStar(const Star &star)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO stars (name, ra, dec, parallax, v_mag, b_mag, spectral_type) "
        "VALUES (:name, :ra, :dec, :parallax, :vMag, :bMag, :spectralType)"
        );
    query.bindValue(":name", star.get_name());
    query.bindValue(":ra", star.get_ra());
    query.bindValue(":dec", star.get_dec());
    query.bindValue(":parallax", star.get_parallax());
    query.bindValue(":vMag", star.get_vMag());
    query.bindValue(":bMag", star.get_bMag());
    query.bindValue(":spectralType", star.get_spectralType());

    if (!query.exec()) {
        qDebug() << "SQL ERROR addStar:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool StarDatabase::updateStar(const Star &star)
{
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE stars "
        "SET name = :name, ra = :ra, dec = :dec, parallax = :parallax, "
        "v_mag = :vMag, b_mag = :bMag, spectral_type = :spectralType "
        "WHERE id = :id"
        );
    query.bindValue(":id", star.get_id());
    query.bindValue(":name", star.get_name());
    query.bindValue(":ra", star.get_ra());
    query.bindValue(":dec", star.get_dec());
    query.bindValue(":parallax", star.get_parallax());
    query.bindValue(":vMag", star.get_vMag());
    query.bindValue(":bMag", star.get_bMag());
    query.bindValue(":spectralType", star.get_spectralType());

    if (!query.exec()) {
        qDebug() << "SQL ERROR updateStar:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool StarDatabase::deleteStar(int id)
{
    QSqlQuery deleteFavoritesQuery(m_db);
    deleteFavoritesQuery.prepare("DELETE FROM favorites WHERE starId = :id");
    deleteFavoritesQuery.bindValue(":id", id);

    if (!deleteFavoritesQuery.exec()) {
        qDebug() << "SQL ERROR deleteStar favorites:" << deleteFavoritesQuery.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM stars WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SQL ERROR deleteStar:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
