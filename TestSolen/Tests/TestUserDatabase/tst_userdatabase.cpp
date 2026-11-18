#include <QtTest>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>

#include "User.h"
#include "UserDatabase.h"

class TestUserDatabase : public QObject
{
    Q_OBJECT

public:
    TestUserDatabase() = default;

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();
    void test_userCrudFlow();

private:
    QString m_dbPath;
    UserDatabase *m_userDb = nullptr;
};

//Creates a test enviroment with a temporary databse file and a connection to it. 
void TestUserDatabase::initTestCase()
{
    m_dbPath = QDir::temp().filePath("testsolen_userdatabase_test.sqlite");
    QFile::remove(m_dbPath);

    m_userDb = new UserDatabase(m_dbPath);
    QVERIFY(m_userDb->openConnection());

    QSqlDatabase db = QSqlDatabase::database("user_connection");
    QVERIFY2(db.isOpen(), "User database connection should be open during test setup.");

    QSqlQuery query(db);
    QVERIFY2(query.exec(
                 "CREATE TABLE IF NOT EXISTS users("
                 "id INTEGER PRIMARY KEY,"
                 "name TEXT UNIQUE NOT NULL,"
                 "password TEXT NOT NULL,"
                 "isAdmin INTEGER NOT NULL DEFAULT 0)"),
             qPrintable(query.lastError().text()));
}

//Resets the data inbetween tests
void TestUserDatabase::cleanup()
{
    QSqlDatabase db = QSqlDatabase::database("user_connection");
    QSqlQuery query(db);
    QVERIFY2(query.exec("DELETE FROM users"), qPrintable(query.lastError().text()));
}

//Closes the connection and deletes the temporary database file.
void TestUserDatabase::cleanupTestCase()
{
    delete m_userDb;
    m_userDb = nullptr;

    QSqlDatabase::removeDatabase("user_connection");
    QFile::remove(m_dbPath);
}

//Tests the full CRUD flow for a user: create, read, update and delete.
void TestUserDatabase::test_userCrudFlow()
{
    User user("Luca", "1234", false, {});
    QVERIFY(m_userDb->addUser(user));

    User found = m_userDb->findUserByName("Luca");
    QCOMPARE(found.username(), QString("Luca"));
    QCOMPARE(found.password(), QString("1234"));
    QCOMPARE(found.isAdmin(), false);
    QVERIFY(found.id() >= 0);

    found.setPassword("abcd");
    found.setIsAdmin(true);
    QVERIFY(m_userDb->editUser(found));

    User edited = m_userDb->findUserByName("Luca");
    QCOMPARE(edited.username(), QString("Luca"));
    QCOMPARE(edited.password(), QString("abcd"));
    QCOMPARE(edited.isAdmin(), true);
    QCOMPARE(edited.id(), found.id());

    QVERIFY(m_userDb->deleteUser("Luca"));

    User deleted = m_userDb->findUserByName("Luca");
    QCOMPARE(deleted.id(), -1);
    QCOMPARE(deleted.username(), QString());
    QCOMPARE(deleted.password(), QString());
    QCOMPARE(deleted.isAdmin(), false);
}

QTEST_MAIN(TestUserDatabase)

#include "tst_userdatabase.moc"
