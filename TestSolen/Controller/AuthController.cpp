#include "AuthController.h"
#include "AppNavigator.h"
#include "StarController.h"
#include "loginview.h"
#include "createaccountview.h"
#include <QDebug>
#include <QTimer>

AuthController::AuthController(const QString &dbPath,
                               AppNavigator *appNavigator,
                               StarController *starController,
                               QObject *parent,
                               int inactivityTimeoutMs)
    : QObject(parent),
      m_appNavigator(appNavigator),
      m_starController(starController),
      m_userDatabase(dbPath),
      m_inactivityTimer(new QTimer(this))
{
    m_userDatabase.openConnection();

    m_inactivityTimer->setInterval(inactivityTimeoutMs);
    m_inactivityTimer->setSingleShot(true);

    connect(m_appNavigator, &AppNavigator::logInSubmitted,
            this, &AuthController::handleLogInRequest);
    connect(m_appNavigator, &AppNavigator::createAccountSubmitted,
            this, &AuthController::handleCreateAccountRequest);
    connect(m_appNavigator, &AppNavigator::userActivityDetected,
            this, &AuthController::handleUserActivity);
    connect(m_inactivityTimer, &QTimer::timeout,
            this, &AuthController::handleInactivityTimeout);

    connect(this, &AuthController::logInSucceeded,
            m_appNavigator, &AppNavigator::showStarView);
    connect(this, &AuthController::createAccountSucceeded,
            m_appNavigator, &AppNavigator::showStarView);

    connect(this, &AuthController::authenticationStateChanged,
            m_starController, &StarController::handleAuthenticationStateChanged);
    connect(this, &AuthController::logInFailed,
            m_appNavigator->logInPage(), &LogInView::showErrorMessage);
    connect(this, &AuthController::createAccountFailed,
            m_appNavigator->createAccountPage(), &createAccountView::showErrorMessage);
}

bool AuthController::isLoggedIn() const
{
    return m_isLoggedIn;
}

User AuthController::currentUser() const
{
    return m_currentUser;
}

bool AuthController::addFavoriteForCurrentUser(int starId)
{
    if (!m_isLoggedIn || m_currentUser.id() < 0) {
        return false;
    }

    if (!m_userDatabase.addFavorite(m_currentUser.id(), starId)) {
        return false;
    }

    refreshCurrentUserFavorites();
    emit authenticationStateChanged();
    return true;
}

bool AuthController::removeFavoriteForCurrentUser(int starId)
{
    if (!m_isLoggedIn || m_currentUser.id() < 0) {
        return false;
    }

    if (!m_userDatabase.removeFavorite(m_currentUser.id(), starId)) {
        return false;
    }

    refreshCurrentUserFavorites();
    emit authenticationStateChanged();
    return true;
}

bool AuthController::isFavoriteForCurrentUser(int starId) const
{
    if (!m_isLoggedIn || m_currentUser.id() < 0) {
        return false;
    }

    return m_userDatabase.isFavorite(m_currentUser.id(), starId);
}

void AuthController::handleLogInRequest(const QString &username, const QString &password)
{
    qDebug() << "Namn:" << username << "Losenord:" << password;

    User foundUser = m_userDatabase.findUserByName(username);
    if (foundUser.id() < 0) {
        emit logInFailed("Anvandaren finns inte.");
        return;
    }

    if (foundUser.password() != password) {
        emit logInFailed("Fel losenord.");
        return;
    }

    m_currentUser = foundUser;
    m_isLoggedIn = true;
    refreshCurrentUserFavorites();
    m_inactivityTimer->start();
    emit logInSucceeded();
    emit authenticationStateChanged();
}

void AuthController::handleCreateAccountRequest(const QString &username, const QString &password)
{
    qDebug() << "Namn:" << username << "Losenord:" << password;

    User existingUser = m_userDatabase.findUserByName(username);
    if (existingUser.id() >= 0) {
        emit createAccountFailed("Anvandaren finns redan.");
        return;
    }

    User newUser(username, password, false, {});
    if (!m_userDatabase.addUser(newUser)) {
        emit createAccountFailed("Kunde inte skapa anvandaren.");
        return;
    }

    handleLogInRequest(username, password);
}

void AuthController::handleLogoutRequest()
{
    m_inactivityTimer->stop();
    m_currentUser = User();
    m_isLoggedIn = false;
    emit authenticationStateChanged();
}

void AuthController::handleUserActivity()
{
    if (!m_isLoggedIn) {
        return;
    }

    m_inactivityTimer->start();
}

void AuthController::handleInactivityTimeout()
{
    if (!m_isLoggedIn) {
        return;
    }

    handleLogoutRequest();
    m_appNavigator->showStarView();
}

void AuthController::refreshCurrentUserFavorites()
{
    if (m_currentUser.id() < 0) {
        m_currentUser.setFavoriteStars({});
        return;
    }

    m_currentUser.setFavoriteStars(m_userDatabase.getFavoriteStarNames(m_currentUser.id()));
}
