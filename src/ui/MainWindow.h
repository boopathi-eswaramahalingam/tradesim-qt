#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

#include "../models/Models.h"
#include "../engine/TradeEngine.h"
#include "../engine/PriceSimulator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const User &user, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPricesUpdated(const QList<Stock> &stocks);
    void onStockSelected(int row, int column);
    void onBuyClicked();
    void onSellClicked();

private:
    // Core
    TradeEngine m_engine;
    PriceSimulator *m_simulator;

    // Widgets
    QTabWidget *m_tabs;
    QTableWidget *m_marketTable;
    QTableWidget *m_portfolioTable;
    QTableWidget *m_historyTable;
    QLabel *m_balanceLabel;
    QLabel *m_chartLabel;

    // State
    QList<Stock> m_stocks;
    QString m_selectedSymbol;

    void setupUi();
    void setupMarketTab();
    void setupPortfolioTab();
    void setupHistoryTab();
    void refreshPortfolio();
    void refreshHistory();
    void updateBalance();
};