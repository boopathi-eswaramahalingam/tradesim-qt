#include "MainWindow.h"

#include "../db/PortfolioRepository.h"
#include "../db/TradeRepository.h"
#include "../db/StockRepository.h"
#include "../db/PriceHistoryRepository.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QCandlestickSet>

MainWindow::MainWindow(const User &user, QWidget *parent)
    : QMainWindow(parent), m_engine(user), m_simulator(new PriceSimulator(this)), m_intervalSecs(60), m_candlesMode(false)
{
    setupUi();
    m_stocks = StockRepository::getAllStocks();

    connect(m_simulator, &PriceSimulator::pricesUpdated, this, &MainWindow::onPricesUpdated);

    m_simulator->start(3000);
}

MainWindow::~MainWindow()
{
    m_simulator->stop();
}

void MainWindow::setupUi()
{
    setWindowTitle("TradeSim");

    m_balanceLabel = new QLabel(this);
    updateBalance();

    m_tabs = new QTabWidget(this);

    setupMarketTab();
    setupPortfolioTab();
    setupHistoryTab();

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addWidget(new QLabel("Welcome, " + m_engine.currentUser().username, this));
    topBar->addStretch();
    topBar->addWidget(m_balanceLabel);

    layout->addLayout(topBar);
    layout->addWidget(m_tabs);

    setCentralWidget(central);
    resize(900, 700);
}

void MainWindow::setupMarketTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    m_marketTable = new QTableWidget(0, 6, tab);
    m_marketTable->setHorizontalHeaderLabels({"Symbol", "Company", "Price (Rs.)", "Change %", "Buy", "Sell"});
    m_marketTable->verticalHeader()->setVisible(false);
    m_marketTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_marketTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_marketTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_marketTable, &QTableWidget::cellClicked, this, &MainWindow::onStockSelected);

    setupChart();

    layout->addWidget(m_marketTable);
    layout->addWidget(m_chartView->parentWidget());

    m_tabs->addTab(tab, "Market");
}

void MainWindow::setupChart()
{
    // Chart type buttons
    QPushButton *lineBtn = new QPushButton("Line");
    QPushButton *candleBtn = new QPushButton("Candle");
    lineBtn->setCheckable(true);
    candleBtn->setCheckable(true);
    lineBtn->setChecked(true);

    m_chartTypeGroup = new QButtonGroup(this);
    m_chartTypeGroup->addButton(lineBtn, 0);
    m_chartTypeGroup->addButton(candleBtn, 1);
    m_chartTypeGroup->setExclusive(true);

    // Interval buttons
    QPushButton *btn1m = new QPushButton("1m");
    QPushButton *btn5m = new QPushButton("5m");
    QPushButton *btn30m = new QPushButton("30m");
    QPushButton *btn1h = new QPushButton("1h");
    btn1m->setCheckable(true);
    btn5m->setCheckable(true);
    btn30m->setCheckable(true);
    btn1h->setCheckable(true);
    btn1m->setChecked(true);

    m_intervalGroup = new QButtonGroup(this);
    m_intervalGroup->addButton(btn1m, 60);
    m_intervalGroup->addButton(btn5m, 300);
    m_intervalGroup->addButton(btn30m, 1800);
    m_intervalGroup->addButton(btn1h, 3600);
    m_intervalGroup->setExclusive(true);

    QHBoxLayout *controls = new QHBoxLayout;
    controls->addWidget(lineBtn);
    controls->addWidget(candleBtn);
    controls->addStretch();
    controls->addWidget(btn1m);
    controls->addWidget(btn5m);
    controls->addWidget(btn30m);
    controls->addWidget(btn1h);

    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(250);

    QWidget *chartWidget = new QWidget(this);
    QVBoxLayout *chartLayout = new QVBoxLayout(chartWidget);
    chartLayout->addLayout(controls);
    chartLayout->addWidget(m_chartView);

    connect(m_chartTypeGroup, &QButtonGroup::idClicked, this, &MainWindow::onChartTypeChanged);
    connect(m_intervalGroup, &QButtonGroup::idClicked, this, &MainWindow::onIntervalChanged);
}

