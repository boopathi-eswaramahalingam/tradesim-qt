#include "LoginDialog.h"

#include "../db/UserRepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void LoginDialog::setupUi()
{
    setWindowTitle("TradeSim - Login");
    setFixedSize(300, 200);

    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_loginBtn = new QPushButton("Login", this);
    m_registerBtn = new QPushButton("Register", this);
    m_messageLabel = new QLabel(this);

    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_messageLabel->setStyleSheet("color: red;");

    QFormLayout *form = new QFormLayout;
    form->addRow("Username:", m_usernameEdit);
    form->addRow("Password:", m_passwordEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_loginBtn);
    btnLayout->addWidget(m_registerBtn);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addLayout(btnLayout);
    main->addWidget(m_messageLabel);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

std::optional<User> LoginDialog::getLoggedInUser() const
{
    return m_loggedInUser;
}

void LoginDialog::onLogin()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        m_messageLabel->setText("Please fill in all fields.");
        return;
    }

    if (!UserRepository::validateLogin(username, password))
    {
        m_messageLabel->setText("Username not found.");
        return;
    }

    m_loggedInUser = UserRepository::findByUsername(username);
    accept();
}

void LoginDialog::onRegister()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        m_messageLabel->setText("Please fill in all fields.");
        return;
    }

    if (UserRepository::findByUsername(username))
    {
        m_messageLabel->setText("Username already exists.");
        return;
    }

    if (!UserRepository::createUser(username, password))
    {
        m_messageLabel->setText("Registration failed.");
        return;
    }

    m_loggedInUser = UserRepository::findByUsername(username);
    qDebug() << "New user registered:" << username << "id" << m_loggedInUser->id;
    accept();
}