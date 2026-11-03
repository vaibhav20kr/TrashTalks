#ifndef HISTORYRECORD_H
#define HISTORYRECORD_H

#include <string>

struct HistoryRecord {
    std::string recordId;
    std::string binId;
    std::string timestamp;
    std::string wasteType;
    double amountCollectedKg = 0.0;
    std::string collectorName;
    std::string status; // Completed, Pending, Cancelled
};

#endif // HISTORYRECORD_H