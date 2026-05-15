#pragma once

#include <QtSql/QSqlQuery>
#include <QDebug>

class Schema
{
public:
    static bool createTables();

private:
    static bool createUserTable();
    static bool createStocksTable();
    static bool createPortfoliotable();
    static bool createTradeHistoryTable();
};