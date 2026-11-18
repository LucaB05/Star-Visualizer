#ifndef CREATEACCOUNTVIEW_H
#define CREATEACCOUNTVIEW_H

#include <QWidget>

namespace Ui {
class CreateAccountPage;
}

class createAccountView : public QWidget
{
    Q_OBJECT

public:
    explicit createAccountView(QWidget *parent = nullptr);
    ~createAccountView();

public slots:
    void showErrorMessage(const QString &message);
    void clearErrorMessage();

signals:
    void logInRequested();
    void continueAsGuestRequested();
    void createAccountSubmitted(const QString &username, const QString &password);

private:
    Ui::CreateAccountPage *ui;
    class QLabel *m_errorLabel;
};

#endif // CREATEACCOUNTVIEW_H
