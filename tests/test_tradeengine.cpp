#include <QtTest/QtTest>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "db/Database.h"
#include "db/Schema.h"
#include "db/UserRepository.h"
#include "db/StockRepository.h"
#include "db/PortfolioRepository.h"
#include "db/TradeRepository.h"

#include "engine/TradeEngine.h"

class TestTradeEngine : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Use in memory SQLite for tests
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");
        QVERIFY(db.open());
        QVERIFY(Schema::createTables());
        QVERIFY(StockRepository::seedStocks());
        QVERIFY(UserRepository::createUser("testuser", "testpass"));
    }

    void test_buy_success()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        bool result = engine.buy("AAPL", 1, 180.0);
        QVERIFY(result);
        QCOMPARE(engine.currentUser().balance, 100000.0 - 180.0);
    }

    void test_buy_insufficient_balance()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        bool result = engine.buy("AAPL", 1000, 99999.0);
        QVERIFY(!result);
    }

    void test_sell_success()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        engine.buy("TCS", 2, 100.0);
        bool result = engine.sell("TCS", 1, 110.0);
        QVERIFY(result);
    }

    void test_sell_insufficient_shares()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        bool result = engine.sell("NVDA", 100, 500.0);
        QVERIFY(!result);
    }

    void test_portfolio_after_buy()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        engine.buy("INFY", 5, 200.0);

        auto position = PortfolioRepository::getPosition(user->id, "INFY");
        QVERIFY(position.has_value());
        QCOMPARE(position->quantity, 5);
    }

    void test_history_recorded()
    {
        auto user = UserRepository::findByUsername("testuser");
        QVERIFY(user.has_value());

        TradeEngine engine(*user);
        engine.buy("WIPRO", 3, 150.0);

        auto history = TradeRepository::getHistory(user->id);
        QVERIFY(!history.isEmpty());
    }

    void cleanupTestCase()
    {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
};

QTEST_MAIN(TestTradeEngine)
#include "test_tradeengine.moc"