void MainWindow::setupPortfolioTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    m_portfolioTable = new QTableWidget(0, 6, tab);
    m_portfolioTable->setHorizontalHeaderLabels({"Symbol", "Qty", "Avg Buy (Rs.)", "Current (Rs.)", "Value (Rs.)", "P&L (Rs.)"});
    m_portfolioTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_portfolioTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_portfolioTable);
    m_tabs->addTab(tab, "Portfolio");
}

void MainWindow::setupHistoryTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    m_historyTable = new QTableWidget(0, 6, tab);
    m_historyTable->setHorizontalHeaderLabels({"Time", "Symbol", "Type", "Qty", "Price (Rs.)", "Total (Rs.)"});
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_historyTable);
    m_tabs->addTab(tab, "History");
}

void MainWindow::onChartTypeChanged(int id)
{
    m_candlesMode = (id == 1);
    refreshChart();
}

void MainWindow::onIntervalChanged(int id)
{
    m_intervalSecs = id;
    refreshChart();
}

void MainWindow::refreshChart()
{
    if (m_selectedSymbol.isEmpty())
        return;

    QChart *chart = new QChart();
    chart->setTitle(m_selectedSymbol);
    chart->legend()->hide();

    if (m_candlesMode)
    {
        auto candles = PriceHistoryRepository::getCandles(
            m_selectedSymbol, m_intervalSecs, 50);

        QCandlestickSeries *series = new QCandlestickSeries();
        series->setIncreasingColor(QColor(Qt::darkGreen));
        series->setDecreasingColor(QColor(Qt::red));

        for (const auto &c : candles)
        {
            auto *set = new QCandlestickSet(
                c.open, c.high, c.low, c.close, c.timestamp.toMSecsSinceEpoch());
            series->append(set);
        }
        chart->addSeries(series);
        chart->createDefaultAxes();
    }
    else
    {
        auto ticks = PriceHistoryRepository::getTicks(
            m_selectedSymbol, m_intervalSecs);

        QLineSeries *series = new QLineSeries();
        for (int i = 0; i < ticks.size(); ++i)
            series->append(i, ticks[i]);

        chart->addSeries(series);
        chart->createDefaultAxes();
    }
    m_chartView->setChart(chart);
}

void MainWindow::onPricesUpdated(const QList<Stock> &stocks)
{
    m_stocks = stocks;
    m_marketTable->setRowCount(stocks.size());

    for (int i = 0; i < stocks.size(); ++i)
    {
        const Stock &s = stocks[i];

        m_marketTable->setItem(i, 0, new QTableWidgetItem(s.symbol));
        m_marketTable->setItem(i, 1, new QTableWidgetItem(s.name));
        m_marketTable->setItem(i, 2, new QTableWidgetItem("Rs." + QString::number(s.price, 'f', 2)));

        QTableWidgetItem *changeItem = new QTableWidgetItem(
            QString::number(s.changePercent, 'f', 2) + "%");
        changeItem->setForeground(s.changePercent >= 0 ? Qt::darkGreen : Qt::red);
        m_marketTable->setItem(i, 3, changeItem);

        QPushButton *buyBtn = new QPushButton("Buy");
        QPushButton *sellBtn = new QPushButton("Sell");

        m_marketTable->setCellWidget(i, 4, buyBtn);
        m_marketTable->setCellWidget(i, 5, sellBtn);

        connect(buyBtn, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
        connect(sellBtn, &QPushButton::clicked, this, &MainWindow::onSellClicked);
    }

    refreshPortfolio();
}

void MainWindow::onStockSelected(int row, int column)
{
    Q_UNUSED(column)
    if (row < 0 || row >= m_stocks.size())
        return;
    m_selectedSymbol = m_stocks[row].symbol;
    refreshChart();
}

void MainWindow::onBuyClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;

    for (int i = 0; i < m_marketTable->rowCount(); ++i)
    {
        if (m_marketTable->cellWidget(i, 4) == btn)
        {
            m_selectedSymbol = m_stocks[i].symbol;
            break;
        }
    }

    bool ok;
    int qty = QInputDialog::getInt(this, "Buy" + m_selectedSymbol, "Quantity:", 1, 1, 10000, 1, &ok);

    if (!ok)
        return;

    double price = 0.0;
    for (const auto &s : m_stocks)
        if (s.symbol == m_selectedSymbol)
        {
            price = s.price;
            break;
        }

    if (m_engine.buy(m_selectedSymbol, qty, price))
    {
        updateBalance();
        refreshPortfolio();
        refreshHistory();
        QMessageBox::information(this, "Success", QString("Bought %1 shares at Rs.%2. Total: Rs.%3").arg(qty).arg(price, 0, 'f', 2).arg(qty * price, 0, 'f', 2));
    }
    else
    {
        QMessageBox::warning(this, "Failed", "Insufficient balance.");
    }
}

