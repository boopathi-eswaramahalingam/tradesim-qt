#include <QtTest/QtTest>
#include "models/Models.h"

class TestModels : public QObject
{
    Q_OBJECT

private slots:
    void test_portfolioEntry_value()
    {
        PortfolioEntry e;
        e.quantity = 10;
        e.currentPrice = 200.0;
        QCOMPARE(e.value(), 2000.0);
    }

    void test_portfolioEntry_pnl()
    {
        PortfolioEntry e;
        e.quantity = 10;
        e.avgBuy = 150.0;
        e.currentPrice = 200.0;
        QCOMPARE(e.pnl(), 500.0);
    }

    void test_portfolioEntry_pnlPct()
    {
        PortfolioEntry e;
        e.avgBuy = 100.0;
        e.currentPrice = 150.0;
        e.quantity = 1;
        QCOMPARE(e.pnlPct(), 50.0);
    }

    void test_portfolioEntry_pnl_negative()
    {
        PortfolioEntry e;
        e.quantity = 5;
        e.avgBuy = 200.0;
        e.currentPrice = 180.0;
        QCOMPARE(e.pnl(), -100.0);
    }

    void test_tradeRecord_total()
    {
        TradeRecord r;
        r.quantity = 3;
        r.price = 250.0;
        QCOMPARE(r.total(), 750.0);
    }
};

QTEST_MAIN(TestModels)
#include "test_models.moc"