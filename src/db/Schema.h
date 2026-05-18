#pragma once

#include <QtSql/QSqlQuery>
#include <QDebug>

class Schema
{
public:
    static bool createTables();

private:
    static bool createUsersTable();
    static bool createStocksTable();
    static bool createPortfolioTable();
    static bool createTradeHistoryTable();
    static bool createPriceHistoryTable();
};