#ifndef APPNAVIGATOR_H
#define APPNAVIGATOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QEvent;
QT_END_NAMESPACE

class StarView;
class LogInView;
class createAccountView;
class AdminView;

class AppNavigator : public QWidget {
    Q_OBJECT

public:
    explicit AppNavigator(QWidget *parent = nullptr);
    ~AppNavigator();

    bool eventFilter(QObject *watched, QEvent *event) override;

    StarView *starView() const;
    LogInView *logInPage() const;
    createAccountView *createAccountPage() const;
    AdminView *adminPage() const;

public slots:
    void showStarView();
    void showLogInPage();
    void showCreateAccountPage();
    void showAdminPage();

signals:
    void userActivityDetected();
    void logInSubmitted(const QString &username, const QString &password);
    void createAccountSubmitted(const QString &username, const QString &password);

private:
    StarView *m_starView;
    LogInView *m_logInPage;
    createAccountView *m_createAccountPage;
    AdminView *m_adminPage;
    QStackedWidget *m_stackedWidget;
};

#endif // APPNAVIGATOR_H
