#include "MainWindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QGroupBox>
#include <QtCharts/QLegend>
#include <random>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    applyStyleSheet();
    resetGameItem();
    refreshAllViews();
}

void MainWindow::setupUI() {
    setWindowTitle("TrashTalk — Industrial Waste Operations & Smart Dispatch");
    resize(1400, 900);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();
    mainLayout->addWidget(sidebarList);

    mainStack = new QStackedWidget(this);
    mainStack->addWidget(createDashboardPage());
    mainStack->addWidget(createBinExplorerPage());
    mainStack->addWidget(createDispatchQueuePage());
    mainStack->addWidget(createAnalyticsPage());
    mainStack->addWidget(createGamePage());
    mainStack->addWidget(createHistoryPage());

    mainLayout->addWidget(mainStack);
    setCentralWidget(centralWidget);

    connect(sidebarList, &QListWidget::currentRowChanged, this, &MainWindow::onSidebarNavChanged);
}

void MainWindow::setupSidebar() {
    sidebarList = new QListWidget(this);
    sidebarList->setFixedWidth(260);
    sidebarList->addItem("📊  DASHBOARD");
    sidebarList->addItem("🗑️  BIN EXPLORER");
    sidebarList->addItem("🚚  DISPATCH QUEUE");
    sidebarList->addItem("📈  ANALYTICS");
    sidebarList->addItem("🎮  WASTE SEGREGATION");
    sidebarList->addItem("📜  COLLECTION HISTORY");
    sidebarList->setCurrentRow(0);
}

void MainWindow::applyStyleSheet() {
    QString qss = R"(
        QMainWindow {
            background-color: #141716;
            color: #E4E8E5;
            font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, sans-serif;
        }
        QListWidget {
            background-color: #0E1110;
            border: none;
            border-right: 2px solid #2B342F;
            color: #9AA59F;
            font-size: 13px;
            font-weight: bold;
            letter-spacing: 1px;
            padding-top: 25px;
        }
        QListWidget::item {
            height: 52px;
            padding-left: 22px;
            border-left: 4px solid transparent;
        }
        QListWidget::item:selected {
            background-color: #1E2522;
            color: #F0F4F1;
            border-left: 4px solid #D96B27;
        }
        QListWidget::item:hover:!selected {
            background-color: #171C1A;
            color: #D4DAD6;
        }
        QWidget#Card {
            background-color: #1B201D;
            border: 1px solid #343E38;
            border-radius: 4px;
        }
        QWidget#DarkCard {
            background-color: #222825;
            border: 1px solid #3D4A43;
            border-radius: 4px;
        }
        QLabel {
            color: #E4E8E5;
        }
        QPushButton {
            background-color: #4E5D42;
            color: #F0F4F1;
            font-weight: bold;
            border: 1px solid #5C6E4E;
            border-radius: 3px;
            padding: 9px 16px;
            font-size: 12px;
            letter-spacing: 0.5px;
        }
        QPushButton:hover {
            background-color: #5C6E4E;
            border-color: #6C825B;
        }
        QPushButton:pressed {
            background-color: #3C4833;
        }
        QPushButton#HazardBtn {
            background-color: #D96B27;
            color: #141716;
            border: 1px solid #E87A36;
            font-weight: 800;
        }
        QPushButton#HazardBtn:hover {
            background-color: #E87A36;
        }
        QPushButton#HazardBtn:pressed {
            background-color: #B5541A;
        }
        QPushButton#DangerBtn {
            background-color: #8C3A35;
            color: #F0F4F1;
            border: 1px solid #A1443E;
        }
        QPushButton#DangerBtn:hover {
            background-color: #A1443E;
        }
        QLineEdit, QComboBox {
            background-color: #171C1A;
            border: 1px solid #343E38;
            border-radius: 3px;
            color: #E4E8E5;
            padding: 8px 12px;
            font-size: 13px;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 1px solid #D96B27;
        }
        QComboBox QAbstractItemView {
            background-color: #1B201D;
            color: #E4E8E5;
            selection-background-color: #4E5D42;
            border: 1px solid #343E38;
        }
        QTableWidget {
            background-color: #1B201D;
            border: 1px solid #343E38;
            gridline-color: #262E2A;
            color: #E4E8E5;
            font-size: 13px;
            border-radius: 4px;
            selection-background-color: #2E3A34;
            selection-color: #FFFFFF;
        }
        QHeaderView::section {
            background-color: #141716;
            color: #D96B27;
            padding: 8px;
            font-weight: bold;
            font-size: 11px;
            letter-spacing: 0.5px;
            border: none;
            border-right: 1px solid #2B342F;
            border-bottom: 2px solid #343E38;
        }
        QMessageBox, QDialog {
            background-color: #1B201D;
            color: #E4E8E5;
        }
    )";
    setStyleSheet(qss);
}

void MainWindow::onSidebarNavChanged(int row) {
    mainStack->setCurrentIndex(row);
}

void MainWindow::refreshAllViews() {
    refreshDashboard();
    refreshBinTable();
    refreshDispatchQueuePage();
    refreshAnalytics();
    refreshHistoryTable();
}

