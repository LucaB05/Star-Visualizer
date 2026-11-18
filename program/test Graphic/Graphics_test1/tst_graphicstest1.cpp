#include <QCoreApplication>
#include <QTest>

// add necessary includes here

class graphicsTest1 : public QObject
{
    Q_OBJECT

public:
    graphicsTest1();
    ~graphicsTest1();

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();
    void test_case1();
};

graphicsTest1::graphicsTest1() {}

graphicsTest1::~graphicsTest1() {}

void graphicsTest1::initTestCase()
{
    // code to be executed before the first test function
}

void graphicsTest1::init()
{
    // code to be executed before each test function
}

void graphicsTest1::cleanupTestCase()
{
    // code to be executed after the last test function
}

void graphicsTest1::cleanup()
{
    // code to be executed after each test function
}

void graphicsTest1::test_case1() {}

QTEST_MAIN(graphicsTest1)

#include "tst_graphicstest1.moc"
