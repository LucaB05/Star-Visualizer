#ifndef ADMINVIEW_H
#define ADMINVIEW_H

#include <QList>
#include <QWidget>
#include "Star.h"
#include "User.h"

namespace Ui {
class AdminView;
}

class AdminView : public QWidget
{
    Q_OBJECT

public:
    explicit AdminView(QWidget *parent = nullptr);
    ~AdminView();

    void displayUsers(const QList<User> &users);
    void displaySelectedUser(const User &user);
    void clearSelectedUser();
    void showUserMessage(const QString &message, bool isError);
    void clearUserMessage();
    void showAddUserMessage(const QString &message, bool isError);
    void clearAddUserMessage();
    void displayStars(const QList<Star> &stars);
    void displaySelectedStar(const Star &star);
    void clearSelectedStar();
    void showStarMessage(const QString &message, bool isError);
    void clearStarMessage();

signals:
    void backToHomeRequested();
    void userSettingsOpened();
    void userSearchChanged(const QString &query);
    void userSelected(const QString &username);
    void saveUserRequested(const User &user, const QString &passwordConfirmation);
    void deleteUserRequested(int userId, const QString &username);
    void addUserRequested(const User &user, const QString &passwordConfirmation);
    void starSettingsOpened();
    void starSearchChanged(const QString &query);
    void starSelected(const QString &starName);
    void saveStarRequested(const Star &star);
    void deleteStarRequested(int starId, const QString &starName);
    void addStarRequested(const Star &star);

public slots:
    void showMenuPage();
    void showUserSettingsPage();
    void showEditUserPage();
    void showAddUserPage();
    void showStarSettingsPage();

private:
    void clearAddUserForm();
    Star starFromEditor(int id) const;
    void showEditStarMode();
    void showAddStarMode();
    void generateStarPosition();

private:
    Ui::AdminView *ui;
    int m_selectedUserId = -1;
    int m_selectedStarId = -1;
    bool m_addingStar = false;
};

#endif // ADMINVIEW_H