void MainWindow::onSellClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;

    for (int i = 0; i < m_marketTable->rowCount(); ++i)
    {
        if (m_marketTable->cellWidget(i, 5) == btn)
        {
            m_selectedSymbol = m_stocks[i].symbol;
            break;
        }
    }

    bool ok;
    int qty = QInputDialog::getInt(this, "Sell" + m_selectedSymbol, "Quantity:", 1, 1, 10000, 1, &ok);

    if (!ok)
        return;

    double price = 0.0;
    for (const auto &s : m_stocks)
        if (s.symbol == m_selectedSymbol)
        {
            price = s.price;
            break;
        }

    if (m_engine.sell(m_selectedSymbol, qty, price))
    {
        updateBalance();
        refreshPortfolio();
        refreshHistory();
        QMessageBox::information(this, "Success", QString("Sold %1 shares at Rs.%2. Total: Rs.%3").arg(qty).arg(price, 0, 'f', 2).arg(qty * price, 0, 'f', 2));
    }
    else
    {
        QMessageBox::warning(this, "Failed", "Insufficient shares.");
    }
}

void MainWindow::refreshPortfolio()
{
    auto entries = PortfolioRepository::getPortfolio(m_engine.currentUser().id);

    for (auto &e : entries)
    {
        for (const auto &s : m_stocks)
            if (s.symbol == e.symbol)
            {
                e.currentPrice = s.price;
                break;
            }
    }

    m_portfolioTable->setRowCount(entries.size());

    for (int i = 0; i < entries.size(); ++i)
    {
        const auto &e = entries[i];
        m_portfolioTable->setItem(i, 0, new QTableWidgetItem(e.symbol));
        m_portfolioTable->setItem(i, 1, new QTableWidgetItem(QString::number(e.quantity)));
        m_portfolioTable->setItem(i, 2, new QTableWidgetItem("Rs." + QString::number(e.avgBuy, 'f', 2)));
        m_portfolioTable->setItem(i, 3, new QTableWidgetItem("Rs." + QString::number(e.currentPrice, 'f', 2)));
        m_portfolioTable->setItem(i, 4, new QTableWidgetItem("Rs." + QString::number(e.value(), 'f', 2)));

        QTableWidgetItem *pnlItem = new QTableWidgetItem(
            QString("%1 (%2%)").arg(e.pnl(), 0, 'f', 2).arg(e.pnlPct(), 0, 'f', 2));

        pnlItem->setForeground(e.pnl() >= 0 ? Qt::darkGreen : Qt::red);
        m_portfolioTable->setItem(i, 5, pnlItem);
    }
}

void MainWindow::refreshHistory()
{
    auto records = TradeRepository::getHistory(m_engine.currentUser().id);
    m_historyTable->setRowCount(records.size());

    for (int i = 0; i < records.size(); ++i)
    {
        const auto &r = records[i];
        m_historyTable->setItem(i, 0, new QTableWidgetItem(r.timestamp));
        m_historyTable->setItem(i, 1, new QTableWidgetItem(r.symbol));

        QTableWidgetItem *typeItem = new QTableWidgetItem(r.type);
        typeItem->setForeground(r.type == "BUY" ? Qt::darkGreen : Qt::red);
        m_historyTable->setItem(i, 2, typeItem);

        m_historyTable->setItem(i, 3, new QTableWidgetItem(QString::number(r.quantity)));
        m_historyTable->setItem(i, 4, new QTableWidgetItem("Rs." + QString::number(r.price, 'f', 2)));
        m_historyTable->setItem(i, 5, new QTableWidgetItem("Rs." + QString::number(r.total(), 'f', 2)));
    }
}

void MainWindow::updateBalance()
{
    m_balanceLabel->setText(
        "Balance: Rs." + QString::number(m_engine.currentUser().balance, 'f', 2));
}