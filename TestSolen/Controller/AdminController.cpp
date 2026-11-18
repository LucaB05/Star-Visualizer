#include "AdminController.h"

#include "adminview.h"

#include <utility>

AdminController::AdminController(const QString &dbPath, AdminView *adminView, QObject *parent)
    : QObject(parent),
      m_adminView(adminView),
      m_userDatabase(dbPath),
      m_starDatabase(dbPath)
{
    m_userDatabase.openConnection();
    m_starDatabase.openConnection();

    connect(m_adminView, &AdminView::userSettingsOpened,
            this, &AdminController::handleUserSettingsOpened);
    connect(m_adminView, &AdminView::userSearchChanged,
            this, &AdminController::handleUserSearchChanged);
    connect(m_adminView, &AdminView::userSelected,
            this, &AdminController::handleUserSelected);
    connect(m_adminView, &AdminView::saveUserRequested,
            this, &AdminController::handleSaveUserRequested);
    connect(m_adminView, &AdminView::deleteUserRequested,
            this, &AdminController::handleDeleteUserRequested);
    connect(m_adminView, &AdminView::addUserRequested,
            this, &AdminController::handleAddUserRequested);

    connect(m_adminView, &AdminView::starSettingsOpened,
            this, &AdminController::handleStarSettingsOpened);
    connect(m_adminView, &AdminView::starSearchChanged,
            this, &AdminController::handleStarSearchChanged);
    connect(m_adminView, &AdminView::starSelected,
            this, &AdminController::handleStarSelected);
    connect(m_adminView, &AdminView::saveStarRequested,
            this, &AdminController::handleSaveStarRequested);
    connect(m_adminView, &AdminView::deleteStarRequested,
            this, &AdminController::handleDeleteStarRequested);
    connect(m_adminView, &AdminView::addStarRequested,
            this, &AdminController::handleAddStarRequested);
}

void AdminController::handleUserSettingsOpened()
{
    m_users = m_userDatabase.getAllUsers();
    m_adminView->displayUsers(m_users);
    m_adminView->clearSelectedUser();
}

void AdminController::handleUserSearchChanged(const QString &query)
{
    if (m_users.isEmpty()) {
        m_users = m_userDatabase.getAllUsers();
    }

    m_adminView->displayUsers(filterUsers(query));
    m_adminView->clearSelectedUser();
}

void AdminController::handleUserSelected(const QString &username)
{
    for (const User &user : std::as_const(m_users)) {
        if (user.username().compare(username, Qt::CaseInsensitive) == 0) {
            m_adminView->displaySelectedUser(user);
            return;
        }
    }

    m_adminView->clearSelectedUser();
}

void AdminController::handleSaveUserRequested(const User &user, const QString &passwordConfirmation)
{
    if (user.id() < 0) {
        m_adminView->showUserMessage("Error saving: select a user first.", true);
        return;
    }

    if (user.username().trimmed().isEmpty()) {
        m_adminView->showUserMessage("Error saving: username cannot be empty.", true);
        return;
    }

    if (user.password().isEmpty()) {
        m_adminView->showUserMessage("Error saving: password cannot be empty.", true);
        return;
    }

    if (passwordConfirmation.isEmpty()) {
        m_adminView->showUserMessage("Error saving: confirm the password.", true);
        return;
    }

    if (user.password() != passwordConfirmation) {
        m_adminView->showUserMessage("Error saving: passwords do not match.", true);
        return;
    }

    User existingUser = m_userDatabase.findUserByName(user.username());
    if (existingUser.id() >= 0 && existingUser.id() != user.id()) {
        m_adminView->showUserMessage("Error saving: username already exists.", true);
        return;
    }

    if (!m_userDatabase.editUser(user)) {
        m_adminView->showUserMessage("Error saving.", true);
        return;
    }

    m_users = m_userDatabase.getAllUsers();
    m_adminView->displayUsers(filterUsers(QString()));
    m_adminView->displaySelectedUser(user);
    m_adminView->showUserMessage("Saved successfully.", false);
}

void AdminController::handleDeleteUserRequested(int userId, const QString &username)
{
    if (userId < 0 || username.trimmed().isEmpty()) {
        m_adminView->showUserMessage("Error deleting: select a user first.", true);
        return;
    }

    if (!m_userDatabase.deleteUser(username)) {
        m_adminView->showUserMessage("Error deleting user.", true);
        return;
    }

    m_users = m_userDatabase.getAllUsers();
    m_adminView->displayUsers(filterUsers(QString()));
    m_adminView->clearSelectedUser();
    m_adminView->showUserMessage("User deleted successfully.", false);
}

void AdminController::handleAddUserRequested(const User &user, const QString &passwordConfirmation)
{
    if (user.username().trimmed().isEmpty()) {
        m_adminView->showAddUserMessage("Error adding: username cannot be empty.", true);
        return;
    }

    if (user.password().isEmpty()) {
        m_adminView->showAddUserMessage("Error adding: password cannot be empty.", true);
        return;
    }

    if (passwordConfirmation.isEmpty()) {
        m_adminView->showAddUserMessage("Error adding: confirm the password.", true);
        return;
    }

    if (user.password() != passwordConfirmation) {
        m_adminView->showAddUserMessage("Error adding: passwords do not match.", true);
        return;
    }

    User existingUser = m_userDatabase.findUserByName(user.username());
    if (existingUser.id() >= 0) {
        m_adminView->showAddUserMessage("Error adding: username already exists.", true);
        return;
    }

    if (!m_userDatabase.addUser(user)) {
        m_adminView->showAddUserMessage("Error adding user.", true);
        return;
    }

    m_users = m_userDatabase.getAllUsers();
    m_adminView->displayUsers(filterUsers(QString()));
    m_adminView->showAddUserMessage("User added successfully.", false);
}

