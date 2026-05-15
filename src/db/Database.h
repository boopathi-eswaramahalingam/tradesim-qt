#pragma once

#include <QString>
#include <QtSql/QSqlDatabase>

class Database
{
public:
    static Database &instance();

    bool open();
    void close();
    bool isOpen() const;
    QSqlDatabase &db();

private:
    Database() = default;
    ~Database();
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    QSqlDatabase m_db;
};