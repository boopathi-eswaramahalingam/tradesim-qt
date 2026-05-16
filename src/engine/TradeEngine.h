#pragma once

#include "../models/Models.h"
#include <QString>

class TradeEngine
{
public:
    explicit TradeEngine(const User &user);

    bool buy(const QString &symbol, int quantity, double currentPrice);
    bool sell(const QString &symbol, int quantity, double currentPrice);

    const User &currentUser() const { return m_user; }
    void refreshUser();

private:
    User m_user;
};