#include "StarController.h"
#include "AuthController.h"
#include "stardatabase.h"
#include <QObject>



StarController::StarController(StarModel *model, StarView *view)
    : m_model(model), m_view(view)
{
    connect(m_view, &StarView::starHovered, this, &StarController::handleStarEntered);
    connect(m_view, &StarView::starHoveredEnded, this, &StarController::handleStarExited);
    connect(m_view, &StarView::starSelected, this, &StarController::onStarSelected);
    connect(m_view, &StarView::starDoubleClicked, this, &StarController::onStarDoubleClicked);
    connect(m_view, &StarView::searchRequested, this, &StarController::handleSearch);
    connect(m_view, &StarView::homeButtonClicked, this, &StarController::handleHomeButton);
    connect(m_view, &StarView::firstPersonRequested, this, &StarController::handleFirstPersonView);
    connect(m_view, &StarView::favoriteToggleRequested, this, &StarController::handleFavoriteToggleRequested);
    connect(m_view, &StarView::favoriteStarSelected, this, &StarController::handleFavoriteStarSelected);
    connect(m_view, &StarView::logoutRequested, this, &StarController::handleLogoutRequested);

    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setSingleShot(true);
    m_hoverTimer->setInterval(300);
    connect(m_hoverTimer, &QTimer::timeout, this, &StarController::showHoverToolTip);


    connect(m_view, &StarView::filterApplied, this, &StarController::handleFilterApplied);
    connect(m_view, &StarView::filterCancelled, this, &StarController::handleFilterCancelled);
}

void StarController::handleLogoutRequested()
{
    if (m_authController) {
        m_authController->handleLogoutRequest();
    }
}

void StarController::showDefaultSidebar()
{
    if (!m_view) {
        return;
    }

    if (m_authController && m_authController->isLoggedIn()) {
        const User user = m_authController->currentUser();
        m_view->displayLoggedInUserData(user.username(), user.favoriteStars(), user.isAdmin());
    } else {
        m_view->displayLoggedOutUserData();
    }

    updateTopBarState();
}

void StarController::start()
{
    if (m_view) {
        showDefaultSidebar();
        m_view->showWindow();
    }


}

float StarController::calculateStarRadius(float sunRadius, float vMag, float parallax, int temperature)
{
    if (parallax <= 0.0f) {
        return sunRadius;
    }

    const float distance = 1.0f / parallax;
    const float absoluteMag = vMag - (5.0f * log(distance)) + 5.0f;
    const float luminosity = pow(10.0f, (4.83f - absoluteMag) / 2.5f);

    float radius = sqrt(luminosity / pow(float(temperature) / 5778.0f, 4));
    radius = log(radius * sunRadius * 1'000'000.0f) * 1.8f + 2.0f;

    if (radius < 2.0f || radius == 999999.0f) {
        radius = sunRadius;
    }

    return radius;
}


void StarController::setAuthController(AuthController *authController)
{
    m_authController = authController;
}

void StarController::setDatabasePath(const QString &dbPath)
{
    m_dbPath = dbPath;
}

void StarController::loadStar(const Star &rawStar)
{
    if (!m_view || !m_model) {
        return;
    }

    Star star = rawStar;
    float sun_size = 20.0f;
    QVector3D finalPosition;
    float finalSize;

    if (star.get_name() == "Solen") { // måste rensas bort
        finalPosition = QVector3D(0, 0, 0);
        finalSize = sun_size;
        // starColor = Qt::darkMagenta;
    }

    else {
        finalSize = calculateStarRadius(sun_size, star.get_vMag(), star.get_parallax(), star.get_temp());
        QVector3D rawPosition = calculatePosition(star);
        finalPosition = separate(rawPosition, finalSize, 0.5f);
    }

    star.set_plottedPosition(finalPosition);
    star.set_visualRadius(finalSize);
    star.set_baseColor();

    m_model->addStar(star);
    m_view->addStar(star);
}

void StarController::reloadStarsFromDatabase()
{
    if (!m_model || !m_view || m_dbPath.isEmpty()) {
        return;
    }

    m_selectedStarId = -1;
    m_pendingStarId = -1;
    m_viewTargetId = 0;
    m_model->clear();
    m_view->clearStars();

    Star sun(0, "Solen", 0.0f, 0.0f, 999999.0f, -26.7f, -26.1f, "G2V");
    loadStar(sun);

    StarDatabase db(m_dbPath);
    if (!db.openConnection()) {
        updateTopBarState();
        return;
    }

    const QList<Star> stars = db.fetchStars(1000);
    for (const Star &star : stars) {
        loadStar(star);
    }

    updateTopBarState();
}

void StarController::handleAuthenticationStateChanged()
{
    showDefaultSidebar();
}

void StarController::handleStarEntered(int id)
{
    m_pendingStarId = id;
    m_hoverTimer->start();
}

void StarController::handleStarExited()
{
    m_hoverTimer->stop();
    m_pendingStarId = -1;
    m_view->hideStarToolTip();
}

void StarController::showHoverToolTip()
{
    if (m_pendingStarId != -1) {
        QString name = m_model->getStarName(m_pendingStarId);
        m_view->showStarToolTip(name);
    }
}