// ---------------- DASHBOARD SCREEN ----------------
QWidget* MainWindow::createDashboardPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *titleBox = new QVBoxLayout();

    QLabel *lblTitle = new QLabel("THE STATE OF OUR ENVIRONMENT.");
    lblTitle->setStyleSheet("font-size: 26px; font-weight: 800; color: #E4E8E5; letter-spacing: 1.5px;");

    QLabel *lblSub = new QLabel("Monitoring waste streams. Optimizing priority collections.");
    lblSub->setStyleSheet("font-size: 14px; color: #9AA59F; font-style: italic; font-weight: 500;");

    titleBox->addWidget(lblTitle);
    titleBox->addWidget(lblSub);

    QPushButton *btnSimulate = new QPushButton("⚡ SIMULATE TIME PASSAGE");
    btnSimulate->setCursor(Qt::PointingHandCursor);
    connect(btnSimulate, &QPushButton::clicked, this, &MainWindow::onSimulateTimeClicked);

    headerLayout->addLayout(titleBox);
    headerLayout->addStretch();
    headerLayout->addWidget(btnSimulate);
    layout->addLayout(headerLayout);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(15);

    auto createMetricCard = [](const QString& title, QLabel*& valLabel, const QString& colorHex) {
        QWidget *card = new QWidget();
        card->setObjectName("Card");
        QVBoxLayout *cLayout = new QVBoxLayout(card);
        cLayout->setContentsMargins(18, 16, 18, 16);

        QLabel *lblT = new QLabel(title);
        lblT->setStyleSheet("font-size: 11px; color: #9AA59F; font-weight: bold; letter-spacing: 1px;");

        valLabel = new QLabel("0");
        valLabel->setStyleSheet(QString("font-size: 34px; font-weight: 800; color: %1; margin-top: 4px;").arg(colorHex));

        cLayout->addWidget(lblT);
        cLayout->addWidget(valLabel);
        return card;
    };

    grid->addWidget(createMetricCard("TOTAL BINS", lblTotalBins, "#E4E8E5"), 0, 0);
    grid->addWidget(createMetricCard("ACTIVE BINS", lblActiveBins, "#5C6E4E"), 0, 1);
    grid->addWidget(createMetricCard("CRITICAL BINS", lblCriticalBins, "#D96B27"), 0, 2);
    grid->addWidget(createMetricCard("AVG FILL PERCENTAGE", lblAvgFill, "#E4E8E5"), 1, 0);
    grid->addWidget(createMetricCard("WASTE COLLECTED", lblTotalWaste, "#5C6E4E"), 1, 1);
    grid->addWidget(createMetricCard("RECYCLING RATE", lblRecycleRate, "#9AA59F"), 1, 2);

    layout->addLayout(grid);

    QHBoxLayout *splitLayout = new QHBoxLayout();

    chartViewDistribution = new QChartView();
    chartViewDistribution->setRenderHint(QPainter::Antialiasing);
    chartViewDistribution->setStyleSheet("background-color: #1B201D; border: 1px solid #343E38; border-radius: 4px;");

    QWidget *dispatchCard = new QWidget();
    dispatchCard->setObjectName("DarkCard");
    QVBoxLayout *dLayout = new QVBoxLayout(dispatchCard);
    dLayout->setContentsMargins(18, 18, 18, 18);

    QHBoxLayout *dispatchHeader = new QHBoxLayout();
    QLabel *lblDTitle = new QLabel("BINARY MAX HEAP PRIORITY DISPATCH");
    lblDTitle->setStyleSheet("font-weight: bold; color: #D96B27; font-size: 12px; letter-spacing: 1px;");

    QPushButton *btnCollectQueueDash = new QPushButton("⚡ COLLECT QUEUE ALL");
    btnCollectQueueDash->setObjectName("HazardBtn");
    btnCollectQueueDash->setCursor(Qt::PointingHandCursor);
    btnCollectQueueDash->setStyleSheet("font-size: 11px; padding: 6px 12px;");
    connect(btnCollectQueueDash, &QPushButton::clicked, this, &MainWindow::onCollectQueueAllClicked);

    dispatchHeader->addWidget(lblDTitle);
    dispatchHeader->addStretch();
    dispatchHeader->addWidget(btnCollectQueueDash);

    tblPriorityDispatch = new QTableWidget(0, 4);
    tblPriorityDispatch->setHorizontalHeaderLabels({"Bin ID", "Location", "Fill %", "Priority"});
    tblPriorityDispatch->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    dLayout->addLayout(dispatchHeader);
    dLayout->addWidget(tblPriorityDispatch);

    splitLayout->addWidget(chartViewDistribution, 5);
    splitLayout->addWidget(dispatchCard, 6);

    layout->addLayout(splitLayout, 1);
    return page;
}

