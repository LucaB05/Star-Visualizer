#include "adminview.h"
#include "ui_adminview.h"
#include <QListWidgetItem>
#include <QPushButton>
#include <QRandomGenerator>

AdminView::AdminView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminView)
{
    ui->setupUi(this);

    for (QPushButton *button : findChildren<QPushButton*>()) {
        button->setCursor(Qt::PointingHandCursor);
    }

    connect(ui->userSettingsButton, &QPushButton::clicked, this, &AdminView::showUserSettingsPage);
    connect(ui->starSettingsButton, &QPushButton::clicked, this, &AdminView::showStarSettingsPage);
    connect(ui->backFromUserSettingsButton, &QPushButton::clicked, this, &AdminView::showMenuPage);
    connect(ui->backFromStarSettingsButton, &QPushButton::clicked, this, &AdminView::showMenuPage);
    connect(ui->adminBackToHomeButton, &QPushButton::clicked, this, &AdminView::backToHomeRequested);
    connect(ui->userSearchLineEdit, &QLineEdit::textChanged, this, &AdminView::userSearchChanged);
    connect(ui->resetUserSearchButton, &QPushButton::clicked, ui->userSearchLineEdit, &QLineEdit::clear);
    connect(ui->addUserButton, &QPushButton::clicked, this, &AdminView::showAddUserPage);
    connect(ui->saveUserButton, &QPushButton::clicked, this, [this]() {
        User editedUser(
            m_selectedUserId,
            ui->usernameLineEdit->text().trimmed(),
            ui->passwordLineEdit->text(),
            ui->adminRoleCheckBox->isChecked(),
            {}
            );

        emit saveUserRequested(editedUser, ui->confirmPasswordLineEdit->text());
    });
    connect(ui->deleteUserButton, &QPushButton::clicked, this, [this]() {
        emit deleteUserRequested(m_selectedUserId, ui->usernameLineEdit->text().trimmed());
    });
    connect(ui->cancelAddUserButton, &QPushButton::clicked, this, [this]() {
        clearAddUserForm();
        showEditUserPage();
    });
    connect(ui->addUserConfirmButton, &QPushButton::clicked, this, [this]() {
        User newUser(
            ui->addUsernameLineEdit->text().trimmed(),
            ui->addPasswordLineEdit->text(),
            ui->addAdminRoleCheckBox->isChecked(),
            {}
            );

        emit addUserRequested(newUser, ui->addConfirmPasswordLineEdit->text());
    });
    connect(ui->starSearchLineEdit, &QLineEdit::textChanged, this, &AdminView::starSearchChanged);
    connect(ui->resetStarSearchButton, &QPushButton::clicked, ui->starSearchLineEdit, &QLineEdit::clear);
    connect(ui->addStarButton, &QPushButton::clicked, this, &AdminView::showAddStarMode);
    connect(ui->generateStarPositionButton, &QPushButton::clicked, this, &AdminView::generateStarPosition);
    connect(ui->saveStarButton, &QPushButton::clicked, this, [this]() {
        if (m_addingStar) {
            emit addStarRequested(starFromEditor(-1));
        } else {
            emit saveStarRequested(starFromEditor(m_selectedStarId));
        }
    });
    connect(ui->deleteStarButton, &QPushButton::clicked, this, [this]() {
        if (m_addingStar) {
            showEditStarMode();
            clearSelectedStar();
        } else {
            emit deleteStarRequested(m_selectedStarId, ui->starNameLineEdit->text().trimmed());
        }
    });
    connect(ui->userListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            emit userSelected(item->text());
        }
    });
    connect(ui->starListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            emit starSelected(item->text());
        }
    });
}

AdminView::~AdminView()
{
    delete ui;
}

void AdminView::showMenuPage()
{
    ui->adminStackedWidget->setCurrentWidget(ui->adminMenuPage);
}

void AdminView::showUserSettingsPage()
{
    ui->adminStackedWidget->setCurrentWidget(ui->userSettingsPage);
    showEditUserPage();
    emit userSettingsOpened();
}

void AdminView::showEditUserPage()
{
    ui->userEditorStackedWidget->setCurrentWidget(ui->editUserPage);
}

void AdminView::showAddUserPage()
{
    clearSelectedUser();
    clearAddUserForm();
    ui->userEditorStackedWidget->setCurrentWidget(ui->addUserPage);
}

void AdminView::showStarSettingsPage()
{
    ui->adminStackedWidget->setCurrentWidget(ui->starSettingsPage);
    showEditStarMode();
    emit starSettingsOpened();
}

void AdminView::displayUsers(const QList<User> &users)
{
    ui->userListWidget->blockSignals(true);
    ui->userListWidget->clear();

    for (const User &user : users) {
        ui->userListWidget->addItem(user.username());
    }

    ui->userListWidget->blockSignals(false);
}

void AdminView::displaySelectedUser(const User &user)
{
    showEditUserPage();
    m_selectedUserId = user.id();
    ui->usernameLineEdit->setText(user.username());
    ui->passwordLineEdit->setText(user.password());
    ui->confirmPasswordLineEdit->setText(user.password());
    ui->adminRoleCheckBox->setChecked(user.isAdmin());
    clearUserMessage();
}

void AdminView::clearSelectedUser()
{
    m_selectedUserId = -1;
    ui->usernameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
    ui->adminRoleCheckBox->setChecked(false);
    clearUserMessage();
}

