#include <QCoreApplication>
#include <QtTest>
#include "StarController.h"
#include "Star.h"
#include "StarModel.h"
#include "StarView.h"
#include <QVector3D>



class test_calculate_position : public QObject
{
    Q_OBJECT

public:
    test_calculate_position() {}
    virtual ~test_calculate_position() {}

private slots:
    void test_calculation_logic_data();
    void test_calculation_logic();
};

void test_calculate_position::test_calculation_logic_data()
{
    QTest::addColumn<float>("ra");
    QTest::addColumn<float>("dec");
    QTest::addColumn<float>("parallax");
    QTest::addColumn<QVector3D>("expected");

    QTest::newRow("X_axel") << 0.0f << 0.0f << 1000.0f << QVector3D(60.0f, 0.0f, 0.0f);
    QTest::newRow("rakt_upp_Z") << 0.0f << 90.0f << 1000.0f << QVector3D(0.0f, 0.0f, 60.0f);
    QTest::newRow("ogiltig parallax") << 0.0f << 0.0f << 0.0f << QVector3D(0.0f, 0.0f, 0.0f);
}

void test_calculate_position::test_calculation_logic()
{

    QFETCH(float, ra);
    QFETCH(float, dec);
    QFETCH(float, parallax);
    QFETCH(QVector3D, expected);

    StarModel model;
    StarView view;
    StarController controller(&model, &view);

    Star testStar;
    testStar.set_ra(ra);
    testStar.set_dec(dec);
    testStar.set_parallax(parallax);

    QVector3D result = controller.calculatePosition(testStar);

    if (parallax <= 0.0f) {
        QVERIFY(result.isNull());
    } else {
        float epsilon = 0.001f;

        QVERIFY2(qAbs(result.x() - expected.x()) < epsilon,
                 qPrintable(QString("X avviker: fick %1, förväntade oss %2").arg(result.x()).arg(expected.x())));
        QVERIFY2(qAbs(result.y() - expected.y()) < epsilon,
                 qPrintable(QString("Y avviker: fick %1, förväntade oss %2").arg(result.y()).arg(expected.y())));
        QVERIFY2(qAbs(result.z() - expected.z()) < epsilon,
                 qPrintable(QString("Z avviker: fick %1, förväntade oss %2").arg(result.z()).arg(expected.z())));
    }
}

QTEST_MAIN(test_calculate_position)
#include "tst_test_calculate_position.moc"