void MainWindow::refreshDashboard() {
    lblTotalBins->setText(QString::number(manager.getTotalBins()));
    lblActiveBins->setText(QString::number(manager.getActiveBins()));
    lblCriticalBins->setText(QString::number(manager.getCriticalBins()));
    lblAvgFill->setText(QString::number(manager.getAverageFillPercentage(), 'f', 1) + "%");
    lblTotalWaste->setText(QString::number(manager.getTotalWasteCollected(), 'f', 0) + " kg");
    lblRecycleRate->setText(QString::number(manager.getRecyclingPercentage(), 'f', 1) + "%");

    tblPriorityDispatch->setRowCount(0);
    const auto& heapNodes = manager.getMaxHeap().getElements();
    for (const auto& node : heapNodes) {
        int r = tblPriorityDispatch->rowCount();
        tblPriorityDispatch->insertRow(r);
        tblPriorityDispatch->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(node.binId)));
        tblPriorityDispatch->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(node.location)));

        QTableWidgetItem *fillItem = new QTableWidgetItem(QString::number(node.fillLevel, 'f', 1) + "%");
        if (node.fillLevel >= 80.0) {
            fillItem->setForeground(QBrush(QColor("#D96B27")));
            fillItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        }
        tblPriorityDispatch->setItem(r, 2, fillItem);
        tblPriorityDispatch->setItem(r, 3, new QTableWidgetItem(QString::number(node.priorityScore, 'f', 1)));
    }

    updateWasteChart();
}

void MainWindow::updateWasteChart() {
    auto *series = new QPieSeries();

    std::map<std::string, int> counts;
    for (const auto& b : manager.getAllBins()) {
        counts[b.type]++;
    }

    const std::vector<QColor> colors = {
        QColor("#5C6E4E"), QColor("#D96B27"), QColor("#9AA59F"),
        QColor("#3A4540"), QColor("#8C3A35"), QColor("#A39B8B")
    };

    int colorIdx = 0;
    for (const auto& pair : counts) {
        auto *slice = series->append(QString::fromStdString(pair.first), pair.second);
        slice->setBrush(QBrush(colors[colorIdx % colors.size()]));
        slice->setLabelColor(QColor("#E4E8E5"));
        colorIdx++;
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("WASTE STREAM COMPOSITION");
    chart->setTitleBrush(QBrush(QColor("#E4E8E5")));
    chart->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
    chart->setBackgroundBrush(QBrush(QColor("#1B201D")));
    if (chart->legend()) {
        chart->legend()->setLabelBrush(QBrush(QColor("#E4E8E5")));
    }

    chartViewDistribution->setChart(chart);
}

void MainWindow::onSimulateTimeClicked() {
    manager.simulateFillIncrement();
    refreshAllViews();
    QMessageBox::information(this, "Simulation", "Simulated 4 hours passage. Bin fill levels and dispatch priority recalculated.");
}

// ---------------- BIN EXPLORER ----------------
QWidget* MainWindow::createBinExplorerPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(18);

    QLabel *lblTitle = new QLabel("BIN DIRECTORY & OVERVIEW");
    lblTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #E4E8E5; letter-spacing: 1px;");
    layout->addWidget(lblTitle);

    QHBoxLayout *topBar = new QHBoxLayout();
    txtSearchBin = new QLineEdit();
    txtSearchBin->setPlaceholderText("Search Bin ID or Location...");
    connect(txtSearchBin, &QLineEdit::textChanged, this, &MainWindow::onSearchBinChanged);

    cmbFilterType = new QComboBox();
    cmbFilterType->addItems({"All Types", "Organic", "Plastic", "Paper", "E-waste", "Glass", "Metal", "General"});
    connect(cmbFilterType, &QComboBox::currentTextChanged, this, [this](const QString&){ refreshBinTable(); });

    cmbFilterStatus = new QComboBox();
    cmbFilterStatus->addItems({"All Statuses", "Active", "Critical", "Maintenance"});
    connect(cmbFilterStatus, &QComboBox::currentTextChanged, this, [this](const QString&){ refreshBinTable(); });

    QPushButton *btnAdd = new QPushButton("+ ADD BIN");
    QPushButton *btnEnqueueBin = new QPushButton("+ ENQUEUE TO DISPATCH");
    btnEnqueueBin->setCursor(Qt::PointingHandCursor);
    btnEnqueueBin->setStyleSheet("background-color: #3A4540; color: #E4E8E5; font-weight: bold;");

    QPushButton *btnCollectQueueExplorer = new QPushButton("⚡ COLLECT QUEUE ALL");
    btnCollectQueueExplorer->setObjectName("HazardBtn");
    btnCollectQueueExplorer->setCursor(Qt::PointingHandCursor);

    QPushButton *btnEdit = new QPushButton("EDIT");
    QPushButton *btnDelete = new QPushButton("DELETE");
    btnDelete->setObjectName("DangerBtn");

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddBinClicked);
    connect(btnEnqueueBin, &QPushButton::clicked, this, &MainWindow::onAddToQueueClicked);
    connect(btnCollectQueueExplorer, &QPushButton::clicked, this, &MainWindow::onCollectQueueAllClicked);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditBinClicked);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteBinClicked);

    topBar->addWidget(txtSearchBin, 3);
    topBar->addWidget(cmbFilterType, 1);
    topBar->addWidget(cmbFilterStatus, 1);
    topBar->addWidget(btnAdd);
    topBar->addWidget(btnEnqueueBin);
    topBar->addWidget(btnCollectQueueExplorer);
    topBar->addWidget(btnEdit);
    topBar->addWidget(btnDelete);

    layout->addLayout(topBar);

    tblBins = new QTableWidget(0, 7);
    tblBins->setHorizontalHeaderLabels({"Bin ID", "Location", "Type", "Capacity (L)", "Fill %", "Last Emptied", "Status"});
    tblBins->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tblBins);

    return page;
}

