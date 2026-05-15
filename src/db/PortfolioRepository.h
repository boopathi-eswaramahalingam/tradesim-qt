#pragma once

#include "../models/Models.h"
#include <QList>
#include <QString>

class PortfolioRepository
{
public:
    static bool addPosition(int userId, const QString &symbol, int quantity, double avgBuy);
    static bool updatePosition(int userId, const QString &symbol, int quantity, double avgBuy);
    static bool removePosition(int userId, const QString &symbol);
    static QList<PortfolioEntry> getPortfolio(int userId);
    static std::optional<PortfolioEntry> getPosition(int userId, const QString &symbol);
};