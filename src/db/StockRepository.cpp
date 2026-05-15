#include "StockRepository.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

bool StockRepository::stocksExist()
{
    QSqlQuery query("SELECT COUNT(*) FROM stocks");
    if (query.next())
        return query.value(0).toInt() > 0;
    return false;
}

bool StockRepository::seedStocks()
{
    if (stocksExist())
        return true;

    qDebug() << "Seeding stock market data...";

    struct SeedStock
    {
        const char *symbol;
        const char *name;
        double price;
    };

    static const SeedStock seeds[] = {
        {"TCS", "Tata Consultancy Services", 3800.0},
        {"INFY", "Infosys Limited", 1500.0},
        {"WIPRO", "Wipro Limited", 480.0},
        {"HDFCB", "HDFC Bank Limited", 1650.0},
        {"RELIANCE", "Reliance Industries", 2900.0},
        {"AAPL", "Apple Inc.", 180.0},
        {"TSLA", "Tesla Inc", 175.0},
        {"GOOGL", "Alphabet Inc.", 140.0},
        {"MSFT", "Microsoft Corporation", 410.0},
        {"NVDA", "NVIDIA Corporation", 880.0},
    };

    QSqlQuery query;
    query.prepare("INSERT INTO stocks (symbol, name, price) VALUES (?,?,?)");

    for (const auto &s : seeds)
    {
        query.addBindValue(s.symbol);
        query.addBindValue(s.name);
        query.addBindValue(s.price);
        if (!query.exec())
        {
            qWarning() << "seedStocks failed:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

QList<Stock> StockRepository::getAllStocks()
{
    QList<Stock> stocks;
    QSqlQuery query("SELECT id, symbol, name, price FROM stocks");

    while (query.next())
    {
        Stock s;
        s.id = query.value(0).toInt();
        s.symbol = query.value(1).toString();
        s.name = query.value(2).toString();
        s.price = query.value(3).toDouble();
        s.changePercent = 0.0;
        stocks.append(s);
    }
    return stocks;
}

bool StockRepository::updatePrice(const QString &symbol, double newPrice)
{
    QSqlQuery query;
    query.prepare("UPDATE stocks SET price = ? WHERE symbol =?");
    query.addBindValue(newPrice);
    query.addBindValue(symbol);

    if (!query.exec())
    {
        qWarning() << "updatePrice failed:" << query.lastError().text();
        return false;
    }
    return true;
}