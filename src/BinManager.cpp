#include "BinManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <algorithm>
#include <cstdlib>

BinManager::BinManager(const std::string& filePath) : dataFilePath(filePath) {
    if (!loadFromJson()) {
        loadInitialSampleBins();
        saveToJson();
    }
    rebuildDSAs();
}

void BinManager::loadInitialSampleBins() {
    bins = {
        {"BIN-101", "Central Park - North", "Organic", 240.0, 85.5, 48.0, 15.0, "Critical"},
        {"BIN-102", "Central Park - South", "Plastic", 240.0, 42.0, 24.0, 10.0, "Active"},
        {"BIN-103", "Main St Shopping Mall", "Paper", 360.0, 92.0, 72.0, 25.0, "Critical"},
        {"BIN-104", "Tech Park Complex", "E-waste", 120.0, 65.0, 96.0, 60.0, "Active"},
        {"BIN-105", "Metro Station West", "Glass", 240.0, 30.0, 12.0, 5.0, "Active"},
        {"BIN-106", "City General Hospital", "General", 360.0, 88.0, 18.0, 75.0, "Critical"},
        {"BIN-107", "University Campus Quad", "Organic", 240.0, 74.0, 36.0, 20.0, "Active"},
        {"BIN-108", "Downtown Market Square", "Plastic", 240.0, 95.0, 60.0, 35.0, "Critical"},
        {"BIN-109", "Harbor View Promenade", "Metal", 180.0, 25.0, 10.0, 0.0, "Active"},
        {"BIN-110", "Industrial Park East", "General", 480.0, 55.0, 40.0, 30.0, "Active"},
        {"BIN-111", "Sports Arena Gate A", "Paper", 360.0, 79.0, 50.0, 10.0, "Active"},
        {"BIN-112", "Residential Sector 4", "Organic", 240.0, 62.0, 30.0, 15.0, "Active"},
        {"BIN-113", "Railway Central Terminal", "Plastic", 360.0, 91.0, 66.0, 40.0, "Critical"},
        {"BIN-114", "Community Library", "Paper", 120.0, 18.0, 8.0, 0.0, "Active"},
        {"BIN-115", "Riverfront Food Court", "Organic", 240.0, 84.0, 52.0, 45.0, "Critical"}
    };

    collectionHistory.clear();
    collectionHistory.insertBack({"REC-001", "BIN-101", "2026-11-01 10:00", "Organic", 180.0, "John Doe", "Completed"});
    collectionHistory.insertBack({"REC-002", "BIN-103", "2026-11-02 14:30", "Paper", 290.0, "Alice Smith", "Completed"});
    collectionHistory.insertBack({"REC-003", "BIN-108", "2026-11-03 09:15", "Plastic", 210.0, "Bob Johnson", "Completed"});
}

void BinManager::rebuildDSAs() {
    hashTable.clear();
    maxHeap.clear();

    std::vector<std::string> existingQueue = dispatchQueue.toVector();

    for (const auto& b : bins) {
        hashTable.insert(b.id, b);

        HeapNode hn;
        hn.binId = b.id;
        hn.location = b.location;
        hn.fillLevel = b.fillLevel;
        hn.hoursAgo = b.lastEmptiedHoursAgo;
        hn.riskFactor = b.riskFactor;
        hn.priorityScore = b.getPriorityScore();
        maxHeap.insert(hn);

        if (b.fillLevel >= 80.0) {
            if (std::find(existingQueue.begin(), existingQueue.end(), b.id) == existingQueue.end()) {
                existingQueue.push_back(b.id);
            }
        }
    }

    dispatchQueue.clear();
    for (const auto& id : existingQueue) {
        Bin dummy;
        if (hashTable.search(id, dummy)) {
            dispatchQueue.enqueue(id);
        }
    }
}