void MainWindow::refreshBinTable() {
    tblBins->setRowCount(0);
    std::string search = txtSearchBin->text().toStdString();
    std::string typeF = cmbFilterType->currentText().toStdString();
    if (typeF == "All Types") typeF = "All";
    std::string statusF = cmbFilterStatus->currentText().toStdString();
    if (statusF == "All Statuses") statusF = "All";

    auto filtered = manager.getFilteredBins("", typeF, statusF);

    for (const auto& b : filtered) {
        if (!search.empty() && b.id.find(search) == std::string::npos && b.location.find(search) == std::string::npos) {
            continue;
        }

        int r = tblBins->rowCount();
        tblBins->insertRow(r);
        tblBins->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(b.id)));
        tblBins->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(b.location)));
        tblBins->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(b.type)));
        tblBins->setItem(r, 3, new QTableWidgetItem(QString::number(b.capacityLiters, 'f', 0)));

        QTableWidgetItem *fillItem = new QTableWidgetItem(QString::number(b.fillLevel, 'f', 1) + "%");
        if (b.fillLevel >= 80.0) {
            fillItem->setForeground(QBrush(QColor("#D96B27")));
            fillItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        }
        tblBins->setItem(r, 4, fillItem);
        tblBins->setItem(r, 5, new QTableWidgetItem(QString::number(b.lastEmptiedHoursAgo, 'f', 0) + " hrs ago"));
        tblBins->setItem(r, 6, new QTableWidgetItem(QString::fromStdString(b.status)));
    }
}

void MainWindow::onAddBinClicked() {
    bool ok;
    QString id = QInputDialog::getText(this, "Add Bin", "Bin ID (e.g. BIN-200):", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;

    QString loc = QInputDialog::getText(this, "Add Bin", "Location:", QLineEdit::Normal, "City Square", &ok);
    if (!ok) return;

    Bin b;
    b.id = id.toStdString();
    b.location = loc.toStdString();
    b.type = "Organic";
    b.capacityLiters = 240.0;
    b.fillLevel = 10.0;
    b.lastEmptiedHoursAgo = 2.0;
    b.status = "Active";

    if (manager.addBin(b)) {
        refreshAllViews();
    } else {
        QMessageBox::warning(this, "Error", "Bin ID already exists!");
    }
}

void MainWindow::onAddToQueueClicked() {
    int r = tblBins->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Select Bin", "Please select a bin from the table to add to the dispatch queue.");
        return;
    }
    std::string binId = tblBins->item(r, 0)->text().toStdString();
    Bin b;
    if (manager.findBinById(binId, b)) {
        if (manager.enqueueBinToDispatch(binId)) {
            QMessageBox::information(this, "Bin Enqueued",
                QString("Bin %1 (%2% filled) has been added to the dispatch queue.")
                .arg(QString::fromStdString(binId), QString::number(b.fillLevel, 'f', 1)));
            refreshAllViews();
        } else {
            QMessageBox::information(this, "Already Enqueued", QString("Bin %1 is already in the dispatch queue.").arg(QString::fromStdString(binId)));
        }
    }
}

void MainWindow::onCollectQueueAllClicked() {
    int collected = manager.collectAllInQueue();
    if (collected > 0) {
        QMessageBox::information(this, "Collection Complete",
            QString("Successfully collected %1 queued bins!\n\n• All collected bins reset to 0% fill.\n• Collection history records logged automatically.")
            .arg(collected));
    } else {
        QMessageBox::information(this, "Queue Empty", "The dispatch queue is currently empty.");
    }
    refreshAllViews();
}

void MainWindow::onEditBinClicked() {
    int r = tblBins->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Select Bin", "Please select a bin to edit.");
        return;
    }
    std::string id = tblBins->item(r, 0)->text().toStdString();
    Bin b;
    if (manager.findBinById(id, b)) {
        bool ok;
        double fill = QInputDialog::getDouble(this, "Edit Fill Level", "Fill Percentage (0-100):", b.fillLevel, 0, 100, 1, &ok);
        if (ok) {
            b.fillLevel = fill;
            if (b.fillLevel >= 80.0) b.status = "Critical";
            else b.status = "Active";
            manager.updateBin(b);
            refreshAllViews();
        }
    }
}

void MainWindow::onDeleteBinClicked() {
    int r = tblBins->currentRow();
    if (r < 0) return;
    std::string id = tblBins->item(r, 0)->text().toStdString();
    if (manager.deleteBin(id)) {
        refreshAllViews();
    }
}

void MainWindow::onSearchBinChanged(const QString&) {
    refreshBinTable();
}

