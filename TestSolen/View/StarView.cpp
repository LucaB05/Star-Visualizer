#include "StarView.h"
#include "ui_StarView.h"
#include <utility>
#include <Qt3DRender>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QOrbitCameraController>
#include <QToolTip>
#include <QCursor>

StarView::StarView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StarView)
{
    ui->setupUi(this);
    this->setWindowTitle("Solen i 3D");
    this->resize(1024, 768);

    // Skapa scenen först
    m_rootEntity = createScene();

    // Skapa 3D-fönstret
    m_viewWindow = new Qt3DExtras::Qt3DWindow();
    m_viewWindow->defaultFrameGraph()->setClearColor(QColor(Qt::black));

    // Setup pick settings
    Qt3DRender::QPickingSettings *pickingSettings =
        m_viewWindow->renderSettings()->pickingSettings();

    pickingSettings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    pickingSettings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
    pickingSettings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);

    // Ställ in kameran så vi ser något
    m_viewWindow->camera()->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_viewWindow->camera()->setPosition(QVector3D(0, 0, 120.0f));
    m_viewWindow->camera()->setViewCenter(QVector3D(0, 0, 0));

    // Lägg till muskontroller för kameran
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(m_rootEntity);
    camController->setCamera(m_viewWindow->camera());
    camController->setLookSpeed(200.0f);
    camController->setLinearSpeed(100.0f);


    // Koppla scenen till fönstret
    m_viewWindow->setRootEntity(m_rootEntity);

    // Bädda in 3D-fönstret i din UI-layout (verticalLayout)
    QWidget *container = QWidget::createWindowContainer(m_viewWindow);
    ui->verticalLayout->addWidget(container);

    //kopplar hemknappen till rätt signal
    connect(ui->homeButton, &QPushButton::clicked, this, &StarView::homeButtonClicked);

    //första person

    connect(ui->firstPersonButton, &QPushButton::clicked, this, &StarView::firstPersonRequested);
    ui->favoritesButton->setVisible(false);
    connect(ui->favoriteToggleButton, &QPushButton::clicked, this, &StarView::favoriteToggleRequested);
    connect(ui->loginButton, &QPushButton::clicked, this, &StarView::loginRequested);
    connect(ui->logoutButton, &QPushButton::clicked, this, &StarView::logoutRequested);
    connect(ui->adminFunctionButton, &QPushButton::clicked, this, &StarView::adminRequested);
    connect(ui->favoritesList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            emit favoriteStarSelected(item->text());
        }
    });


    //sökning
    ui->SearchBox->setEditable(true);
    ui->SearchBox->setPlaceholderText("Sök stjärna...");
    connect(ui->SearchBox, &QComboBox::editTextChanged, this, [this](const QString &text) {
        qDebug() << "1. View: Text ändrad till:" << text;
        emit searchRequested(text);
    });
    connect(ui->SearchBox, &QComboBox::activated, this, &StarView::onSearchBoxActivated);

    connect(ui->filterButton, &QPushButton::clicked, this, &StarView::showFilterPage);

    //filtrering
    connect(ui->applyFilterButton, &QPushButton::clicked, this, &StarView::onApplyFilterClicked);
    connect(ui->cancelFilterButton, &QPushButton::clicked, this, &StarView::onCancelFilterClicked);

}

StarView::~StarView() {
    delete ui;
}

void StarView::showWindow() {
    QWidget *topLevel = window();
    if (topLevel) {
        topLevel->show();
    } else {
        this->show();
    }
}

Qt3DCore::QEntity* StarView::createScene() {
    m_rootEntity = new Qt3DCore::QEntity;
    return m_rootEntity;
}


