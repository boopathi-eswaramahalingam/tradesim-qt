#pragma once

#include "../models/Models.h"
#include <QList>
#include <QString>

class TradeRepository
{
public:
    static bool recordTrade(int userId, const QString &symbol, const QString &type, int quantity, double price);
    static QList<TradeRecord> getHistory(int userId);
};