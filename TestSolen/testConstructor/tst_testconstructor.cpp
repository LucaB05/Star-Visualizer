#include <QTest>
#include "Star.h"

class testcontroller : public QObject
{
    Q_OBJECT

private slots:
    void test_constructor();
};

void testcontroller::test_constructor()
{
    Star star(1, "Sun", 10.0f, 20.0f, 0.5f,
              1.0f, 2.0f, "G2V");

    QCOMPARE(star.get_id(), 1);
    QCOMPARE(star.get_name(), QString("Sun"));
}

QTEST_APPLESS_MAIN(testcontroller)

#include "tst_testconstructor.moc"
