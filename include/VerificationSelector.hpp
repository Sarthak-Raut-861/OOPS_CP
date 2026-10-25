#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "Interval.hpp"
#include <string>
#include <vector>

namespace fitmetric {

struct VerificationCandidate {
    int rank;                            // 1 = top priority
    std::string targetMeasurement;       // "Weight" or "Height"
    std::string proposedTool;            // e.g., "Calibrated Class III Clinical Scale (+/- 0.1 kg)"
    double targetUncertainty;            // +/- value
    std::string unit;                    // "kg" or "cm"
    Interval simulatedInterval;          // Simulated BMI interval under target precision
    double intervalReductionPercent;    // % reduction in interval width
    double sensitivity;                  // Partial derivative dBMI/dX
    bool resolvesAmbiguity;              // Whether simulated interval clears crossed boundary
    double score;                        // Composite heuristic priority score
    std::string rationale;               // Structured explanation of why ranked here
};

class VerificationSelector {
public:
    VerificationSelector() = default;

    // Evaluates eligible measurements, simulates target precision, and ranks actions deterministically
    std::vector<VerificationCandidate> evaluateAndRank(
        const MeasurementRecord& record,
        const Interval& currentBmiInterval,
        const std::vector<double>& crossedBoundaries) const;

    std::string generateTopActionSummary(const std::vector<VerificationCandidate>& rankedList) const;
};

} // namespace fitmetric