void AdminView::showUserMessage(const QString &message, bool isError)
{
    ui->userStatusLabel->setText(message);
    ui->userStatusLabel->setStyleSheet(isError ? "color: #ff6b6b;" : "color: #51cf66;");
    ui->userStatusLabel->setVisible(true);
}

void AdminView::clearUserMessage()
{
    ui->userStatusLabel->clear();
    ui->userStatusLabel->setVisible(false);
}

void AdminView::showAddUserMessage(const QString &message, bool isError)
{
    ui->addUserStatusLabel->setText(message);
    ui->addUserStatusLabel->setStyleSheet(isError ? "color: #ff6b6b;" : "color: #51cf66;");
    ui->addUserStatusLabel->setVisible(true);
}

void AdminView::clearAddUserMessage()
{
    ui->addUserStatusLabel->clear();
    ui->addUserStatusLabel->setVisible(false);
}

void AdminView::clearAddUserForm()
{
    ui->addUsernameLineEdit->clear();
    ui->addPasswordLineEdit->clear();
    ui->addConfirmPasswordLineEdit->clear();
    ui->addAdminRoleCheckBox->setChecked(false);
    clearAddUserMessage();
}

void AdminView::displayStars(const QList<Star> &stars)
{
    ui->starListWidget->blockSignals(true);
    ui->starListWidget->clear();

    for (const Star &star : stars) {
        ui->starListWidget->addItem(star.get_name());
    }

    ui->starListWidget->blockSignals(false);
}

void AdminView::displaySelectedStar(const Star &star)
{
    showEditStarMode();
    m_selectedStarId = star.get_id();
    ui->starNameLineEdit->setText(star.get_name());
    ui->raSpinBox->setValue(star.get_ra());
    ui->decSpinBox->setValue(star.get_dec());
    ui->parallaxSpinBox->setValue(star.get_parallax());
    ui->distanceFromSunSpinBox->setValue(star.get_parallax() > 0.000001f ? 1000.0 / star.get_parallax() : 0.0);
    ui->vMagSpinBox->setValue(star.get_vMag());
    ui->bMagSpinBox->setValue(star.get_bMag());
    ui->spectralTypeLineEdit->setText(star.get_spectralType());
    clearStarMessage();
}

void AdminView::clearSelectedStar()
{
    m_selectedStarId = -1;
    ui->starNameLineEdit->clear();
    ui->raSpinBox->setValue(0.0);
    ui->decSpinBox->setValue(0.0);
    ui->parallaxSpinBox->setValue(0.0);
    ui->distanceFromSunSpinBox->setValue(0.0);
    ui->vMagSpinBox->setValue(0.0);
    ui->bMagSpinBox->setValue(0.0);
    ui->spectralTypeLineEdit->clear();
    clearStarMessage();
}

void AdminView::showStarMessage(const QString &message, bool isError)
{
    ui->starStatusLabel->setText(message);
    ui->starStatusLabel->setStyleSheet(isError ? "color: #ff6b6b;" : "color: #51cf66;");
    ui->starStatusLabel->setVisible(true);
}

void AdminView::clearStarMessage()
{
    ui->starStatusLabel->clear();
    ui->starStatusLabel->setVisible(false);
}

Star AdminView::starFromEditor(int id) const
{
    Star star(
        id,
        ui->starNameLineEdit->text().trimmed(),
        ui->raSpinBox->value(),
        ui->decSpinBox->value(),
        ui->parallaxSpinBox->value(),
        ui->vMagSpinBox->value(),
        ui->bMagSpinBox->value(),
        ui->spectralTypeLineEdit->text().trimmed()
        );
    star.set_temp();
    star.set_baseColor();
    return star;
}

void AdminView::showEditStarMode()
{
    m_addingStar = false;
    ui->starEditorTitleLabel->setText("Star Details");
    ui->deleteStarButton->setText("Delete Star");
    ui->saveStarButton->setText("Save Star");
    ui->saveStarButton->setStyleSheet(QString());
    ui->generateStarPositionButton->setVisible(false);
    ui->distanceFromSunSpinBox->setVisible(false);
    ui->distanceFromSunLabel->setVisible(false);
}

void AdminView::showAddStarMode()
{
    m_addingStar = true;
    clearSelectedStar();
    ui->starEditorTitleLabel->setText("Add New Star");
    ui->deleteStarButton->setText("Cancel");
    ui->saveStarButton->setText("Add Star");
    ui->saveStarButton->setStyleSheet("border-color: #2f9e44;");
    ui->generateStarPositionButton->setVisible(true);
    ui->distanceFromSunSpinBox->setVisible(true);
    ui->distanceFromSunLabel->setVisible(true);
}

void AdminView::generateStarPosition()
{
    const double distance = ui->distanceFromSunSpinBox->value();
    if (distance <= 0.0) {
        showStarMessage("Error generating: distance must be greater than 0.", true);
        return;
    }

    ui->raSpinBox->setValue(QRandomGenerator::global()->generateDouble() * 360.0);
    ui->decSpinBox->setValue((QRandomGenerator::global()->generateDouble() * 180.0) - 90.0);
    ui->parallaxSpinBox->setValue(1000.0 / distance);
    showStarMessage("Position generated from distance.", false);
}