bool BinManager::loadFromJson() {
    QFile file(QString::fromStdString(dataFilePath));
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) return false;

    QJsonObject root = doc.object();
    QJsonArray binArray = root["bins"].toArray();
    bins.clear();

    for (const auto& val : binArray) {
        QJsonObject obj = val.toObject();
        Bin b;
        b.id = obj["id"].toString().toStdString();
        b.location = obj["location"].toString().toStdString();
        b.type = obj["type"].toString().toStdString();
        b.capacityLiters = obj["capacityLiters"].toDouble(240.0);
        b.fillLevel = obj["fillLevel"].toDouble(0.0);
        b.lastEmptiedHoursAgo = obj["lastEmptiedHoursAgo"].toDouble(0.0);
        b.riskFactor = obj["riskFactor"].toDouble(0.0);
        b.status = obj["status"].toString().toStdString();
        bins.push_back(b);
    }

    QJsonArray histArray = root["history"].toArray();
    collectionHistory.clear();
    for (const auto& val : histArray) {
        QJsonObject obj = val.toObject();
        HistoryRecord hr;
        hr.recordId = obj["recordId"].toString().toStdString();
        hr.binId = obj["binId"].toString().toStdString();
        hr.timestamp = obj["timestamp"].toString().toStdString();
        hr.wasteType = obj["wasteType"].toString().toStdString();
        hr.amountCollectedKg = obj["amountCollectedKg"].toDouble(0.0);
        hr.collectorName = obj["collectorName"].toString().toStdString();
        hr.status = obj["status"].toString().toStdString();
        collectionHistory.insertBack(hr);
    }

    rebuildDSAs();
    return true;
}

bool BinManager::saveToJson() const {
    QJsonObject root;
    QJsonArray binArray;

    for (const auto& b : bins) {
        QJsonObject obj;
        obj["id"] = QString::fromStdString(b.id);
        obj["location"] = QString::fromStdString(b.location);
        obj["type"] = QString::fromStdString(b.type);
        obj["capacityLiters"] = b.capacityLiters;
        obj["fillLevel"] = b.fillLevel;
        obj["lastEmptiedHoursAgo"] = b.lastEmptiedHoursAgo;
        obj["riskFactor"] = b.riskFactor;
        obj["status"] = QString::fromStdString(b.status);
        binArray.append(obj);
    }
    root["bins"] = binArray;

    QJsonArray histArray;
    for (const auto& hr : collectionHistory.toVector()) {
        QJsonObject obj;
        obj["recordId"] = QString::fromStdString(hr.recordId);
        obj["binId"] = QString::fromStdString(hr.binId);
        obj["timestamp"] = QString::fromStdString(hr.timestamp);
        obj["wasteType"] = QString::fromStdString(hr.wasteType);
        obj["amountCollectedKg"] = hr.amountCollectedKg;
        obj["collectorName"] = QString::fromStdString(hr.collectorName);
        obj["status"] = QString::fromStdString(hr.status);
        histArray.append(obj);
    }
    root["history"] = histArray;

    QFile file(QString::fromStdString(dataFilePath));
    if (!file.open(QIODevice::WriteOnly)) return false;

    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}

bool BinManager::addBin(const Bin& bin) {
    Bin existing;
    if (hashTable.search(bin.id, existing)) return false;
    bins.push_back(bin);
    rebuildDSAs();
    saveToJson();
    return true;
}

bool BinManager::updateBin(const Bin& bin) {
    for (auto& b : bins) {
        if (b.id == bin.id) {
            b = bin;
            rebuildDSAs();
            saveToJson();
            return true;
        }
    }
    return false;
}

bool BinManager::deleteBin(const std::string& id) {
    auto it = std::remove_if(bins.begin(), bins.end(), [&](const Bin& b) { return b.id == id; });
    if (it != bins.end()) {
        bins.erase(it, bins.end());
        dispatchQueue.remove(id);
        rebuildDSAs();
        saveToJson();
        return true;
    }
    return false;
}

bool BinManager::findBinById(const std::string& id, Bin& outBin) const {
    return hashTable.search(id, outBin);
}

std::vector<Bin> BinManager::getFilteredBins(const std::string& locationFilter,
                                             const std::string& typeFilter,
                                             const std::string& statusFilter) const {
    std::vector<Bin> filtered;
    for (const auto& b : bins) {
        if (!locationFilter.empty() && locationFilter != "All" && b.location.find(locationFilter) == std::string::npos)
            continue;
        if (!typeFilter.empty() && typeFilter != "All" && b.type != typeFilter)
            continue;
        if (!statusFilter.empty() && statusFilter != "All" && b.status != statusFilter)
            continue;
        filtered.push_back(b);
    }
    return filtered;
}

bool BinManager::enqueueBinToDispatch(const std::string& id) {
    Bin b;
    if (findBinById(id, b)) {
        auto currentQueue = dispatchQueue.toVector();
        if (std::find(currentQueue.begin(), currentQueue.end(), id) == currentQueue.end()) {
            dispatchQueue.enqueue(id);
            saveToJson();
            return true;
        }
    }
    return false;
}

