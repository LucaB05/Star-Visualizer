#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QString>
#include "User.h"
#include "UserDatabase.h"

class AppNavigator;
class StarController;
class QTimer;

class AuthController : public QObject
{
    Q_OBJECT

public:
    explicit AuthController(const QString &dbPath,
                            AppNavigator *appNavigator,
                            StarController *starController,
                            QObject *parent = nullptr,
                            int inactivityTimeoutMs = 5 * 60 * 1000);
    bool isLoggedIn() const;
    User currentUser() const;
    bool addFavoriteForCurrentUser(int starId);
    bool removeFavoriteForCurrentUser(int starId);
    bool isFavoriteForCurrentUser(int starId) const;

public slots:
    void handleLogInRequest(const QString &username, const QString &password);
    void handleCreateAccountRequest(const QString &username, const QString &password);
    void handleLogoutRequest();
    void handleUserActivity();

signals:
    void logInSucceeded();
    void createAccountSucceeded();
    void authenticationStateChanged();
    void logInFailed(const QString &message);
    void createAccountFailed(const QString &message);

private:
    AppNavigator *m_appNavigator;
    StarController *m_starController;
    UserDatabase m_userDatabase;
    bool m_isLoggedIn = false;
    User m_currentUser;
    QTimer *m_inactivityTimer;

    void refreshCurrentUserFavorites();
    void handleInactivityTimeout();
};

#endif // AUTHCONTROLLER_H
