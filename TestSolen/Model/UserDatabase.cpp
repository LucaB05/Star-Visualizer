 #include "UserDatabase.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QDir>

UserDatabase::UserDatabase(const QString &path)
    : m_dbPath(path),
      m_connectionName("user_connection")
{
    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    }

    m_db.setDatabaseName(m_dbPath);
}

UserDatabase::~UserDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool UserDatabase::openConnection()
{
    qDebug() << QDir().absolutePath();
    if (!m_db.open()) {
        qDebug() << "Error: user database connection failed" << m_db.lastError();
        return false;
    }

    qDebug() << "User database connection successful!";
    qDebug() << "Database file being used:" << m_db.databaseName();
    return true;
}

bool UserDatabase::addUser(const User &user)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (name, password, isAdmin) VALUES (:name, :password, :isAdmin)");
    query.bindValue(":name", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":isAdmin", user.isAdmin() ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "SQL ERROR addUser:" << query.lastError().text();
        return false;
    }

    return true;
}

bool UserDatabase::editUser(const User &user)
{
    QSqlQuery query(m_db);

    if (user.id() >= 0) {
        query.prepare("UPDATE users SET name = :name, password = :password, isAdmin = :isAdmin WHERE id = :id");
        query.bindValue(":id", user.id());
    } else {
        query.prepare("UPDATE users SET password = :password, isAdmin = :isAdmin WHERE name = :name");
    }

    query.bindValue(":name", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":isAdmin", user.isAdmin() ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "SQL ERROR editUser:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool UserDatabase::deleteUser(const QString &name)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "SQL ERROR deleteUser:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<User> UserDatabase::getAllUsers() const
{
    QList<User> users;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, password, isAdmin FROM users ORDER BY name COLLATE NOCASE");

    if (!query.exec()) {
        qDebug() << "SQL ERROR getAllUsers:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        users.append(User(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toInt() != 0,
            {}
        ));
    }

    return users;
}

User UserDatabase::findUserByName(const QString &name) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, password, isAdmin FROM users WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "SQL ERROR findUserByName:" << query.lastError().text();
        return User();
    }

    if (!query.next()) {
        return User();
    }

    return User(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toInt() != 0,
        {}
        );
}

bool UserDatabase::addFavorite(int userId, int starId)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO favorites (userId, starId) VALUES (:userId, :starId)");
    query.bindValue(":userId", userId);
    query.bindValue(":starId", starId);

    if (!query.exec()) {
        qDebug() << "SQL ERROR addFavorite:" << query.lastError().text();
        return false;
    }

    return true;
}

bool UserDatabase::removeFavorite(int userId, int starId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM favorites WHERE userId = :userId AND starId = :starId");
    query.bindValue(":userId", userId);
    query.bindValue(":starId", starId);

    if (!query.exec()) {
        qDebug() << "SQL ERROR removeFavorite:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool UserDatabase::isFavorite(int userId, int starId) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT 1 FROM favorites WHERE userId = :userId AND starId = :starId LIMIT 1");
    query.bindValue(":userId", userId);
    query.bindValue(":starId", starId);

    if (!query.exec()) {
        qDebug() << "SQL ERROR isFavorite:" << query.lastError().text();
        return false;
    }

    return query.next();
}

QStringList UserDatabase::getFavoriteStarNames(int userId) const
{
    QStringList favorites;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT stars.name "
        "FROM favorites "
        "INNER JOIN stars ON favorites.starId = stars.id "
        "WHERE favorites.userId = :userId "
        "ORDER BY stars.name COLLATE NOCASE"
        );
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "SQL ERROR getFavoriteStarNames:" << query.lastError().text();
        return favorites;
    }

    while (query.next()) {
        favorites << query.value(0).toString();
    }

    return favorites;
}
