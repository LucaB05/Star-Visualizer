#include "loginview.h"
#include "ui_logIn.h"
#include <QLabel>

LogInView::LogInView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogInPage)
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
        emit createAccountRequested();
    });

    connect(ui->logInButton, &QPushButton::clicked, this, [this]() {
        clearErrorMessage();
        emit logInSubmitted(ui->nameLineEdit->text(), ui->passwordLineEdit->text());
    });

    connect(ui->continueAsGuestButton, &QPushButton::clicked, this, [this]() {
        clearErrorMessage();
        emit continueAsGuestRequested();
    });
}

LogInView::~LogInView()
{
    delete ui;
}

void LogInView::showErrorMessage(const QString &message)
{
    m_errorLabel->setText(message);
    m_errorLabel->setVisible(true);
}

void LogInView::clearErrorMessage()
{
    m_errorLabel->clear();
    m_errorLabel->setVisible(false);
}