// ---------------- DEDICATED DISPATCH QUEUE PAGE ----------------
QWidget* MainWindow::createDispatchQueuePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(18);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *titleBox = new QVBoxLayout();

    QLabel *lblTitle = new QLabel("DISPATCH QUEUE & FLEET ROUTING");
    lblTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #E4E8E5; letter-spacing: 1px;");

    QLabel *lblSub = new QLabel("Real-time collection tasks ordered by priority score and fill level.");
    lblSub->setStyleSheet("font-size: 13px; color: #9AA59F; font-style: italic;");

    titleBox->addWidget(lblTitle);
    titleBox->addWidget(lblSub);

    QPushButton *btnSimulate = new QPushButton("⚡ SIMULATE TIME PASSAGE");
    btnSimulate->setCursor(Qt::PointingHandCursor);
    connect(btnSimulate, &QPushButton::clicked, this, &MainWindow::onSimulateTimeClicked);

    headerLayout->addLayout(titleBox);
    headerLayout->addStretch();
    headerLayout->addWidget(btnSimulate);
    layout->addLayout(headerLayout);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(15);

    auto createMetricCard = [](const QString& title, QLabel*& valLabel, const QString& colorHex) {
        QWidget *card = new QWidget();
        card->setObjectName("Card");
        QVBoxLayout *cLayout = new QVBoxLayout(card);
        cLayout->setContentsMargins(18, 14, 18, 14);

        QLabel *lblT = new QLabel(title);
        lblT->setStyleSheet("font-size: 11px; color: #9AA59F; font-weight: bold; letter-spacing: 1px;");

        valLabel = new QLabel("0");
        valLabel->setStyleSheet(QString("font-size: 28px; font-weight: 800; color: %1; margin-top: 2px;").arg(colorHex));

        cLayout->addWidget(lblT);
        cLayout->addWidget(valLabel);
        return card;
    };

    grid->addWidget(createMetricCard("QUEUED TASKS", lblQueueTotalCount, "#D96B27"), 0, 0);
    grid->addWidget(createMetricCard("ESTIMATED QUEUED VOLUME", lblQueueTotalVolume, "#5C6E4E"), 0, 1);
    grid->addWidget(createMetricCard("CRITICAL OVERFLOW RISK BINS", lblQueueCriticalCount, "#8C3A35"), 0, 2);

    layout->addLayout(grid);

    QHBoxLayout *topBar = new QHBoxLayout();
    txtSearchQueue = new QLineEdit();
    txtSearchQueue->setPlaceholderText("Search Queued Bin ID or Location...");
    connect(txtSearchQueue, &QLineEdit::textChanged, this, [this](const QString&){ refreshDispatchQueuePage(); });

    QPushButton *btnCollectAll = new QPushButton("⚡ COLLECT QUEUE ALL");
    btnCollectAll->setObjectName("HazardBtn");
    btnCollectAll->setCursor(Qt::PointingHandCursor);

    QPushButton *btnCollectSelected = new QPushButton("✓ COLLECT SELECTED");
    btnCollectSelected->setCursor(Qt::PointingHandCursor);

    QPushButton *btnRemoveSelected = new QPushButton("✕ REMOVE FROM QUEUE");
    btnRemoveSelected->setObjectName("DangerBtn");
    btnRemoveSelected->setCursor(Qt::PointingHandCursor);

    connect(btnCollectAll, &QPushButton::clicked, this, &MainWindow::onCollectQueueAllClicked);
    connect(btnCollectSelected, &QPushButton::clicked, this, &MainWindow::onCollectSelectedQueueClicked);
    connect(btnRemoveSelected, &QPushButton::clicked, this, &MainWindow::onRemoveSelectedQueueClicked);

    topBar->addWidget(txtSearchQueue, 3);
    topBar->addWidget(btnCollectAll);
    topBar->addWidget(btnCollectSelected);
    topBar->addWidget(btnRemoveSelected);

    layout->addLayout(topBar);

    tblDispatchQueue = new QTableWidget(0, 9);
    tblDispatchQueue->setHorizontalHeaderLabels({
        "Queue Order", "Bin ID", "Location", "Waste Type",
        "Capacity (L)", "Fill %", "Hours Unemptied", "Priority Score", "Status"
    });
    tblDispatchQueue->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tblDispatchQueue);

    return page;
}

