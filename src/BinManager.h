#ifndef BINMANAGER_H
#define BINMANAGER_H

#include <vector>
#include <string>
#include <QString>
#include "Bin.h"
#include "HistoryRecord.h"
#include "HashTable.h"
#include "MaxHeap.h"
#include "Queue.h"
#include "LinkedList.h"

class BinManager {
private:
    std::vector<Bin> bins;
    HashTable<std::string, Bin> hashTable;
    MaxHeap maxHeap;
    CustomQueue<std::string> dispatchQueue;
    CustomLinkedList<HistoryRecord> collectionHistory;
    std::string dataFilePath;

    void rebuildDSAs();

public:
    explicit BinManager(const std::string& filePath = "trashtalk_data.json");

    void loadInitialSampleBins();
    bool loadFromJson();
    bool saveToJson() const;

    // CRUD Bins
    bool addBin(const Bin& bin);
    bool updateBin(const Bin& bin);
    bool deleteBin(const std::string& id);
    bool findBinById(const std::string& id, Bin& outBin) const;

    std::vector<Bin> getAllBins() const { return bins; }
    std::vector<Bin> getFilteredBins(const std::string& locationFilter,
                                     const std::string& typeFilter,
                                     const std::string& statusFilter) const;

    // Queue & Collection Operations
    bool enqueueBinToDispatch(const std::string& id);
    bool removeBinFromDispatch(const std::string& id);
    bool collectSingleFromQueue(const std::string& id);
    int collectAllInQueue();

    // Analytics
    int getTotalBins() const;
    int getActiveBins() const;
    int getCriticalBins() const;
    double getAverageFillPercentage() const;
    double getTotalWasteCollected() const;
    double getRecyclingPercentage() const;

    // Collection History CRUD
    void addHistoryRecord(const HistoryRecord& record);
    bool deleteHistoryRecord(const std::string& recordId);
    std::vector<HistoryRecord> getHistory() const;

    // Custom DSA Accessors
    const HashTable<std::string, Bin>& getHashTable() const { return hashTable; }
    const MaxHeap& getMaxHeap() const { return maxHeap; }
    CustomQueue<std::string>& getDispatchQueue() { return dispatchQueue; }
    const CustomLinkedList<HistoryRecord>& getHistoryLinkedList() const { return collectionHistory; }

    void simulateFillIncrement();
};

#endif // BINMANAGER_H