bool BinManager::removeBinFromDispatch(const std::string& id) {
    bool res = dispatchQueue.remove(id);
    if (res) {
        saveToJson();
    }
    return res;
}

bool BinManager::collectSingleFromQueue(const std::string& id) {
    if (!dispatchQueue.remove(id)) return false;
    Bin b;
    if (findBinById(id, b)) {
        double collectedKg = (b.fillLevel / 100.0) * (b.capacityLiters * 0.45);
        if (collectedKg < 10.0) collectedKg = 18.0;

        HistoryRecord hr;
        hr.recordId = "REC-" + std::to_string(rand() % 90000 + 10000);
        hr.binId = b.id;
        hr.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm").toStdString();
        hr.wasteType = b.type;
        hr.amountCollectedKg = collectedKg;
        hr.collectorName = "Priority Fleet";
        hr.status = "Completed";

        b.fillLevel = 0.0;
        b.lastEmptiedHoursAgo = 0.0;
        b.status = "Active";

        for (auto& item : bins) {
            if (item.id == b.id) {
                item = b;
                break;
            }
        }
        collectionHistory.insertFront(hr);
    }
    rebuildDSAs();
    saveToJson();
    return true;
}

int BinManager::collectAllInQueue() {
    int collectedCount = 0;
    while (!dispatchQueue.isEmpty()) {
        std::string binId = dispatchQueue.dequeue();
        Bin b;
        if (findBinById(binId, b)) {
            double collectedKg = (b.fillLevel / 100.0) * (b.capacityLiters * 0.45);
            if (collectedKg < 10.0) collectedKg = 18.0;

            HistoryRecord hr;
            hr.recordId = "REC-" + std::to_string(rand() % 90000 + 10000);
            hr.binId = b.id;
            hr.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm").toStdString();
            hr.wasteType = b.type;
            hr.amountCollectedKg = collectedKg;
            hr.collectorName = "Priority Fleet";
            hr.status = "Completed";

            b.fillLevel = 0.0;
            b.lastEmptiedHoursAgo = 0.0;
            b.status = "Active";

            for (auto& item : bins) {
                if (item.id == b.id) {
                    item = b;
                    break;
                }
            }
            collectionHistory.insertFront(hr);
            collectedCount++;
        }
    }
    rebuildDSAs();
    saveToJson();
    return collectedCount;
}

int BinManager::getTotalBins() const { return static_cast<int>(bins.size()); }

int BinManager::getActiveBins() const {
    int count = 0;
    for (const auto& b : bins) if (b.status == "Active") count++;
    return count;
}

int BinManager::getCriticalBins() const {
    int count = 0;
    for (const auto& b : bins) if (b.fillLevel >= 80.0 || b.status == "Critical") count++;
    return count;
}

double BinManager::getAverageFillPercentage() const {
    if (bins.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& b : bins) sum += b.fillLevel;
    return sum / bins.size();
}

double BinManager::getTotalWasteCollected() const {
    double total = 0.0;
    for (const auto& hr : collectionHistory.toVector()) {
        total += hr.amountCollectedKg;
    }
    return total;
}

double BinManager::getRecyclingPercentage() const {
    double recyclableKg = 0.0;
    double totalKg = 0.0;
    for (const auto& hr : collectionHistory.toVector()) {
        totalKg += hr.amountCollectedKg;
        if (hr.wasteType == "Plastic" || hr.wasteType == "Paper" || hr.wasteType == "Glass" || hr.wasteType == "Metal" || hr.wasteType == "E-waste") {
            recyclableKg += hr.amountCollectedKg;
        }
    }
    return totalKg == 0.0 ? 0.0 : (recyclableKg / totalKg) * 100.0;
}

void BinManager::addHistoryRecord(const HistoryRecord& record) {
    collectionHistory.insertFront(record);
    saveToJson();
}

bool BinManager::deleteHistoryRecord(const std::string& recordId) {
    bool res = collectionHistory.remove([&](const HistoryRecord& hr) { return hr.recordId == recordId; });
    if (res) saveToJson();
    return res;
}

std::vector<HistoryRecord> BinManager::getHistory() const {
    return collectionHistory.toVector();
}

void BinManager::simulateFillIncrement() {
    for (auto& b : bins) {
        b.fillLevel = std::min(100.0, b.fillLevel + (rand() % 15 + 2));
        b.lastEmptiedHoursAgo += 4.0;
        if (b.fillLevel >= 80.0) b.status = "Critical";
    }
    rebuildDSAs();
    saveToJson();
}