#include "PriceSimulator.h"

#include "../db/StockRepository.h"
#include "../db/PriceHistoryRepository.h"

#include <QDebug>
#include <QRandomGenerator>

PriceSimulator::PriceSimulator(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &PriceSimulator::onTick);
}

void PriceSimulator::start(int intervalMs)
{
    m_stocks = StockRepository::getAllStocks();

    // restore last known prices from history
    for (auto &stock : m_stocks)
    {
        double last = PriceHistoryRepository::getLastPrice(stock.symbol);
        if (last > 0.0)
            stock.price = last;
    }

    qDebug() << "Price simulator starting, interval:" << intervalMs << "ms";
    m_timer.start(intervalMs);
}

void PriceSimulator::stop()
{
    m_timer.stop();
}

void PriceSimulator::onTick()
{
    for (auto &stock : m_stocks)
    {
        double oldPrice = stock.price;
        double newPrice = randomChange(oldPrice);
        stock.changePercent = ((newPrice - oldPrice) / oldPrice) * 100.0;
        stock.price = newPrice;
        StockRepository::updatePrice(stock.symbol, newPrice);
        PriceHistoryRepository::insertTick(stock.symbol, newPrice);
    }

    qDebug() << "Tick - prices updated for" << m_stocks.size() << "stocks";
    emit pricesUpdated(m_stocks);
}

double PriceSimulator::randomChange(double price)
{
    double change = QRandomGenerator::global()->bounded(200) / 100.0 - 1.0;
    double newPrice = price * (1.0 + change / 100.0);
    return qMax(newPrice, 1.0);
}