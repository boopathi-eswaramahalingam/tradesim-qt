#include "Database.h"

#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QStandardPaths>

Database &Database::instance()
{
    static Database instance;
    return instance;
}

Database::~Database()
{
    close();
}

bool Database::open()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_WASM
    m_db.setDatabaseName("tradesim.db");
#elif defined(Q_OS_ANDROID)
    QString dbPath = QStandardPaths::writableLocation(
                         QStandardPaths::AppDataLocation) +
                     "/tradesim.db";
    m_db.setDatabaseName(dbPath);
#else
    m_db.setDatabaseName("tradesim.db");
#endif

    if (!m_db.open())
    {
        qWarning() << "Database failed to open";
        return false;
    }

    qDebug() << "Database opened successfully at:" << m_db.databaseName();
    return true;
}

void Database::close()
{
    if (m_db.isOpen())
        m_db.close();
}

bool Database::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase &Database::db()
{
    return m_db;
}