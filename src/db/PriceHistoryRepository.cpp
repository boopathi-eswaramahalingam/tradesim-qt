#include "PriceHistoryRepository.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDateTime>

bool PriceHistoryRepository::insertTick(const QString &symbol, double price)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO price_history (symbol,price,timestamp) VALUES (?,?,?)");
    query.addBindValue(symbol);
    query.addBindValue(price);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec())
    {
        qWarning() << "insertTick failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<OhlcCandle> PriceHistoryRepository::getCandles(
    const QString &symbol, int intervalSecs, int limit)
{
    QList<OhlcCandle> candles;

    QSqlQuery query;
    query.prepare(
        "SELECT price, timestamp FROM price_history "
        "WHERE symbol = ? ORDER BY timestamp ASC");
    query.addBindValue(symbol);

    if (!query.exec())
        return candles;

    struct RawTick
    {
        double price;
        QDateTime time;
    };
    QList<RawTick> ticks;

    while (query.next())
    {
        RawTick t;
        t.price = query.value(0).toDouble();
        t.time = QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
        ticks.append(t);
    }

    if (ticks.isEmpty())
        return candles;

    QDateTime bucketStart = ticks.first().time;
    double open = ticks.first().price;
    double high = ticks.first().price;
    double low = ticks.first().price;
    double close = ticks.first().price;

    for (int i = 1; i < ticks.size(); ++i)
    {
        const auto &t = ticks[i];
        qint64 diff = bucketStart.secsTo(t.time);

        if (diff >= intervalSecs)
        {
            OhlcCandle c;
            c.timestamp = bucketStart;
            c.open = open;
            c.high = high;
            c.low = low;
            c.close = close;
            candles.append(c);

            bucketStart = t.time;
            open = t.price;
            high = t.price;
            low = t.price;
            close = t.price;
        }
        else
        {
            high = qMax(high, t.price);
            low = qMin(low, t.price);
            close = t.price;
        }
    }

    OhlcCandle c;
    c.timestamp = bucketStart;
    c.open = open;
    c.high = high;
    c.low = low;
    c.close = close;
    candles.append(c);

    if (candles.size() > limit)
        candles = candles.mid(candles.size() - limit);

    return candles;
}

QList<double> PriceHistoryRepository::getTicks(
    const QString &symbol, int limitSeconds)
{
    QList<double> prices;
    QString since = QDateTime::currentDateTime().addSecs(-limitSeconds).toString(Qt::ISODate);

    QSqlQuery query;
    query.prepare(
        "SELECT price FROM price_history "
        "WHERE symbol = ? AND timestamp >= ? ORDER BY timestamp ASC");

    query.addBindValue(symbol);
    query.addBindValue(since);

    if (!query.exec())
        return prices;

    while (query.next())
        prices.append(query.value(0).toDouble());

    return prices;
}

double PriceHistoryRepository::getLastPrice(const QString &symbol)
{
    QSqlQuery query;
    query.prepare(
        "SELECT price FROM price_history "
        "WHERE symbol = ? ORDER BY timestamp DESC LIMIT 1");
    query.addBindValue(symbol);

    if (query.exec() && query.next())
        return query.value(0).toDouble();

    return -1.0;
}