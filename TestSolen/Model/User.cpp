#include "User.h"

User::User(int id, const QString &username, const QString &password, bool isAdmin, const QStringList &favoriteStars)
    : m_id(id),
      m_username(username),
      m_password(password),
      m_isAdmin(isAdmin),
      m_favoriteStars(favoriteStars)
{
}

User::User(const QString &username, const QString &password, bool isAdmin, const QStringList &favoriteStars)
    : m_username(username),
      m_password(password),
      m_isAdmin(isAdmin),
      m_favoriteStars(favoriteStars)
{
}

int User::id() const
{
    return m_id;
}

QString User::username() const
{
    return m_username;
}

QString User::password() const
{
    return m_password;
}

bool User::isAdmin() const
{
    return m_isAdmin;
}

QStringList User::favoriteStars() const
{
    return m_favoriteStars;
}

void User::setId(int id)
{
    m_id = id;
}

void User::setUsername(const QString &username)
{
    m_username = username;
}

void User::setPassword(const QString &password)
{
    m_password = password;
}

void User::setIsAdmin(bool isAdmin)
{
    m_isAdmin = isAdmin;
}

void User::setFavoriteStars(const QStringList &favoriteStars)
{
    m_favoriteStars = favoriteStars;
}
