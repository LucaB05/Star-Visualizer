#include <QtTest>
#include "StarController.h"
#include "StarModel.h"
#include "StarView.h"

class test_star_search : public QObject
{
    Q_OBJECT

private slots:
    void test_search_logic() {
        StarModel model;
        StarView view;
        view.hide();

        StarController controller(&model, &view);


        Star s1 = Star();
        Star s2 = Star();
        Star s3 = Star();
        s1.set_id(1);
        s2.set_id(2);
        s3.set_id(3);
        s1.set_name("Solen");
        s2.set_name("Vega");
        s3.set_name("Proxima centauri");

        model.addStar(s1);
        model.addStar(s2);
        model.addStar(s3);


        controller.handleSearch("Solen");
        QCoreApplication::processEvents();


        QList<Star> result = view.getCurrentStarList();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).get_name(), "Solen");
    }
};

QTEST_MAIN(test_star_search)
#include "tst_test_star_search.moc"