void StarController::onStarSelected(int id)
{
    qDebug() << "Enkelklick pa stjarna:" << m_model->getStarName(id) << "(ID:" << id << ")";
    m_selectedStarId = id;

    Star star = m_model->getStar(id);

    QMap<QString, QString> info;
    info["name"] = star.get_name();
    info["id"] = QString::number(star.get_id());
    info["ra"] = QString::number(star.get_ra(), 'f', 3);
    info["dec"] = QString::number(star.get_dec(), 'f', 3);
    info["parallax"] = QString::number(star.get_parallax(), 'f', 3);
    info["vMag"] = QString::number(star.get_vMag(), 'f', 3);
    info["bMag"] = QString::number(star.get_bMag(), 'f', 3);
    info["spectral"] = star.get_spectralType().isEmpty() ? "-" : star.get_spectralType();

    m_view->displayStarInfo(info);
    updateTopBarState();
    updateFavoriteToggleState();
}

void StarController::onStarDoubleClicked(int id)
{
    m_viewTargetId = id;
    updateCameraView();
}


QVector3D StarController::calculatePosition(const Star &star) const
{
    //testar division med noll
    if(star.get_parallax() <= 0.000001f) {
        return QVector3D(0,0,0);
    }

    float distance = 1000.0f / star.get_parallax();
    float scaledDistance = distance * 60.0f;

    float raRad = qDegreesToRadians(star.get_ra());
    float decRad = qDegreesToRadians(star.get_dec());

    float x = scaledDistance * qCos(decRad) * qCos(raRad);
    float y = scaledDistance * qCos(decRad) * qSin(raRad);
    float z = scaledDistance * qSin(decRad);

    return QVector3D(x, y, z);
}

void StarController::handleHomeButton()
{
    m_viewTargetId = 0;
    m_selectedStarId = -1;
    m_isFirstPerson = false;
    m_view->setFirstPersonButtonText("To First person");
    updateCameraView();
    showDefaultSidebar();
    updateFavoriteToggleState();
}

void StarController::handleFirstPersonView()
{
    m_isFirstPerson = !m_isFirstPerson;

    if (m_isFirstPerson) {
        m_view->setFirstPersonButtonText("To Third Person");
    } else {
        m_view->setFirstPersonButtonText("To First Person");
    }

    updateCameraView();
}

void StarController::handleFavoritesRequested()
{
    if (!m_authController || !m_authController->isLoggedIn() || m_selectedStarId < 0) {
        return;
    }

    if (m_authController->addFavoriteForCurrentUser(m_selectedStarId)) {
        showDefaultSidebar();
    }
}

void StarController::handleFavoriteToggleRequested()
{
    if (!m_authController || !m_authController->isLoggedIn() || m_selectedStarId < 0) {
        return;
    }

    const bool isFavorite = m_authController->isFavoriteForCurrentUser(m_selectedStarId);
    const bool success = isFavorite
        ? m_authController->removeFavoriteForCurrentUser(m_selectedStarId)
        : m_authController->addFavoriteForCurrentUser(m_selectedStarId);

    if (success) {
        onStarSelected(m_selectedStarId);
    }
}

void StarController::handleFavoriteStarSelected(const QString &starName)
{
    if (!m_model) {
        return;
    }

    const int starId = m_model->getStarIdByName(starName);
    if (starId < 0) {
        return;
    }

    onStarSelected(starId);
    onStarDoubleClicked(starId);
}

void StarController::updateCameraView()
{
    if (!m_view || !m_model) {
        return;
    }

    Star targetStar = m_model->getStar(m_viewTargetId);
    QVector3D pos = targetStar.get_plottedPosition();

    float offset = m_isFirstPerson ? 0.5f : 120.0f;
    m_view->flyToPosition(pos, offset);
}

void StarController::updateTopBarState()
{
    if (!m_view) {
        return;
    }

    const bool hasActiveStar = m_selectedStarId >= 0;
    const bool isLoggedIn = m_authController && m_authController->isLoggedIn();

    m_view->setFirstPersonButtonVisible(hasActiveStar);
    m_view->setFavoritesButtonVisible(false);
    m_view->setFavoriteToggleVisible(hasActiveStar && isLoggedIn);
}

void StarController::updateFavoriteToggleState()
{
    if (!m_view) {
        return;
    }

    const bool canShowFavoriteButton = m_selectedStarId >= 0 && m_authController && m_authController->isLoggedIn();
    m_view->setFavoriteToggleVisible(canShowFavoriteButton);

    if (!canShowFavoriteButton) {
        return;
    }

    const bool isFavorite = m_authController->isFavoriteForCurrentUser(m_selectedStarId);
    m_view->setFavoriteToggleText(isFavorite ? "Remove Favorite" : "Add Favorite");
}

