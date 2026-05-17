#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../models/Models.h"
#include <optional>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    std::optional<User> getLoggedInUser() const;

private slots:
    void onLogin();
    void onRegister();

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
    QLabel *m_messageLabel;

    std::optional<User> m_loggedInUser;

    void setupUi();
};