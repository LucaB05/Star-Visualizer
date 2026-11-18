#include <QtTest>
#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QEvent>

#include "AppNavigator.h"
#include "AuthController.h"
#include "StarController.h"
#include "StarModel.h"

class TestAutoLogout : public QObject
{
    Q_OBJECT

private slots:
    void activityEvents_emitUserActivitySignal();
    void mouseMove_doesNotEmitUserActivitySignal();
    void login_startsTimerAndTimeoutLogsOut();
    void activity_restartsTimer();
    void guestActivity_doesNotLogInOrStartLogoutFlow();
    void manualLogout_stopsTimer();

private:
    QString createTestDatabase() const;
    void simulateActivity(AppNavigator &navigator, QEvent::Type eventType) const;
};

QString TestAutoLogout::createTestDatabase() const
{
    const QString dbPath = QDir::temp().filePath(
        "testsolen_autologout_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".sqlite"
    );
    const QString connectionName = "autologout_setup_" + QUuid::createUuid().toString(QUuid::WithoutBraces);

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            qFatal("Could not open auto logout test database.");
        }

        QSqlQuery query(db);
        if (!query.exec(
                "CREATE TABLE users("
                "id INTEGER PRIMARY KEY,"
                "name TEXT UNIQUE NOT NULL,"
                "password TEXT NOT NULL,"
                "isAdmin INTEGER NOT NULL DEFAULT 0)")) {
            qFatal("SQL ERROR create users: %s", qPrintable(query.lastError().text()));
        }

        if (!query.exec(
                "CREATE TABLE stars("
                "id INTEGER PRIMARY KEY,"
                "name TEXT,"
                "ra REAL,"
                "dec REAL,"
                "parallax REAL,"
                "v_mag REAL,"
                "b_mag REAL,"
                "spectral_type TEXT)")) {
            qFatal("SQL ERROR create stars: %s", qPrintable(query.lastError().text()));
        }

        if (!query.exec(
                "CREATE TABLE favorites("
                "userId INTEGER NOT NULL,"
                "starId INTEGER NOT NULL,"
                "PRIMARY KEY (userId, starId))")) {
            qFatal("SQL ERROR create favorites: %s", qPrintable(query.lastError().text()));
        }

        query.prepare("INSERT INTO users (name, password, isAdmin) VALUES (:name, :password, 0)");
        query.bindValue(":name", "testuser");
        query.bindValue(":password", "password");
        if (!query.exec()) {
            qFatal("SQL ERROR insert user: %s", qPrintable(query.lastError().text()));
        }

        db.close();
    }

    QSqlDatabase::removeDatabase(connectionName);
    return dbPath;
}

void TestAutoLogout::simulateActivity(AppNavigator &navigator, QEvent::Type eventType) const
{
    QEvent event(eventType);
    navigator.eventFilter(&navigator, &event);
}

void TestAutoLogout::activityEvents_emitUserActivitySignal()
{
    AppNavigator navigator;
    QSignalSpy spy(&navigator, &AppNavigator::userActivityDetected);
    QVERIFY(spy.isValid());

    simulateActivity(navigator, QEvent::MouseButtonPress);
    simulateActivity(navigator, QEvent::MouseButtonRelease);
    simulateActivity(navigator, QEvent::MouseButtonDblClick);
    simulateActivity(navigator, QEvent::KeyPress);
    simulateActivity(navigator, QEvent::Wheel);

    QCOMPARE(spy.count(), 5);
}

void TestAutoLogout::mouseMove_doesNotEmitUserActivitySignal()
{
    AppNavigator navigator;
    QSignalSpy spy(&navigator, &AppNavigator::userActivityDetected);
    QVERIFY(spy.isValid());

    simulateActivity(navigator, QEvent::MouseMove);

    QCOMPARE(spy.count(), 0);
}

void TestAutoLogout::login_startsTimerAndTimeoutLogsOut()
{
    const QString dbPath = createTestDatabase();
    {
        AppNavigator navigator;
        StarModel model;
        StarController starController(&model, navigator.starView());
        AuthController authController(dbPath, &navigator, &starController, nullptr, 80);
        starController.setAuthController(&authController);

        authController.handleLogInRequest("testuser", "password");
        QVERIFY(authController.isLoggedIn());

        QTest::qWait(130);

        QVERIFY(!authController.isLoggedIn());
    }
    QFile::remove(dbPath);
}

void TestAutoLogout::activity_restartsTimer()
{
    const QString dbPath = createTestDatabase();
    {
        AppNavigator navigator;
        StarModel model;
        StarController starController(&model, navigator.starView());
        AuthController authController(dbPath, &navigator, &starController, nullptr, 200);
        starController.setAuthController(&authController);

        authController.handleLogInRequest("testuser", "password");
        QVERIFY(authController.isLoggedIn());

        QTest::qWait(150);
        simulateActivity(navigator, QEvent::MouseButtonPress);
        QTest::qWait(130);

        QVERIFY(authController.isLoggedIn());

        QTest::qWait(100);

        QVERIFY(!authController.isLoggedIn());
    }
    QFile::remove(dbPath);
}

void TestAutoLogout::guestActivity_doesNotLogInOrStartLogoutFlow()
{
    const QString dbPath = createTestDatabase();
    {
        AppNavigator navigator;
        StarModel model;
        StarController starController(&model, navigator.starView());
        AuthController authController(dbPath, &navigator, &starController, nullptr, 50);
        starController.setAuthController(&authController);

        simulateActivity(navigator, QEvent::MouseButtonPress);
        simulateActivity(navigator, QEvent::KeyPress);
        QTest::qWait(80);

        QVERIFY(!authController.isLoggedIn());
    }
    QFile::remove(dbPath);
}

void TestAutoLogout::manualLogout_stopsTimer()
{
    const QString dbPath = createTestDatabase();
    {
        AppNavigator navigator;
        StarModel model;
        StarController starController(&model, navigator.starView());
        AuthController authController(dbPath, &navigator, &starController, nullptr, 80);
        starController.setAuthController(&authController);

        authController.handleLogInRequest("testuser", "password");
        QVERIFY(authController.isLoggedIn());

        authController.handleLogoutRequest();
        QVERIFY(!authController.isLoggedIn());

        QTest::qWait(120);

        QVERIFY(!authController.isLoggedIn());
    }
    QFile::remove(dbPath);
}

QTEST_MAIN(TestAutoLogout)

#include "tst_autologout.moc"
