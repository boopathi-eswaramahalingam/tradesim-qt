#include "TradeRepository.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDateTime>

bool TradeRepository::recordTrade(int userId, const QString &symbol, const QString &type, int quantity, double price)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO trade_history (user_id, symbol,type,quantity,price,timestamp) "
        "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(userId);
    query.addBindValue(symbol);
    query.addBindValue(type);
    query.addBindValue(quantity);
    query.addBindValue(price);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec())
    {
        qWarning() << "recordTrade failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<TradeRecord> TradeRepository::getHistory(int userId)
{
    QList<TradeRecord> records;
    QSqlQuery query;
    query.prepare(
        "SELECT id, user_id, symbol, type, quantity, price, timestamp "
        "FROM trade_history WHERE user_id = ? ORDER BY id DESC");
    query.addBindValue(userId);

    if (!query.exec())
        return records;

    while (query.next())
    {
        TradeRecord r;
        r.id = query.value(0).toInt();
        r.userId = query.value(1).toInt();
        r.symbol = query.value(2).toString();
        r.type = query.value(3).toString();
        r.quantity = query.value(4).toInt();
        r.price = query.value(5).toDouble();
        r.timestamp = query.value(6).toString();
        records.append(r);
    }
    return records;
}