Qt3DCore::QEntity* StarView::addStar(const Star& star) {
    int id = star.get_id();

    Qt3DCore::QEntity *starEntity = new Qt3DCore::QEntity(m_rootEntity);

    Qt3DExtras::QSphereMesh *starMesh = new Qt3DExtras::QSphereMesh();
    starMesh->setRadius(star.get_visualRadius());
    starMesh->setRings(20);
    starMesh->setSlices(20);

    Qt3DExtras::QPhongMaterial *starMaterial = new Qt3DExtras::QPhongMaterial();
    starMaterial->setDiffuse(star.get_baseColor());
    starMaterial->setAmbient(star.get_baseColor());

    Qt3DCore::QTransform *starTransform = new Qt3DCore::QTransform();
    starTransform->setTranslation(star.get_plottedPosition());

    Qt3DRender::QObjectPicker *picker = new Qt3DRender::QObjectPicker(starEntity);
    picker->setHoverEnabled(true); // optional but useful

    starEntity->addComponent(picker);
    starEntity->addComponent(starMesh);
    starEntity->addComponent(starMaterial);
    starEntity->addComponent(starTransform);

    starEntity->setProperty("id", star.get_id());


    QObject::connect(picker, &Qt3DRender::QObjectPicker::clicked, this, [this, id]() {
        this->handleStarClick(id);
    });

    QObject::connect(picker, &Qt3DRender::QObjectPicker::entered, this, [this, id]() {
        emit starHovered(id);
    });

    QObject::connect(picker, &Qt3DRender::QObjectPicker::exited, this, [this]() {
        emit starHoveredEnded();
    });

    m_starEntities[id] = starEntity;
    m_starMaterials[id] = starMaterial;



    return starEntity;
}

void StarView::clearStars()
{
    for (Qt3DCore::QEntity *entity : std::as_const(m_starEntities)) {
        if (entity) {
            entity->deleteLater();
        }
    }

    m_starEntities.clear();
    m_starMaterials.clear();
    m_currentStars.clear();
    ui->SearchBox->blockSignals(true);
    ui->SearchBox->clear();
    ui->SearchBox->blockSignals(false);
}

void StarView::handleStarClick(int id) {

    static int lastId = -1;
    static QElapsedTimer timer;

    if (timer.isValid() && timer.elapsed() < 300 && lastId == id) {
        emit starDoubleClicked(id);
        timer.invalidate();
    } else {
        timer.restart();
        lastId = id;
        emit starSelected(id);
    }
}



void StarView::flyToPosition(const QVector3D& targetPos, float distance) {
    if (!m_viewWindow || !m_viewWindow->camera()) return;

    Qt3DRender::QCamera *camera = m_viewWindow->camera();

    QVector3D direction = camera->position() - targetPos;

    if(direction.isNull()) {
        return;
    }

    direction.normalize();
    QVector3D targetCamPos = targetPos + (direction * distance);

    // 1. Animera "ViewCenter" (vart kameran tittar)
    QPropertyAnimation *centerAnim = new QPropertyAnimation(camera, "viewCenter");
    centerAnim->setDuration(1000); // 1 sekund
    centerAnim->setStartValue(camera->viewCenter());
    centerAnim->setEndValue(targetPos);
    centerAnim->setEasingCurve(QEasingCurve::InOutCubic); // Mjuk start och stopp

    // 2. Animera "Position" (var kameran är)
    QPropertyAnimation *posAnim = new QPropertyAnimation(camera, "position");
    posAnim->setDuration(1000);
    posAnim->setStartValue(camera->position());
    posAnim->setEndValue(targetCamPos);
    posAnim->setEasingCurve(QEasingCurve::InOutCubic);

    // Starta animationerna
    centerAnim->start(QAbstractAnimation::DeleteWhenStopped);
    posAnim->start(QAbstractAnimation::DeleteWhenStopped);


}

void StarView::showStarToolTip(const QString& text) {
    QToolTip::showText(QCursor::pos(), text);
}

void StarView::hideStarToolTip() {
    QToolTip::hideText();
}

void StarView::displayStarInfo(const QMap<QString, QString>& info) {

    ui->stackedWidget->setCurrentWidget(ui->starInfoPage);

    ui->l_StarName->setText(info["name"]);
    ui->starNameValue->setText(info["name"]);
    ui->starIdValue->setText(info["id"]);
    ui->starRaValue->setText(info["ra"]);
    ui->starDecValue->setText(info["dec"]);
    ui->starParallaxValue->setText(info["parallax"]);
    ui->starVMagValue->setText(info["vMag"]);
    ui->starBMagValue->setText(info["bMag"]);
    ui->starSpectralTypeValue->setText(info["spectral"]);
}

void StarView::showFilterPage()
{
    ui->stackedWidget->setCurrentWidget(ui->filterPage);
    ui->l_StarName->setText("Filter Stars");
}

//Byt till logged in page i sidebaren och uppdatera alla fält
void StarView::displayLoggedInUserData(const QString& username, const QStringList& favorites, bool isAdmin) {
    ui->stackedWidget->setCurrentWidget(ui->loggedInPage);
    ui->favoritesContainer->setTitle(username + "'s Favorites");
    ui->favoritesList->clear();
    ui->favoritesList->addItems(favorites);
    ui->adminFunctionButton->setVisible(isAdmin);
    ui->l_StarName->clear();
}
//Byt till logged out page i sidebaren och ta bort all data
void StarView::displayLoggedOutUserData() {
    ui->stackedWidget->setCurrentWidget(ui->loggedOutPage);
    ui->favoritesList->clear();
    ui->favoritesContainer->setTitle("Favorites");
    ui->adminFunctionButton->setVisible(false);
    ui->l_StarName->clear();
}

