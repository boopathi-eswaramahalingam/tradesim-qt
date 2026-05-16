#include "TradeEngine.h"

#include "../db/UserRepository.h"
#include "../db/PortfolioRepository.h"
#include "../db/TradeRepository.h"
#include <QDebug>

TradeEngine::TradeEngine(const User &user)
    : m_user(user) {}

bool TradeEngine::buy(const QString &symbol, int quantity, double currentPrice)
{
    double totalCost = quantity * currentPrice;

    if (m_user.balance < totalCost)
    {
        qWarning() << "Insufficient balance";
        return false;
    }

    auto existing = PortfolioRepository::getPosition(m_user.id, symbol);

    if (existing)
    {
        int newQty = existing->quantity + quantity;
        double newAvg = ((existing->avgBuy * existing->quantity) + (currentPrice * quantity)) / newQty;
        PortfolioRepository::updatePosition(m_user.id, symbol, newQty, newAvg);
    }
    else
    {
        PortfolioRepository::addPosition(m_user.id, symbol, quantity, currentPrice);
    }

    double newBalance = m_user.balance - totalCost;
    UserRepository::updateBalance(m_user.id, newBalance);
    m_user.balance = newBalance;

    TradeRepository::recordTrade(m_user.id, symbol, "BUY", quantity, currentPrice);

    qDebug() << "Bought" << quantity << "shares of" << symbol << "at" << currentPrice << "Total:" << totalCost;
    return true;
}

bool TradeEngine::sell(const QString &symbol, int quantity, double currentPrice)
{
    auto existing = PortfolioRepository::getPosition(m_user.id, symbol);

    if (!existing || existing->quantity < quantity)
    {
        qWarning() << "Insufficient shares to sell";
        return false;
    }

    int newQty = existing->quantity - quantity;

    if (newQty == 0)
        PortfolioRepository::removePosition(m_user.id, symbol);
    else
        PortfolioRepository::updatePosition(m_user.id, symbol, newQty, existing->avgBuy);

    double proceeds = quantity * currentPrice;
    double newBalance = m_user.balance + proceeds;
    UserRepository::updateBalance(m_user.id, newBalance);
    m_user.balance = newBalance;

    TradeRepository::recordTrade(m_user.id, symbol, "SELL", quantity, currentPrice);

    qDebug() << "Sold" << quantity << "shares of" << symbol
             << "at" << currentPrice << "Total:" << proceeds;
    return true;
}

void TradeEngine::refreshUser()
{
    auto updated = UserRepository::findByUsername(m_user.username);
    if (updated)
        m_user = *updated;
}