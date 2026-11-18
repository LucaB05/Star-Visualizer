#include <QCoreApplication>
#include <QtTest>
#include "StarController.h"
#include "Star.h"
#include "StarModel.h"
#include "StarView.h"
#include <QVector3D>

class test_separate : public QObject
{
    Q_OBJECT
public:
    test_separate() {}
    virtual ~test_separate() {}
private slots:
    void test_no_overlap_returns_original_position();
    void test_overlap_moves_star();
    void test_empty_model_returns_original_position();
    void test_multiple_stars_no_overlap_with_any();
};

// Tom modell – positionen ska lämnas orörd
void test_separate::test_empty_model_returns_original_position()
{
    StarModel model;
    StarView view;
    StarController controller(&model, &view);

    QVector3D input(10.0f, 20.0f, 30.0f);
    QVector3D result = controller.separate(input, 2.0f, 0.5f);

    QCOMPARE(result, input);
}

// Ny stjärna långt bort från befintlig – ska inte flyttas
void test_separate::test_no_overlap_returns_original_position()
{
    StarModel model;
    StarView view;
    StarController controller(&model, &view);

    Star existing(1, "Befintlig", 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, "G2");
    existing.set_plottedPosition(QVector3D(0.0f, 0.0f, 0.0f));
    existing.set_visualRadius(2.0f);
    model.addStar(existing);

    QVector3D input(100.0f, 100.0f, 100.0f);
    QVector3D result = controller.separate(input, 2.0f, 0.5f);

    QCOMPARE(result, input);
}

// Ny stjärna placerad direkt ovanpå befintlig – ska separeras
void test_separate::test_overlap_moves_star()
{
    StarModel model;
    StarView view;
    StarController controller(&model, &view);

    Star existing(1, "Befintlig", 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, "G2");
    existing.set_plottedPosition(QVector3D(0.0f, 0.0f, 0.0f));
    existing.set_visualRadius(3.0f);
    model.addStar(existing);

    QVector3D input(0.0f, 0.0f, 0.0f);
    float newRadius = 2.0f;
    float space = 0.5f;
    QVector3D result = controller.separate(input, newRadius, space);

    float dist = result.distanceToPoint(existing.get_plottedPosition());
    float minDist = existing.get_visualRadius() + newRadius + space; // 3+2+0.5 = 5.5

    QVERIFY2(dist >= minDist - 0.001f,
             qPrintable(QString("Avstånd %1 understiger minDist %2").arg(dist).arg(minDist)));
}

// Två befintliga stjärnor – den nya ska inte överlappa någon av dem
void test_separate::test_multiple_stars_no_overlap_with_any()
{
    StarModel model;
    StarView view;
    StarController controller(&model, &view);

    Star s1(1, "Stjärna1", 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, "G2");
    s1.set_plottedPosition(QVector3D(2.0f, 0.0f, 0.0f));
    s1.set_visualRadius(2.0f);
    model.addStar(s1);

    Star s2(2, "Stjärna2", 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, "G2");
    s2.set_plottedPosition(QVector3D(-2.0f, 0.0f, 0.0f));
    s2.set_visualRadius(2.0f);
    model.addStar(s2);

    float newRadius = 1.0f;
    float space = 0.5f;
    QVector3D result = controller.separate(QVector3D(0.0f, 0.0f, 0.0f), newRadius, space);

    for (const Star &s : model.getAllStars()) {
        float dist = result.distanceToPoint(s.get_plottedPosition());
        float minDist = s.get_visualRadius() + newRadius + space;
        QVERIFY2(dist >= minDist - 0.001f,
                 qPrintable(QString("Kollision kvar med stjärna %1 (dist=%2, min=%3)")
                                .arg(s.get_id()).arg(dist).arg(minDist)));
    }
}

QTEST_MAIN(test_separate)
#include "tst_test_separate.moc"