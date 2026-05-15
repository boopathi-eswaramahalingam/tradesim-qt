#pragma once

#include <QString>

struct User
{
    int id;
    QString username;
    QString password;
    double balance;
};

struct Stock
{
    int id;
    QString symbol;
    QString name;
    double price;
    double changePercent;
};

struct PortfolioEntry
{
    int userId;
    QString symbol;
    int quantity;
    double avgBuy;
    double currentPrice;

    double value() const { return quantity * currentPrice; }
    double pnl() const { return (currentPrice - avgBuy) * quantity; }
    double pnlPct() const { return avgBuy > 0 ? ((currentPrice - avgBuy) / avgBuy) * 100.0 : 0.0; }
};

struct TradeRecord
{
    int id;
    int userId;
    QString symbol;
    QString type;
    int quantity;
    double price;
    QString timestamp;

    double total() const { return quantity * price; }
};