void StarView::setFirstPersonButtonVisible(bool visible) {
    ui->firstPersonButton->setVisible(visible);
}

void StarView::setFavoritesButtonVisible(bool visible)
{
    Q_UNUSED(visible);
    ui->favoritesButton->setVisible(false);
}

void StarView::setFavoriteToggleVisible(bool visible)
{
    ui->favoriteToggleButton->setVisible(visible);
}

void StarView::setFavoriteToggleText(const QString &text)
{
    ui->favoriteToggleButton->setText(text);
    const bool isRemoveAction = text.compare("Remove Favorite", Qt::CaseInsensitive) == 0;
    ui->favoriteToggleButton->setStyleSheet(QString(
        "QPushButton {"
        "color: white;"
        "background-color: black;"
        "border: 1px solid %1;"
        "border-radius: 8px;"
        "padding: 8px 14px;"
        "}"
        "QPushButton:hover {"
        "border-color: #aaaaaa;"
        "}"
    ).arg(isRemoveAction ? "#b94a48" : "#2f9e44"));
}

void StarView::setFirstPersonButtonText(const QString &text) {
    ui->firstPersonButton->setText(text);
}

void StarView::updateStarList(const QList<Star> &stars) {

    //för att vi ska kunna testa funktionen
    m_currentStars = stars;

    QString filterText = ui->SearchBox->currentText();


    ui->SearchBox->blockSignals(true);

    ui->SearchBox->clear();
    for (const Star &star : stars) {
        ui->SearchBox->addItem(star.get_name(), star.get_id());
    }


    ui->SearchBox->setEditText(filterText);

    ui->SearchBox->blockSignals(false);
}

void StarView::onSearchBoxActivated(int index) {

    int starId = ui->SearchBox->itemData(index).toInt();

    emit starSelected(starId);
    emit starDoubleClicked(starId);

}

void StarView::updateStarState(int id, StarState state, const QColor& baseColor) {
    if (!m_starMaterials.contains(id)) {
        return;
    }

    Qt3DExtras::QPhongMaterial *material = m_starMaterials[id];

    if (state == StarState::Filtered) {

        QColor grayed(60, 60, 60);
        material->setDiffuse(grayed);
        material->setAmbient(grayed);
    }
    else {

        material->setDiffuse(baseColor);
        material->setAmbient(baseColor);
    }
}

void StarView::onApplyFilterClicked() {
    StarFilterOptions opts;

    // Fyll på ALLA spektralklasser i kartan (Se till att namnen på dina checkboxar stämmer!)
    opts.spectralClasses["O"] = ui->spectralOCheckBox->isChecked();
    opts.spectralClasses["B"] = ui->spectralBCheckBox->isChecked();
    opts.spectralClasses["A"] = ui->spectralACheckBox->isChecked();
    opts.spectralClasses["F"] = ui->spectralFCheckBox->isChecked();
    opts.spectralClasses["G"] = ui->spectralGCheckBox->isChecked();
    opts.spectralClasses["K"] = ui->spectralKCheckBox->isChecked();
    opts.spectralClasses["M"] = ui->spectralMCheckBox->isChecked();
    opts.spectralClasses["L"] = ui->spectralLCheckBox->isChecked();
    opts.spectralClasses["T"] = ui->spectralTCheckBox->isChecked();





    opts.minDist = ui->minDistanceSpinBox->value();
    opts.maxDist = ui->maxDistanceSpinBox->value();
    opts.minTemp = ui->minTemperatureSpinBox->value();
    opts.maxTemp = ui->maxTemperatureSpinBox->value();
    opts.minRadius = ui->minRadiusSpinBox->value();
    opts.maxRadius = ui->maxRadiusSpinBox->value();
    opts.minMass = ui->minMassSpinBox->value();
    opts.maxMass = ui->maxMassSpinBox->value();
    opts.minVMag = ui->minVMagSpinBox->value();
    opts.maxVMag = ui->maxVMagSpinBox->value();

    emit filterApplied(opts);
}

void StarView::onCancelFilterClicked() {

    emit filterCancelled();

}

