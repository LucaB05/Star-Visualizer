#ifndef USER_H
#define USER_H

#include <QString>
#include <QStringList>

class User
{
public:
    User() = default;
    User(int id, const QString &username, const QString &password, bool isAdmin, const QStringList &favoriteStars);
    User(const QString &username, const QString &password, bool isAdmin, const QStringList &favoriteStars);

    int id() const;
    QString username() const;
    QString password() const;
    bool isAdmin() const;
    QStringList favoriteStars() const;

    void setId(int id);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setIsAdmin(bool isAdmin);
    void setFavoriteStars(const QStringList &favoriteStars);

private:
    int m_id = -1;
    QString m_username;
    QString m_password;
    bool m_isAdmin = false;
    QStringList m_favoriteStars;
};

#endif // USER_H
