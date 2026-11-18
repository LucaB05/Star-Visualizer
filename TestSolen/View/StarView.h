#ifndef STARVIEW_H
#define STARVIEW_H

#include "qelapsedtimer.h"
#include <Qt3DRender/QPickEvent>
#include <QWidget>
#include <QVector3D>
#include <QColor>
#include <QStringList>
#include <QListWidgetItem>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include "Star.h"
#include <Qt3DExtras/QPhongMaterial>

namespace Ui {
class StarView;
}

struct StarFilterOptions {
    QMap<QString, bool> spectralClasses;
    bool includeUnknownSpectral;

    double minDist;
    double maxDist;

    int minTemp;
    int maxTemp;

    double minRadius;
    double maxRadius;

    double minMass;
    double maxMass;

    double minVMag;
    double maxVMag;
};

class StarView : public QWidget {
    Q_OBJECT

public:
    explicit StarView(QWidget *parent = nullptr);
    ~StarView();

    void showWindow();
    Qt3DCore::QEntity *addStar(const Star& star);
    void clearStars();

    void showStarToolTip(const QString& text);
    void hideStarToolTip();

    void flyToPosition(const QVector3D& pos, float distance = 80.0f);

    void displayStarInfo(const QMap<QString, QString>& info);
    void displayLoggedInUserData(const QString& username, const QStringList& favorites, bool isAdmin);
    void displayLoggedOutUserData();
    void showFilterPage();
    void setFirstPersonButtonVisible(bool visible);
    void setFavoritesButtonVisible(bool visible);
    void setFavoriteToggleVisible(bool visible);
    void setFavoriteToggleText(const QString &text);

    void setFirstPersonButtonText(const QString &text);


    void updateStarList(const QList<Star> &stars);

    void onSearchBoxActivated(int index);

    QList<Star> getCurrentStarList() const { return m_currentStars; }

    void updateStarState(int id, StarState state, const QColor& baseColor);

signals:
    void starHovered(int id);
    void starHoveredEnded();
    void starSelected(int id); //(enkelklick)
    void starDoubleClicked(int id);
    void homeButtonClicked();
    void firstPersonRequested();
    void favoritesRequested();
    void favoriteToggleRequested();
    void favoriteStarSelected(const QString &starName);
    void loginRequested();
    void logoutRequested();
    void adminRequested();
    void searchRequested(const QString &query);
    void filterRequested();

    void filterApplied(const StarFilterOptions& options);
    void filterCancelled();

private:
    Ui::StarView *ui;
    Qt3DExtras::Qt3DWindow *m_viewWindow;
    Qt3DCore::QEntity *m_rootEntity;

    Qt3DCore::QEntity* createScene();
    void handleStarClick(int id);

    int m_lastId = -1;
    QElapsedTimer m_clickTimer;

    QList<Star> m_currentStars;

    QMap<int, Qt3DExtras::QPhongMaterial*> m_starMaterials;
    QMap<int, Qt3DCore::QEntity*> m_starEntities;

    void onApplyFilterClicked();
    void onCancelFilterClicked();

};

#endif // STARVIEW_H
