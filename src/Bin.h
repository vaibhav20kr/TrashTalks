#ifndef BIN_H
#define BIN_H

#include <string>
#include <algorithm>

struct Bin {
    std::string id;
    std::string location;
    std::string type;              // Organic, Plastic, Paper, E-waste, Glass, Metal, General
    double capacityLiters = 240.0;
    double fillLevel = 0.0;         // 0.0 to 100.0%
    double lastEmptiedHoursAgo = 0.0;
    double riskFactor = 0.0;        // 0.0 to 100.0 (fire/hazard risk)
    std::string status = "Active";  // Active, Critical, Maintenance

    double getPriorityScore() const {
        // Priority formula: 0.6 * Fill Score + 0.3 * Time Score + 0.1 * Risk Score
        double timeScore = std::min(100.0, (lastEmptiedHoursAgo / 72.0) * 100.0);
        return (0.6 * fillLevel) + (0.3 * timeScore) + (0.1 * riskFactor);
    }
};

#endif // BIN_H