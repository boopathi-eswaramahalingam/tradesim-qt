#pragma once

#include "../models/Models.h"
#include <QObject>
#include <QTimer>
#include <QList>

class PriceSimulator : public QObject
{
    Q_OBJECT
public:
    explicit PriceSimulator(QObject *parent = nullptr);

    void start(int intervalMs = 3000);
    void stop();

signals:
    void pricesUpdated(const QList<Stock> &stocks);

private slots:
    void onTick();

private:
    QTimer m_timer;
    QList<Stock> m_stocks;

    double randomChange(double price);
};