void MainWindow::refreshDispatchQueuePage() {
    tblDispatchQueue->setRowCount(0);
    std::string search = txtSearchQueue ? txtSearchQueue->text().toStdString() : "";

    auto queuedIds = manager.getDispatchQueue().toVector();

    int totalCount = static_cast<int>(queuedIds.size());
    double totalVolL = 0.0;
    int criticalCount = 0;

    int orderIdx = 1;
    for (const auto& id : queuedIds) {
        Bin b;
        if (manager.findBinById(id, b)) {
            totalVolL += (b.fillLevel / 100.0) * b.capacityLiters;
            if (b.fillLevel >= 80.0) criticalCount++;

            if (!search.empty() && b.id.find(search) == std::string::npos && b.location.find(search) == std::string::npos) {
                continue;
            }

            int r = tblDispatchQueue->rowCount();
            tblDispatchQueue->insertRow(r);

            tblDispatchQueue->setItem(r, 0, new QTableWidgetItem(orderIdx == 1 ? "1 (FRONT)" : QString::number(orderIdx)));
            tblDispatchQueue->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(b.id)));
            tblDispatchQueue->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(b.location)));
            tblDispatchQueue->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(b.type)));
            tblDispatchQueue->setItem(r, 4, new QTableWidgetItem(QString::number(b.capacityLiters, 'f', 0) + " L"));

            QTableWidgetItem *fillItem = new QTableWidgetItem(QString::number(b.fillLevel, 'f', 1) + "%");
            if (b.fillLevel >= 80.0) {
                fillItem->setForeground(QBrush(QColor("#D96B27")));
                fillItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
            }
            tblDispatchQueue->setItem(r, 5, fillItem);
            tblDispatchQueue->setItem(r, 6, new QTableWidgetItem(QString::number(b.lastEmptiedHoursAgo, 'f', 0) + " hrs"));
            tblDispatchQueue->setItem(r, 7, new QTableWidgetItem(QString::number(b.getPriorityScore(), 'f', 1)));
            tblDispatchQueue->setItem(r, 8, new QTableWidgetItem(QString::fromStdString(b.status)));

            orderIdx++;
        }
    }

    if (lblQueueTotalCount) lblQueueTotalCount->setText(QString::number(totalCount));
    if (lblQueueTotalVolume) lblQueueTotalVolume->setText(QString::number(totalVolL, 'f', 0) + " Liters");
    if (lblQueueCriticalCount) lblQueueCriticalCount->setText(QString::number(criticalCount));
}

void MainWindow::onCollectSelectedQueueClicked() {
    int r = tblDispatchQueue->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Select Item", "Please select a queued bin from the table to collect.");
        return;
    }
    std::string binId = tblDispatchQueue->item(r, 1)->text().toStdString();
    if (manager.collectSingleFromQueue(binId)) {
        QMessageBox::information(this, "Collection Logged",
            QString("Bin %1 has been collected.\n• Fill level reset to 0%.\n• Historical record logged.")
            .arg(QString::fromStdString(binId)));
        refreshAllViews();
    }
}

void MainWindow::onRemoveSelectedQueueClicked() {
    int r = tblDispatchQueue->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Select Item", "Please select a queued bin from the table to remove.");
        return;
    }
    std::string binId = tblDispatchQueue->item(r, 1)->text().toStdString();
    if (manager.removeBinFromDispatch(binId)) {
        refreshAllViews();
    }
}

// ---------------- ANALYTICS SCREEN ----------------
QWidget* MainWindow::createAnalyticsPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QLabel *lblTitle = new QLabel("ENVIRONMENTAL ANALYTICS & INSIGHTS");
    lblTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #E4E8E5; letter-spacing: 1px;");
    layout->addWidget(lblTitle);

    QHBoxLayout *chartsLayout = new QHBoxLayout();

    chartViewAnalyticsBar = new QChartView();
    chartViewAnalyticsBar->setRenderHint(QPainter::Antialiasing);
    chartViewAnalyticsBar->setStyleSheet("background-color: #1B201D; border: 1px solid #343E38; border-radius: 4px;");

    chartViewAnalyticsPie = new QChartView();
    chartViewAnalyticsPie->setRenderHint(QPainter::Antialiasing);
    chartViewAnalyticsPie->setStyleSheet("background-color: #1B201D; border: 1px solid #343E38; border-radius: 4px;");

    chartsLayout->addWidget(chartViewAnalyticsBar, 6);
    chartsLayout->addWidget(chartViewAnalyticsPie, 5);

    layout->addLayout(chartsLayout, 1);

    tblAnalyticsSummary = new QTableWidget(0, 4);
    tblAnalyticsSummary->setHorizontalHeaderLabels({"Metric Category", "Current Value", "Benchmark Goal", "Status Evaluation"});
    tblAnalyticsSummary->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tblAnalyticsSummary);

    return page;
}

