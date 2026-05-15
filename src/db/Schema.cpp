#include "Schema.h"

bool Schema::createTables()
{
    if (!createUsersTable())
        return false;
    if (!createStocksTable())
        return false;
    if (!createPortfolioTable())
        return false;
    if (!createTradeHistoryTable())
        return false;

    qDebug() << "All tables created successfully.";
    return true;
}

bool Schema::createUsersTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "balance REAL  NOT NULL DEFAULT 100000.0"
        ")");
}

bool Schema::createStocksTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS stocks ("
        "id     INTEGER PRIMARY KEY AUTOINCREMENT,"
        "symbol TEXT  NOT NULL UNIQUE,"
        "name   TEXT  NOT NULL,"
        "price  REAL  NOT NULL"
        ")");
}

bool Schema::createPortfolioTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE   TABLE IF NOT EXISTS portfolio ("
        "id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id  INTEGER NOT NULL,"
        "symbol   TEXT    NOT NULL,"
        "quantity INTEGER NOT NULL,"
        "avg_buy  REAL    NOT NULL,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ")");
}

bool Schema::createTradeHistoryTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS trade_history ("
        "id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id      INTEGER NOT NULL,"
        "symbol       TEXT    NOT NULL,"
        "type         TEXT    NOT NULL,"
        "quantity     INTEGER NOT NULL,"
        "price        REAL    NOT NULL,"
        "timestamp    TEXT    NOT NULL,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ")");
}