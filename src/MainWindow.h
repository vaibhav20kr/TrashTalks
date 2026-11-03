#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

#include "Bin.h"
#include "HistoryRecord.h"
#include "BinManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onSidebarNavChanged(int row);

    // Bin Explorer & Queue Controls
    void refreshBinTable();
    void onAddBinClicked();
    void onEditBinClicked();
    void onDeleteBinClicked();
    void onSearchBinChanged(const QString& text);
    void onAddToQueueClicked();
    void onCollectQueueAllClicked();

    // Dispatch Queue Dedicated Page
    void refreshDispatchQueuePage();
    void onCollectSelectedQueueClicked();
    void onRemoveSelectedQueueClicked();

    // Waste Game
    void onGameAnswerClicked(const QString& selectedType);
    void resetGameItem();

    // History
    void refreshHistoryTable();
    void onAddHistoryClicked();
    void onDeleteHistoryClicked();

    // Simulation
    void onSimulateTimeClicked();

private:
    BinManager manager;

    // Navigation & Stacks
    QListWidget *sidebarList;
    QStackedWidget *mainStack;

    // Page 1: Dashboard
    QLabel *lblTotalBins;
    QLabel *lblActiveBins;
    QLabel *lblCriticalBins;
    QLabel *lblAvgFill;
    QLabel *lblTotalWaste;
    QLabel *lblRecycleRate;
    QTableWidget *tblPriorityDispatch;
    QChartView *chartViewDistribution;

    // Page 2: Bin Explorer
    QLineEdit *txtSearchBin;
    QComboBox *cmbFilterType;
    QComboBox *cmbFilterStatus;
    QTableWidget *tblBins;

    // Page 3: Dedicated Dispatch Queue Page
    QLabel *lblQueueTotalCount;
    QLabel *lblQueueTotalVolume;
    QLabel *lblQueueCriticalCount;
    QLineEdit *txtSearchQueue;
    QTableWidget *tblDispatchQueue;

    // Page 4: Analytics
    QChartView *chartViewAnalyticsBar;
    QChartView *chartViewAnalyticsPie;
    QTableWidget *tblAnalyticsSummary;

    // Page 5: Waste Game
    QLabel *lblGameItem;
    QLabel *lblGameScore;
    QLabel *lblGameStreak;
    QLabel *lblGameFeedback;
    QString currentWasteItem;
    QString currentWasteCategory;
    int gameScore = 0;
    int gameStreak = 0;

    // Page 6: Collection History
    QTableWidget *tblHistory;
    QLineEdit *txtHistorySearch;

    void setupUI();
    void setupSidebar();
    void applyStyleSheet();
    void refreshAllViews();

    QWidget* createDashboardPage();
    QWidget* createBinExplorerPage();
    QWidget* createDispatchQueuePage();
    QWidget* createAnalyticsPage();
    QWidget* createGamePage();
    QWidget* createHistoryPage();

    void refreshDashboard();
    void refreshAnalytics();
    void updateWasteChart();
};

#endif // MAINWINDOW_H