void MainWindow::refreshAnalytics() {
    auto *barSet = new QBarSet("Fill Level (%)");
    barSet->setBrush(QBrush(QColor("#4E5D42")));

    QStringList categories;
    const auto& bins = manager.getAllBins();
    int limit = std::min(8, static_cast<int>(bins.size()));
    for (int i = 0; i < limit; ++i) {
        *barSet << bins[i].fillLevel;
        categories << QString::fromStdString(bins[i].id);
    }

    auto *seriesBar = new QBarSeries();
    seriesBar->append(barSet);

    auto *chartBar = new QChart();
    chartBar->addSeries(seriesBar);
    chartBar->setTitle("BIN CAPACITY UTILIZATION (SAMPLE BINS)");
    chartBar->setTitleBrush(QBrush(QColor("#E4E8E5")));
    chartBar->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
    chartBar->setBackgroundBrush(QBrush(QColor("#1B201D")));

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsBrush(QBrush(QColor("#E4E8E5")));
    chartBar->addAxis(axisX, Qt::AlignBottom);
    seriesBar->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setLabelsBrush(QBrush(QColor("#E4E8E5")));
    chartBar->addAxis(axisY, Qt::AlignLeft);
    seriesBar->attachAxis(axisY);

    chartViewAnalyticsBar->setChart(chartBar);

    auto *seriesPie = new QPieSeries();
    double totalKg = manager.getTotalWasteCollected();
    double recycleRate = manager.getRecyclingPercentage();
    double recyclableKg = (totalKg * recycleRate) / 100.0;
    double nonRecyclableKg = totalKg - recyclableKg;

    auto *s1 = seriesPie->append("Recyclable Stream", recyclableKg);
    auto *s2 = seriesPie->append("General Landfill", nonRecyclableKg);

    s1->setBrush(QBrush(QColor("#5C6E4E")));
    s2->setBrush(QBrush(QColor("#8C3A35")));
    s1->setLabelColor(QColor("#E4E8E5"));
    s2->setLabelColor(QColor("#E4E8E5"));

    auto *chartPie = new QChart();
    chartPie->addSeries(seriesPie);
    chartPie->setTitle("RECYCLING EFFICIENCY RATIO");
    chartPie->setTitleBrush(QBrush(QColor("#E4E8E5")));
    chartPie->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
    chartPie->setBackgroundBrush(QBrush(QColor("#1B201D")));

    chartViewAnalyticsPie->setChart(chartPie);

    tblAnalyticsSummary->setRowCount(0);
    auto addSummaryRow = [this](const QString& cat, const QString& val, const QString& goal, const QString& eval) {
        int r = tblAnalyticsSummary->rowCount();
        tblAnalyticsSummary->insertRow(r);
        tblAnalyticsSummary->setItem(r, 0, new QTableWidgetItem(cat));
        tblAnalyticsSummary->setItem(r, 1, new QTableWidgetItem(val));
        tblAnalyticsSummary->setItem(r, 2, new QTableWidgetItem(goal));
        tblAnalyticsSummary->setItem(r, 3, new QTableWidgetItem(eval));
    };

    addSummaryRow("Municipal Recycling Rate", QString::number(recycleRate, 'f', 1) + "%", "> 50.0%", recycleRate >= 50.0 ? "OPTIMAL" : "NEEDS IMPROVEMENT");
    addSummaryRow("Average Bin Fill Level", QString::number(manager.getAverageFillPercentage(), 'f', 1) + "%", "< 70.0%", manager.getAverageFillPercentage() < 70.0 ? "BALANCED" : "CRITICAL DEMAND");
    addSummaryRow("Critical Bins Overflow Risk", QString::number(manager.getCriticalBins()), "0 Bins", manager.getCriticalBins() == 0 ? "LOW RISK" : "HIGH ACTION REQUIRED");
}

// ---------------- WASTE SEGREGATION GAME ----------------
QWidget* MainWindow::createGamePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 35, 40, 35);
    layout->setSpacing(20);

    QLabel *lblTitle = new QLabel("WASTE SEGREGATION CHALLENGE");
    lblTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #E4E8E5; letter-spacing: 1px;");
    layout->addWidget(lblTitle);

    QHBoxLayout *scoreLayout = new QHBoxLayout();
    lblGameScore = new QLabel("SCORE: 0");
    lblGameScore->setStyleSheet("font-size: 18px; font-weight: 800; color: #5C6E4E;");
    lblGameStreak = new QLabel("STREAK: 0 🔥");
    lblGameStreak->setStyleSheet("font-size: 18px; font-weight: 800; color: #D96B27;");

    scoreLayout->addWidget(lblGameScore);
    scoreLayout->addWidget(lblGameStreak);
    scoreLayout->addStretch();
    layout->addLayout(scoreLayout);

    QWidget *itemCard = new QWidget();
    itemCard->setObjectName("DarkCard");
    QVBoxLayout *icLayout = new QVBoxLayout(itemCard);
    icLayout->setContentsMargins(35, 35, 35, 35);

    lblGameItem = new QLabel("Plastic Bottle");
    lblGameItem->setAlignment(Qt::AlignCenter);
    lblGameItem->setStyleSheet("font-size: 32px; font-weight: 800; color: #F0F4F1; letter-spacing: 1px;");
    icLayout->addWidget(lblGameItem);
    layout->addWidget(itemCard);

    QLabel *lblPrompt = new QLabel("Select the designated disposal container category:");
    lblPrompt->setStyleSheet("font-size: 13px; color: #9AA59F; font-weight: bold;");
    layout->addWidget(lblPrompt);

    QGridLayout *btnGrid = new QGridLayout();
    QStringList categories = {"Organic", "Plastic", "Paper", "E-waste", "Glass", "Metal", "General"};

    int r = 0, c = 0;
    for (const QString& cat : categories) {
        QPushButton *btn = new QPushButton(cat);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("font-size: 14px; padding: 14px; font-weight: bold;");
        connect(btn, &QPushButton::clicked, this, [this, cat]() { onGameAnswerClicked(cat); });
        btnGrid->addWidget(btn, r, c);
        c++;
        if (c > 3) { c = 0; r++; }
    }
    layout->addLayout(btnGrid);

    lblGameFeedback = new QLabel("");
    lblGameFeedback->setAlignment(Qt::AlignCenter);
    lblGameFeedback->setStyleSheet("font-size: 15px; font-weight: bold; margin-top: 10px;");
    layout->addWidget(lblGameFeedback);

    layout->addStretch();
    return page;
}