QVector3D StarController::separate(QVector3D position, float radius, float space)
{
    QVector3D adjustedPos = position;
    bool overlapping = true;
    int attempts = 0;

    const auto &existingStars = m_model->getAllStars();

    while (overlapping && attempts < 10) {
        overlapping = false;

        for (const Star &existingStar : existingStars) {
            float distance = adjustedPos.distanceToPoint(existingStar.get_plottedPosition());
            float minDistance = existingStar.get_visualRadius() + radius + space;

            if (distance < minDistance) {
                adjustedPos.setX(adjustedPos.x() + (minDistance * 0.5f));
                adjustedPos.setY(adjustedPos.y() + (minDistance * 0.5f));
                overlapping = true;
                break;
            }
        }
        attempts++;
    }

    return adjustedPos;
}

void StarController::handleSearch(const QString &query)
{
    if (!m_view || !m_model) {
        return;
    }

    QList<Star> results = m_model->getAllStars().values();
    filterStars(results, query);
    sortStarsAlphabetically(results);
    m_view->updateStarList(results);
}

void StarController::filterStars(QList<Star> &list, const QString &query) const
{
    if (query.isEmpty()) {
        return;
    }

    list.removeIf([&query](const Star &s) {
        return !s.get_name().contains(query, Qt::CaseInsensitive);
    });
}

void StarController::sortStarsAlphabetically(QList<Star> &list) const
{
    std::sort(list.begin(), list.end(), [](const Star &a, const Star &b) {
        return a.get_name().toLower() < b.get_name().toLower();
    });
}


void StarController::handleFilterApplied(const StarFilterOptions& opts) {


    m_filterActive = true;

    for (int id : m_model->getAllIds()) {
        Star star = m_model->getStar(id);
        StarState newState = StarState::Normal;


        bool matchesTemp = false;
        if (opts.minTemp == 0 && opts.maxTemp == 0) {
            matchesTemp = true;
        } else {
            matchesTemp = (star.get_temp() >= opts.minTemp && star.get_temp() <= opts.maxTemp);
        }


        bool matchesDist = false;
        if (opts.minDist == 0 && opts.maxDist == 0) {
            matchesDist = true;
        } else {
            float dist = (star.get_parallax() > 0.000001f) ? (1000.0f / star.get_parallax()) : 999999.0f;
            matchesDist = (dist >= opts.minDist && dist <= opts.maxDist);
        }


        bool matchesRadius = false;
        if (opts.minRadius == 0 && opts.maxRadius == 0) {
            matchesRadius = true;
        } else {
            matchesRadius = (star.get_visualRadius() >= opts.minRadius && star.get_visualRadius() <= opts.maxRadius);
        }
        bool matchesMass = true;
        //vet inte om vi kommer fixa massa, satte till true så länge


        bool matchesVMag = false;
        if (opts.minVMag == 0 && opts.maxVMag == 0) {
            matchesVMag = true; // Filtret är inaktivt
        } else {
            matchesVMag = (star.get_vMag() >= opts.minVMag && star.get_vMag() <= opts.maxVMag);
        }


        QString starSpectral = star.get_spectralType().trimmed();
        bool matchesSpectral = false;

        // tillåter alla spektralklasser om man ej specificerat någon
        bool anySpectralChecked = false;
        for (bool checked : opts.spectralClasses) {
            if (checked) {
                anySpectralChecked = true;
                break;
            }
        }


        if (!anySpectralChecked) {
            matchesSpectral = true;
        }

        else if (starSpectral.isEmpty() || starSpectral == "-") {
            matchesSpectral = false;
        }
        // jämför grundklassen, tar bort prefix
        else {
            QString cleanSpectral = starSpectral;

            // Fall A: Tar bort "sd" (Subdwarf, t.ex. sdM3.0 -> M3.0)
            if (cleanSpectral.startsWith("sd", Qt::CaseInsensitive)) {
                cleanSpectral = cleanSpectral.mid(2);
            }
            // Fall B: Tar bort litet 'd' (Dwarf, t.ex. dM4.0 -> M4.0)
            else if (cleanSpectral.startsWith("d") && !cleanSpectral.startsWith("D")) {
                cleanSpectral = cleanSpectral.mid(1);
            }
            // Fall C: Tar bort stort 'D' (Vit dvärg, t.ex. DA2.4 -> A2.4)
            else if (cleanSpectral.startsWith("D")) {
                cleanSpectral = cleanSpectral.mid(1);
            }


            for (auto it = opts.spectralClasses.constBegin(); it != opts.spectralClasses.constEnd(); ++it) {
                if (it.value() && cleanSpectral.startsWith(it.key(), Qt::CaseInsensitive)) {
                    matchesSpectral = true;
                    break;
                }
            }
        }

        bool totalMatch = matchesTemp && matchesDist && matchesRadius && matchesMass && matchesVMag && matchesSpectral;


        newState = totalMatch ? StarState::Normal : StarState::Filtered;


        m_model->setStarState(id, newState);
        m_view->updateStarState(id, newState, star.get_baseColor());
    }
}

void StarController::handleFilterCancelled() {

    m_filterActive = false;


    for (int id : m_model->getAllIds()) {
        Star star = m_model->getStar(id);


        m_model->setStarState(id, StarState::Normal);
        m_view->updateStarState(id, StarState::Normal, star.get_baseColor());
    }

}
