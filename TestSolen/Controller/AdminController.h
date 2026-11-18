#ifndef ADMINCONTROLLER_H
#define ADMINCONTROLLER_H

#include <QList>
#include <QObject>

#include "Star.h"
#include "User.h"
#include "UserDatabase.h"
#include "stardatabase.h"

class AdminView;

class AdminController : public QObject
{
    Q_OBJECT

public:
    explicit AdminController(const QString &dbPath, AdminView *adminView, QObject *parent = nullptr);

signals:
    void starsChanged();

private:
    static constexpr int maxAdminStarsToLoad = 10000;

    AdminView *m_adminView;
    UserDatabase m_userDatabase;
    StarDatabase m_starDatabase;
    QList<User> m_users;
    QList<Star> m_stars;

    QList<User> filterUsers(const QString &query) const;
    QList<Star> filterStars(const QString &query) const;
    bool validateStar(const Star &star, const QString &action);

private slots:
    void handleUserSettingsOpened();
    void handleUserSearchChanged(const QString &query);
    void handleUserSelected(const QString &username);
    void handleSaveUserRequested(const User &user, const QString &passwordConfirmation);
    void handleDeleteUserRequested(int userId, const QString &username);
    void handleAddUserRequested(const User &user, const QString &passwordConfirmation);
    void handleStarSettingsOpened();
    void handleStarSearchChanged(const QString &query);
    void handleStarSelected(const QString &starName);
    void handleSaveStarRequested(const Star &star);
    void handleDeleteStarRequested(int starId, const QString &starName);
    void handleAddStarRequested(const Star &star);
};

#endif // ADMINCONTROLLER_H