void MainWindow::resetGameItem() {
    static const std::vector<std::pair<QString, QString>> items = {
        {"Banana Peel", "Organic"}, {"Plastic Water Bottle", "Plastic"},
        {"Cardboard Box", "Paper"}, {"Old Smartphone", "E-waste"},
        {"Wine Glass", "Glass"}, {"Aluminum Soda Can", "Metal"},
        {"Soiled Pizza Box", "General"}, {"Apple Core", "Organic"},
        {"Broken Laptop Battery", "E-waste"}, {"Newspaper", "Paper"}
    };

    int idx = rand() % items.size();
    currentWasteItem = items[idx].first;
    currentWasteCategory = items[idx].second;
    lblGameItem->setText(currentWasteItem);
}

void MainWindow::onGameAnswerClicked(const QString& selectedType) {
    if (selectedType == currentWasteCategory) {
        gameScore += 10;
        gameStreak++;
        lblGameFeedback->setText("✅ Correct Classification (+10 Points)");
        lblGameFeedback->setStyleSheet("color: #5C6E4E; font-size: 15px; font-weight: bold;");
    } else {
        gameStreak = 0;
        lblGameFeedback->setText(QString("❌ Incorrect! %1 belongs in %2").arg(currentWasteItem, currentWasteCategory));
        lblGameFeedback->setStyleSheet("color: #D96B27; font-size: 15px; font-weight: bold;");
    }

    lblGameScore->setText("SCORE: " + QString::number(gameScore));
    lblGameStreak->setText(QString("STREAK: %1 🔥").arg(gameStreak));
    resetGameItem();
}

// ---------------- COLLECTION HISTORY ----------------
QWidget* MainWindow::createHistoryPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(18);

    QLabel *lblTitle = new QLabel("COLLECTION HISTORY & TIMELINE");
    lblTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #E4E8E5; letter-spacing: 1px;");
    layout->addWidget(lblTitle);

    QHBoxLayout *topBar = new QHBoxLayout();
    txtHistorySearch = new QLineEdit();
    txtHistorySearch->setPlaceholderText("Search history records...");
    QPushButton *btnAddHist = new QPushButton("+ LOG COLLECTION");
    QPushButton *btnDelHist = new QPushButton("DELETE RECORD");
    btnDelHist->setObjectName("DangerBtn");

    connect(btnAddHist, &QPushButton::clicked, this, &MainWindow::onAddHistoryClicked);
    connect(btnDelHist, &QPushButton::clicked, this, &MainWindow::onDeleteHistoryClicked);

    topBar->addWidget(txtHistorySearch, 3);
    topBar->addWidget(btnAddHist);
    topBar->addWidget(btnDelHist);
    layout->addLayout(topBar);

    tblHistory = new QTableWidget(0, 7);
    tblHistory->setHorizontalHeaderLabels({"Record ID", "Bin ID", "Timestamp", "Waste Type", "Amount (kg)", "Collector", "Status"});
    tblHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tblHistory);

    return page;
}

void MainWindow::refreshHistoryTable() {
    tblHistory->setRowCount(0);
    for (const auto& hr : manager.getHistory()) {
        int r = tblHistory->rowCount();
        tblHistory->insertRow(r);
        tblHistory->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(hr.recordId)));
        tblHistory->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(hr.binId)));
        tblHistory->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(hr.timestamp)));
        tblHistory->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(hr.wasteType)));
        tblHistory->setItem(r, 4, new QTableWidgetItem(QString::number(hr.amountCollectedKg, 'f', 1)));
        tblHistory->setItem(r, 5, new QTableWidgetItem(QString::fromStdString(hr.collectorName)));
        tblHistory->setItem(r, 6, new QTableWidgetItem(QString::fromStdString(hr.status)));
    }
}

void MainWindow::onAddHistoryClicked() {
    bool ok;
    QString binId = QInputDialog::getText(this, "Log History", "Bin ID:", QLineEdit::Normal, "BIN-101", &ok);
    if (!ok || binId.isEmpty()) return;

    double amount = QInputDialog::getDouble(this, "Log History", "Amount Collected (kg):", 150.0, 1, 1000, 1, &ok);
    if (!ok) return;

    HistoryRecord hr;
    hr.recordId = "REC-" + std::to_string(rand() % 9000 + 1000);
    hr.binId = binId.toStdString();
    hr.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm").toStdString();
    hr.wasteType = "Organic";
    hr.amountCollectedKg = amount;
    hr.collectorName = "Eco Team A";
    hr.status = "Completed";

    manager.addHistoryRecord(hr);
    refreshAllViews();
}

void MainWindow::onDeleteHistoryClicked() {
    int r = tblHistory->currentRow();
    if (r < 0) return;
    std::string recId = tblHistory->item(r, 0)->text().toStdString();
    if (manager.deleteHistoryRecord(recId)) {
        refreshAllViews();
    }
}