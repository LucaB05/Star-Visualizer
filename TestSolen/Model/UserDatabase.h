#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QList>
#include <QSqlDatabase>
#include "User.h"

class UserDatabase
{
public:
    explicit UserDatabase(const QString &path);
    ~UserDatabase();

    bool openConnection();
    bool addUser(const User &user);
    bool editUser(const User &user);
    bool deleteUser(const QString &name);
    QList<User> getAllUsers() const;
    User findUserByName(const QString &name) const;
    bool addFavorite(int userId, int starId);
    bool removeFavorite(int userId, int starId);
    bool isFavorite(int userId, int starId) const;
    QStringList getFavoriteStarNames(int userId) const;

private:
    QString m_dbPath;
    QString m_connectionName;
    QSqlDatabase m_db;
};

#endif // USERDATABASE_H
