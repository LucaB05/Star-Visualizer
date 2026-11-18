#include <QCoreApplication>
#include <QTest>

// add necessary includes here
#include "Star.h"

class TestStar : public QObject
{
    Q_OBJECT

public:
    TestStar();
    ~TestStar();

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();

    void test_constructor();
    void test_setters();
    void test_set_baseColor();
    void test_set_temp();
};

TestStar::TestStar() {

}

TestStar::~TestStar() {}

void TestStar::initTestCase()
{

}

void TestStar::init()
{
    // code to be executed before each test function
}

void TestStar::cleanupTestCase()
{
    // code to be executed after the last test function
}



Star star;

void TestStar::test_constructor() {
    Star star(0, "Solen", 3.0f, 2.0f, 999.0f, -26.7f, -26.1f, "G2V");
    QCOMPARE(star.get_id(), 0);
    QCOMPARE(star.get_name(), "Solen");
    QVERIFY(qFuzzyCompare(star.get_ra(), 3.0f));
    QVERIFY(qFuzzyCompare(star.get_dec(), 2.0f));
    QVERIFY(qFuzzyCompare(star.get_parallax(), 999.0f));
    QVERIFY(qFuzzyCompare(star.get_vMag(), -26.7f));
    QVERIFY(qFuzzyCompare(star.get_bMag(), -26.1f));
    QCOMPARE(star.get_spectralType(), "G2V");
}

void TestStar::test_setters() {
    Star star(1, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "");
    star.set_id(0);
    star.set_name("Solen");
    star.set_ra(3.0f);
    star.set_dec(2.0f);
    star.set_parallax(999.0f);
    star.set_vMag(-26.7f);
    star.set_bMag(-26.1);
    star.set_spectralType("G2V");

    QCOMPARE(star.get_id(), 0);
    QCOMPARE(star.get_name(), "Solen");
    QVERIFY(qFuzzyCompare(star.get_ra(), 3.0f));
    QVERIFY(qFuzzyCompare(star.get_dec(), 2.0f));
    QVERIFY(qFuzzyCompare(star.get_parallax(), 999.0f));
    QVERIFY(qFuzzyCompare(star.get_vMag(), -26.7f));
    QVERIFY(qFuzzyCompare(star.get_bMag(), -26.1f));
    QCOMPARE(star.get_spectralType(), "G2V");
}

void TestStar::test_set_temp() {
    Star star1(0, "", 3.0f, 2.0f, 999.0f, 0, 0, "G2V");
    star1.set_temp();
    QCOMPARE(star1.get_temp(), 5800);

    Star star2(0, "", 3.0f, 2.0f, 999.0f, 0, 0, "B2");
    star2.set_temp();
    QCOMPARE(star2.get_temp(), 21000);

    Star star3(0, "", 3.0f, 2.0f, 999.0f, 0, 0, "M7");
    star3.set_temp();
    QCOMPARE(star3.get_temp(), 2900);

    Star star4(0, "", 3.0f, 2.0f, 999.0f, 0, 0, "dM4");
    star4.set_temp();
    QCOMPARE(star4.get_temp(), 3150);

    Star star5(0, "", 3.0f, 2.0f, 999.0f, -26.7f, -26.1f, "");
    star5.set_temp();
    QCOMPARE(star5.get_temp(),  5658);

    Star star6(0, "", 0, 0, 0, 0, 0, "");
    star6.set_temp();
    QCOMPARE(star6.get_temp(),  9094);
}

void TestStar::test_set_baseColor() {
    Star star(0, "", 3.0f, 2.0f, 999.0f, -26.7f, -26.1f, "G2V");
    star.set_baseColor();
    QCOMPARE(star.get_baseColor(), QColorConstants::Svg::yellow);
}

void TestStar::cleanup()
{
    // code to be executed after each test function
}

QTEST_MAIN(TestStar)

#include "tst_teststar.moc"
