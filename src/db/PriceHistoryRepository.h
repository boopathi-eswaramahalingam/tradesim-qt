#pragma once

#include "../models/Models.h"
#include <QList>
#include <QString>
#include <QDateTime>

struct OhlcCandle
{
    QDateTime timestamp;
    double open;
    double high;
    double low;
    double close;
};

class PriceHistoryRepository
{
public:
    static bool insertTick(const QString &symbol, double price);
    static QList<OhlcCandle> getCandles(const QString &symbol, int intervalSecs, int limit = 50);
    static QList<double> getTicks(const QString &symbol, int limitSeconds);
    static double getLastPrice(const QString &symbol);
};