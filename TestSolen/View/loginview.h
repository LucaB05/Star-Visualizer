#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui {
class LogInPage;
}

class LogInView : public QWidget
{
    Q_OBJECT

public:
    explicit LogInView(QWidget *parent = nullptr);
    ~LogInView();

public slots:
    void showErrorMessage(const QString &message);
    void clearErrorMessage();

signals:
    void createAccountRequested();
    void continueAsGuestRequested();
    void logInSubmitted(const QString &username, const QString &password);

private:
    Ui::LogInPage *ui;
    class QLabel *m_errorLabel;
};

#endif // LOGINVIEW_H
