#ifndef STARCONTROLLER_H
#define STARCONTROLLER_H

#pragma once
#include "StarView.h"
#include "Star.h"
#include "StarModel.h"
#include <QTimer>

    class AuthController;

class StarController : public QObject {
    Q_OBJECT
public:
    StarController(StarModel *model, StarView *view);
    void start();
    void loadStar(const Star &star);
    void setDatabasePath(const QString &dbPath);
    void setAuthController(AuthController *authController);
    void handleSearch(const QString &query);

    QVector3D calculatePosition(const Star &star) const;

public slots:
    void handleAuthenticationStateChanged();
    void reloadStarsFromDatabase();

private:
    int m_pendingStarId = -1;
    int m_selectedStarId = -1;
    int m_viewTargetId = 0;
    bool m_isFirstPerson = false;
    int m_filterActive = 0;

    QTimer *m_hoverTimer;
    StarModel *m_model;
    StarView *m_view;
    AuthController *m_authController = nullptr;
    QString m_dbPath;

    QVector3D separate(QVector3D position, float radius, float space);
    float calculateStarRadius(float sunRadius, float vMag, float parallax, int temperature);
    float calculateRadius(const Star &star) const;

    void handleStarEntered(int id);
    void handleStarExited();
    void onStarSelected(int id);
    void onStarDoubleClicked(int id);
    void showHoverToolTip();
    void handleHomeButton();
    void handleFirstPersonView();
    void handleFavoritesRequested();
    void handleFavoriteStarSelected(const QString &starName);
    void handleFavoriteToggleRequested();
    void handleLogoutRequested();
    void showDefaultSidebar();
    void updateCameraView();
    void updateTopBarState();
    void updateFavoriteToggleState();

    void filterStars(QList<Star> &list, const QString &query) const;
    void sortStarsAlphabetically(QList<Star> &list) const;

    friend class test_separate;


    void handleFilterApplied(const StarFilterOptions& opts);

    void handleFilterCancelled();

};

#endif // STARCONTROLLER_H
