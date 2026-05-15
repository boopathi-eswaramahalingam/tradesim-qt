#include "PortfolioRepository.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

bool PortfolioRepository::addPosition(int userId, const QString &symbol, int quantity, double avgBuy)
{
    QSqlQuery query;
    query.prepare("INSERT INTO portfolio (user_id, symbol, quantity, avg_buy) VALUES (?,?,?,?)");
    query.addBindValue(userId);
    query.addBindValue(symbol);
    query.addBindValue(quantity);
    query.addBindValue(avgBuy);

    if (!query.exec())
    {
        qWarning() << "addPosition failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PortfolioRepository::updatePosition(int userId, const QString &symbol, int quantity, double avgBuy)
{
    QSqlQuery query;
    query.prepare("UPDATE portfolio SET quantity = ?, avg_buy = ? WHERE user_id = ? AND symbol = ?");
    query.addBindValue(quantity);
    query.addBindValue(avgBuy);
    query.addBindValue(userId);
    query.addBindValue(symbol);

    if (!query.exec())
    {
        qWarning() << "updatePosition failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PortfolioRepository::removePosition(int userId, const QString &symbol)
{
    QSqlQuery query;
    query.prepare("DELETE FROM portfolio WHERE user_id = ? AND symbol = ?");
    query.addBindValue(userId);
    query.addBindValue(symbol);

    if (!query.exec())
    {
        qWarning() << "removePosition failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<PortfolioEntry> PortfolioRepository::getPortfolio(int userId)
{
    QList<PortfolioEntry> entries;
    QSqlQuery query;
    query.prepare("SELECT user_id, symbol, quantity, avg_buy FROM portfolio WHERE user_id = ?");
    query.addBindValue(userId);

    if (!query.exec())
        return entries;

    while (query.next())
    {
        PortfolioEntry e;
        e.userId = query.value(0).toInt();
        e.symbol = query.value(1).toString();
        e.quantity = query.value(2).toInt();
        e.avgBuy = query.value(3).toDouble();
        e.currentPrice = 0.0;
        entries.append(e);
    }
    return entries;
}

std::optional<PortfolioEntry> PortfolioRepository::getPosition(int userId, const QString &symbol)
{
    QSqlQuery query;
    query.prepare("SELECT user_id, symbol, quantity, avg_buy FROM portfolio WHERE user_id = ? AND symbol = ?");
    query.addBindValue(userId);
    query.addBindValue(symbol);

    if (!query.exec() || !query.next())
        return std::nullopt;

    PortfolioEntry e;
    e.userId = query.value(0).toInt();
    e.symbol = query.value(1).toString();
    e.quantity = query.value(2).toInt();
    e.avgBuy = query.value(3).toDouble();
    e.currentPrice = 0.0;
    return e;
}