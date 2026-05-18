#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
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
    void onChartTypeChanged(int id);
    void onIntervalChanged(int id);

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

    // Chart
    QChartView *m_chartView;
    QButtonGroup *m_chartTypeGroup;
    QButtonGroup *m_intervalGroup;
    int m_intervalSecs;
    bool m_candlesMode;

    // State
    QList<Stock> m_stocks;
    QString m_selectedSymbol;

    void setupUi();
    void setupMarketTab();
    void setupPortfolioTab();
    void setupHistoryTab();
    void setupChart();
    void refreshPortfolio();
    void refreshHistory();
    void refreshChart();
    void updateBalance();
};