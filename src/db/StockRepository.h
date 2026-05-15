#pragma once

#include "../models/Models.h"
#include <QList>
#include <QString>

class StockRepository
{
public:
    static bool seedStocks();
    static QList<Stock> getAllStocks();
    static bool updatePrice(const QString &symbol, double newPrice);
    static bool stocksExist();
};