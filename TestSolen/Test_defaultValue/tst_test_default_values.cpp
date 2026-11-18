#include <QTest>
#include "Star.h"

class Test_DefaultValues : public QObject
{
    Q_OBJECT

private slots:
    void Test_Default_Values();
};

void Test_DefaultValues::Test_Default_Values()
{
    Star star(2, "Alpha", 5.0f, 6.0f, 0.2f,
              1.0f, 1.5f, "A1V");

    QCOMPARE(star.get_plottedPosition(), QVector3D(0,0,0));
    QCOMPARE(star.get_visualRadius(), 1.0f);
}

QTEST_APPLESS_MAIN(Test_DefaultValues)

#include "tst_test_default_values.moc"
