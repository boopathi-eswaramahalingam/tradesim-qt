#include "UserRepository.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QCryptographicHash>

static QString hashPassword(const QString &password)
{
    return QString(QCryptographicHash::hash(
                       password.toUtf8(),
                       QCryptographicHash::Sha256)
                       .toHex());
}

bool UserRepository::createUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, balance) VALUES (?, ?,?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    query.addBindValue(100000.0);

    if (!query.exec())
    {
        qWarning() << "createUser failed:" << query.lastError().text();
        return false;
    }

    qDebug() << "Created user:" << username << "with id:" << query.lastInsertId().toInt();
    return true;
}

std::optional<User> UserRepository::findByUsername(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT id, username, password, balance FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next())
    {
        return std::nullopt;
    }

    User user;
    user.id = query.value(0).toInt();
    user.username = query.value(1).toString();
    user.password = query.value(2).toString();
    user.balance = query.value(3).toDouble();
    return user;
}

bool UserRepository::updateBalance(int userId, double newBalance)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET balance = ? WHERE id= ?");
    query.addBindValue(newBalance);
    query.addBindValue(userId);

    if (!query.exec())
    {
        qWarning() << "updateBalance failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UserRepository::validateLogin(const QString &username, const QString &password)
{
    auto user = findByUsername(username);
    if (!user)
        return false;
    return user->password == hashPassword(password);
}