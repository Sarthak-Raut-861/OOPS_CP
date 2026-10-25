#include "VerificationSelector.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace fitmetric {

std::vector<VerificationCandidate> VerificationSelector::evaluateAndRank(
    const MeasurementRecord& record,
    const Interval& currentBmiInterval,
    const std::vector<double>& crossedBoundaries) const {

    std::vector<VerificationCandidate> candidates;

    if (!currentBmiInterval.isDefined() || crossedBoundaries.empty()) {
        return candidates;
    }

    double currentWidth = currentBmiInterval.getWidth();
    if (currentWidth <= 0.0) currentWidth = 0.001;

    double nomWeight = record.getWeightKg();
    double nomHeightM = record.getHeightM();
    double nomHeightCm = record.getHeightCm();

    // 1. Partial derivatives (Sensitivity)
    // dBMI/dW = 1 / H^2
    double sensWeight = 1.0 / (nomHeightM * nomHeightM);
    // |dBMI/dH| = 2 * W / H^3 (where H is in metres)
    // For cm: divide by 100
    double sensHeightPerM = (2.0 * nomWeight) / (nomHeightM * nomHeightM * nomHeightM);
    double sensHeightPerCm = sensHeightPerM / 100.0;

    // --- Candidate 1: Weight Verification ---
    // Target precision: calibrated digital medical scale (+/- 0.1 kg)
    double targetWeightDelta = 0.1; // kg
    double simWMin = nomWeight - targetWeightDelta;
    double simWMax = nomWeight + targetWeightDelta;
    if (simWMin <= 0) simWMin = 0.1;

    // Height bounds in metres for simulation
    double simHMinM = record.hasHeightBounds() ? (record.getHeightBoundsCm().getMin() / 100.0) : nomHeightM;
    double simHMaxM = record.hasHeightBounds() ? (record.getHeightBoundsCm().getMax() / 100.0) : nomHeightM;

    double simBmiMin_W = simWMin / (simHMaxM * simHMaxM);
    double simBmiMax_W = simWMax / (simHMinM * simHMinM);
    Interval simInterval_W(std::round(simBmiMin_W * 100.0) / 100.0,
                           std::round(simBmiMax_W * 100.0) / 100.0);

    double width_W = simInterval_W.getWidth();
    double reduction_W = std::max(0.0, ((currentWidth - width_W) / currentWidth) * 100.0);

    bool clearsBoundary_W = true;
    for (double b : crossedBoundaries) {
        if (simInterval_W.crosses(b)) {
            clearsBoundary_W = false;
            break;
        }
    }

    // Heuristic Score for Weight:
    // Base score from reduction % + bonus for boundary clearance + sensitivity weighting
    double score_W = reduction_W + (clearsBoundary_W ? 50.0 : 0.0) + (sensWeight * 10.0);

    std::ostringstream rat_W;
    rat_W << "Verifying weight using a calibrated clinical scale (+/- 0.1 kg) contracts interval width from "
          << std::fixed << std::setprecision(2) << currentWidth << " to " << width_W
          << " (" << std::setprecision(1) << reduction_W << "% reduction). ";
    if (clearsBoundary_W) {
        rat_W << "The contracted range completely clears the boundary at " << crossedBoundaries[0]
              << ", potentially eliminating classification ambiguity.";
    } else {
        rat_W << "Although the interval narrows, the nominal position remains close to threshold "
              << crossedBoundaries[0] << "; verification narrows uncertainty but may require joint height check.";
    }

    VerificationCandidate candWeight;
    candWeight.targetMeasurement = "Weight";
    candWeight.proposedTool = "Calibrated Class III Clinical Digital Scale (+/- 0.1 kg)";
    candWeight.targetUncertainty = targetWeightDelta;
    candWeight.unit = "kg";
    candWeight.simulatedInterval = simInterval_W;
    candWeight.intervalReductionPercent = std::round(reduction_W * 10.0) / 10.0;
    candWeight.sensitivity = std::round(sensWeight * 1000.0) / 1000.0;
    candWeight.resolvesAmbiguity = clearsBoundary_W;
    candWeight.score = score_W;
    candWeight.rationale = rat_W.str();

    // --- Candidate 2: Height Verification ---
    // Target precision: rigid wall-mounted stadiometer (+/- 0.2 cm)
    double targetHeightDeltaCm = 0.2; // cm
    double simHMinCm = nomHeightCm - targetHeightDeltaCm;
    double simHMaxCm = nomHeightCm + targetHeightDeltaCm;
    if (simHMinCm <= 0) simHMinCm = 1.0;

    double simWMin_H = record.hasWeightBounds() ? record.getWeightBoundsKg().getMin() : nomWeight;
    double simWMax_H = record.hasWeightBounds() ? record.getWeightBoundsKg().getMax() : nomWeight;

    double simBmiMin_H = simWMin_H / ((simHMaxCm / 100.0) * (simHMaxCm / 100.0));
    double simBmiMax_H = simWMax_H / ((simHMinCm / 100.0) * (simHMinCm / 100.0));
    Interval simInterval_H(std::round(simBmiMin_H * 100.0) / 100.0,
                           std::round(simBmiMax_H * 100.0) / 100.0);

    double width_H = simInterval_H.getWidth();
    double reduction_H = std::max(0.0, ((currentWidth - width_H) / currentWidth) * 100.0);

    bool clearsBoundary_H = true;
    for (double b : crossedBoundaries) {
        if (simInterval_H.crosses(b)) {
            clearsBoundary_H = false;
            break;
        }
    }

    // Heuristic Score for Height:
    double score_H = reduction_H + (clearsBoundary_H ? 50.0 : 0.0) + (sensHeightPerCm * 10.0);

    std::ostringstream rat_H;
    rat_H << "Verifying height using a wall-mounted stadiometer (+/- 0.2 cm) contracts interval width from "
          << std::fixed << std::setprecision(2) << currentWidth << " to " << width_H
          << " (" << std::setprecision(1) << reduction_H << "% reduction). ";
    if (clearsBoundary_H) {
        rat_H << "The contracted range clears the threshold boundary at " << crossedBoundaries[0]
              << ", stabilizing classification under standard posture protocol.";
    } else {
        rat_H << "Uncertainty is reduced, but remaining weight variance preserves overlap around threshold "
              << crossedBoundaries[0] << ".";
    }

    VerificationCandidate candHeight;
    candHeight.targetMeasurement = "Height";
    candHeight.proposedTool = "Wall-Mounted Precision Stadiometer (+/- 0.2 cm)";
    candHeight.targetUncertainty = targetHeightDeltaCm;
    candHeight.unit = "cm";
    candHeight.simulatedInterval = simInterval_H;
    candHeight.intervalReductionPercent = std::round(reduction_H * 10.0) / 10.0;
    candHeight.sensitivity = std::round(sensHeightPerCm * 1000.0) / 1000.0;
    candHeight.resolvesAmbiguity = clearsBoundary_H;
    candHeight.score = score_H;
    candHeight.rationale = rat_H.str();

    // Deterministic comparison:
    // If score_W >= score_H, Weight is Rank 1, Height is Rank 2 (deterministic tie-break to Weight)
    if (score_W >= score_H) {
        candWeight.rank = 1;
        candHeight.rank = 2;
        candidates.push_back(candWeight);
        candidates.push_back(candHeight);
    } else {
        candHeight.rank = 1;
        candWeight.rank = 2;
        candidates.push_back(candHeight);
        candidates.push_back(candWeight);
    }

    return candidates;
}

std::string VerificationSelector::generateTopActionSummary(
    const std::vector<VerificationCandidate>& rankedList) const {
    if (rankedList.empty()) {
        return "No measurement verification action needed; interpretation is stable or unassessable.";
    }

    const auto& top = rankedList[0];
    std::ostringstream oss;
    oss << "Priority 1 Recommendation: Verify " << top.targetMeasurement << " using "
        << top.proposedTool << ". (Heuristic priority based on "
        << std::fixed << std::setprecision(1) << top.intervalReductionPercent
        << "% interval contraction and sensitivity dBMI/d" << top.targetMeasurement[0]
        << " = " << std::setprecision(3) << top.sensitivity << ". "
        << "Note: follow-up verification is a heuristic recommendation and does not guarantee complete ambiguity resolution.)";
    return oss.str();
}

} // namespace fitmetric
