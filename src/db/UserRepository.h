#pragma once

#include "../models/Models.h"
#include <optional>
#include <QString>

class UserRepository
{
public:
    static bool createUser(const QString &username, const QString &password);
    static std::optional<User> findByUsername(const QString &username);
    static bool updateBalance(int userId, double newBalance);
    static bool validateLogin(const QString &username, const QString &password);
};