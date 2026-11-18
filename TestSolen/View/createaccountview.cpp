#include "createaccountview.h"
#include "ui_createAccount.h"
#include <QLabel>

createAccountView::createAccountView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreateAccountPage)
    , m_errorLabel(new QLabel(this))
{
    ui->setupUi(this);

    m_errorLabel->setVisible(false);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setStyleSheet("color: #ff6b6b;");
    ui->cardLayout->insertWidget(ui->cardLayout->indexOf(ui->buttonSpacer), m_errorLabel);

    connect(ui->linkLabel, &QLabel::linkActivated, this, [this](const QString &) {
        clearErrorMessage();
        emit logInRequested();
    });

    connect(ui->createAccountButton, &QPushButton::clicked, this, [this]() {
        clearErrorMessage();
        emit createAccountSubmitted(ui->nameLineEdit->text(), ui->passwordLineEdit->text());
    });

    connect(ui->continueAsGuestButton, &QPushButton::clicked, this, [this]() {
        clearErrorMessage();
        emit continueAsGuestRequested();
    });
}

createAccountView::~createAccountView()
{
    delete ui;
}

void createAccountView::showErrorMessage(const QString &message)
{
    m_errorLabel->setText(message);
    m_errorLabel->setVisible(true);
}

void createAccountView::clearErrorMessage()
{
    m_errorLabel->clear();
    m_errorLabel->setVisible(false);
}
