#include "AppNavigator.h"

#include "StarView.h"
#include "loginview.h"
#include "createaccountview.h"
#include "adminview.h"

#include <QApplication>
#include <QEvent>
#include <QStackedWidget>
#include <QVBoxLayout>

AppNavigator::AppNavigator(QWidget *parent)
    : QWidget(parent),
      m_starView(new StarView(this)),
      m_logInPage(new LogInView(this)),
      m_createAccountPage(new createAccountView(this)),
      m_adminPage(new AdminView(this)),
      m_stackedWidget(new QStackedWidget(this))
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(m_stackedWidget);

    setStyleSheet("background-color: black; color: white;");
    m_stackedWidget->addWidget(m_starView);
    m_stackedWidget->addWidget(m_logInPage);
    m_stackedWidget->addWidget(m_createAccountPage);
    m_stackedWidget->addWidget(m_adminPage);

    setWindowTitle("Solen i 3D");
    resize(1024, 768);
    showStarView();
    qApp->installEventFilter(this);

    connect(m_starView, &StarView::loginRequested, this, [this]() {
        showLogInPage();
    });
    connect(m_starView, &StarView::adminRequested, this, &AppNavigator::showAdminPage);

    connect(m_logInPage, &LogInView::createAccountRequested, this, &AppNavigator::showCreateAccountPage);
    connect(m_logInPage, &LogInView::logInSubmitted, this, [this](const QString &username, const QString &password) {
        emit logInSubmitted(username, password);
    });
    connect(m_logInPage, &LogInView::continueAsGuestRequested, this, &AppNavigator::showStarView);

    connect(m_createAccountPage, &createAccountView::logInRequested, this, &AppNavigator::showLogInPage);
    connect(m_createAccountPage, &createAccountView::createAccountSubmitted, this, [this](const QString &username, const QString &password) {
        emit createAccountSubmitted(username, password);
    });
    connect(m_createAccountPage, &createAccountView::continueAsGuestRequested, this, &AppNavigator::showStarView);
    connect(m_adminPage, &AdminView::backToHomeRequested, this, &AppNavigator::showStarView);
}

AppNavigator::~AppNavigator()
{
    if (qApp) {
        qApp->removeEventFilter(this);
    }
}

bool AppNavigator::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::KeyPress:
    case QEvent::Wheel:
        emit userActivityDetected();
        break;
    default:
        break;
    }

    return QWidget::eventFilter(watched, event);
}

StarView *AppNavigator::starView() const
{
    return m_starView;
}

LogInView *AppNavigator::logInPage() const
{
    return m_logInPage;
}

createAccountView *AppNavigator::createAccountPage() const
{
    return m_createAccountPage;
}

AdminView *AppNavigator::adminPage() const
{
    return m_adminPage;
}

void AppNavigator::showStarView()
{
    m_logInPage->clearErrorMessage();
    m_createAccountPage->clearErrorMessage();
    m_stackedWidget->setCurrentWidget(m_starView);
}

void AppNavigator::showLogInPage()
{
    m_logInPage->clearErrorMessage();
    m_stackedWidget->setCurrentWidget(m_logInPage);
}

void AppNavigator::showCreateAccountPage()
{
    m_createAccountPage->clearErrorMessage();
    m_stackedWidget->setCurrentWidget(m_createAccountPage);
}

void AppNavigator::showAdminPage()
{
    m_adminPage->showMenuPage();
    m_stackedWidget->setCurrentWidget(m_adminPage);
}
