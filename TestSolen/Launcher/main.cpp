#include <QApplication>
#include <Qdebug>
#include <QDir>
#include <QFileInfo>
#include "AdminController.h"
#include "AuthController.h"
#include "AppNavigator.h"
#include "StarView.h"
#include "StarController.h"
#include "Star.h"
#include "stardatabase.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    const int maxStarsToLoad = 1200;

//Ser till att använda rätt databas fil "stars.db" ur root/databas/
    QString dbPath;
    QDir currentDir(QCoreApplication::applicationDirPath());
    //Kollar om databas/stars.db finns i current folder annars går den upp en nivå
    while (!currentDir.exists("databas/stars.db") && currentDir.cdUp()) {
    }
    if (currentDir.exists("databas/stars.db")) {
        dbPath = currentDir.filePath("databas/stars.db");
    } else {
        dbPath = "stars.db";
    }

    qDebug() << "Using database file:" << QFileInfo(dbPath).absoluteFilePath();

    AppNavigator *appNavigator = new AppNavigator();
    StarView *view = appNavigator->starView();
    StarModel *model = new StarModel();

    StarController controller(model, view);
    AuthController authController(dbPath, appNavigator, &controller);
    AdminController adminController(dbPath, appNavigator->adminPage());
    controller.setDatabasePath(dbPath);
    controller.setAuthController(&authController);
    QObject::connect(&adminController, &AdminController::starsChanged,
                     &controller, &StarController::reloadStarsFromDatabase);

    Star sun(0, "Solen", 0.0f, 0.0f, 999999.0f, -26.7f, -26.1f, "G2V");

    controller.loadStar(sun);

    StarDatabase db(dbPath);
    if (db.openConnection()) {
        QList<Star> allStars = db.fetchAllStars();
        int num = 0;
        for (const Star& s : allStars) {
            // qDebug() << "Distance: " << s.get_parallax();
            num += 1;
            if(num == maxStarsToLoad){
                break;
            }
            controller.loadStar(s);
        }
    }

    controller.start();

    return app.exec();
}