void AdminController::handleStarSettingsOpened()
{
    m_stars = m_starDatabase.fetchStars(maxAdminStarsToLoad);
    m_adminView->displayStars(m_stars);
    m_adminView->clearSelectedStar();
}

void AdminController::handleStarSearchChanged(const QString &query)
{
    if (m_stars.isEmpty()) {
        m_stars = m_starDatabase.fetchStars(maxAdminStarsToLoad);
    }

    m_adminView->displayStars(filterStars(query));
    m_adminView->clearSelectedStar();
}

void AdminController::handleSaveStarRequested(const Star &star)
{
    if (star.get_id() < 0) {
        m_adminView->showStarMessage("Error saving: select a star first.", true);
        return;
    }

    if (!validateStar(star, "saving")) {
        return;
    }

    Star existingStar = m_starDatabase.findStarByName(star.get_name());
    if (existingStar.get_id() >= 0 && existingStar.get_id() != star.get_id()) {
        m_adminView->showStarMessage("Error saving: star name already exists.", true);
        return;
    }

    if (!m_starDatabase.updateStar(star)) {
        m_adminView->showStarMessage("Error saving star.", true);
        return;
    }

    m_stars = m_starDatabase.fetchStars(maxAdminStarsToLoad);
    m_adminView->displayStars(filterStars(QString()));
    m_adminView->displaySelectedStar(star);
    m_adminView->showStarMessage("Star saved successfully.", false);
    emit starsChanged();
}

void AdminController::handleDeleteStarRequested(int starId, const QString &starName)
{
    if (starId < 0 || starName.trimmed().isEmpty()) {
        m_adminView->showStarMessage("Error deleting: select a star first.", true);
        return;
    }

    if (!m_starDatabase.deleteStar(starId)) {
        m_adminView->showStarMessage("Error deleting star.", true);
        return;
    }

    m_stars = m_starDatabase.fetchStars(maxAdminStarsToLoad);
    m_adminView->displayStars(filterStars(QString()));
    m_adminView->clearSelectedStar();
    m_adminView->showStarMessage("Star deleted successfully.", false);
    emit starsChanged();
}

void AdminController::handleAddStarRequested(const Star &star)
{
    if (!validateStar(star, "adding")) {
        return;
    }

    Star existingStar = m_starDatabase.findStarByName(star.get_name());
    if (existingStar.get_id() >= 0) {
        m_adminView->showStarMessage("Error adding: star name already exists.", true);
        return;
    }

    const int newId = m_starDatabase.addStar(star);
    if (newId < 0) {
        m_adminView->showStarMessage("Error adding star.", true);
        return;
    }

    Star addedStar = star;
    addedStar.set_id(newId);

    m_stars = m_starDatabase.fetchStars(maxAdminStarsToLoad);
    m_adminView->displayStars(filterStars(QString()));
    m_adminView->displaySelectedStar(addedStar);
    m_adminView->showStarMessage("Star added successfully.", false);
    emit starsChanged();
}

void AdminController::handleStarSelected(const QString &starName)
{
    for (const Star &star : std::as_const(m_stars)) {
        if (star.get_name().compare(starName, Qt::CaseInsensitive) == 0) {
            m_adminView->displaySelectedStar(star);
            return;
        }
    }

    m_adminView->clearSelectedStar();
}

QList<User> AdminController::filterUsers(const QString &query) const
{
    if (query.trimmed().isEmpty()) {
        return m_users;
    }

    QList<User> filteredUsers;
    for (const User &user : m_users) {
        if (user.username().contains(query, Qt::CaseInsensitive)) {
            filteredUsers.append(user);
        }
    }

    return filteredUsers;
}

QList<Star> AdminController::filterStars(const QString &query) const
{
    if (query.trimmed().isEmpty()) {
        return m_stars;
    }

    QList<Star> filteredStars;
    for (const Star &star : m_stars) {
        if (star.get_name().contains(query, Qt::CaseInsensitive)) {
            filteredStars.append(star);
        }
    }

    return filteredStars;
}

bool AdminController::validateStar(const Star &star, const QString &action)
{
    if (star.get_name().trimmed().isEmpty()) {
        m_adminView->showStarMessage(QString("Error %1: star name cannot be empty.").arg(action), true);
        return false;
    }

    if (star.get_name().compare("Solen", Qt::CaseInsensitive) == 0) {
        m_adminView->showStarMessage(QString("Error %1: Solen is the built-in sun.").arg(action), true);
        return false;
    }

    if (star.get_parallax() <= 0.000001f) {
        m_adminView->showStarMessage(QString("Error %1: parallax must be greater than 0.").arg(action), true);
        return false;
    }

    return true;
}
