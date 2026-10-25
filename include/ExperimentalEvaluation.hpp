#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "FitMetricService.hpp"
#include <string>
#include <vector>

namespace fitmetric {

struct EvaluationTestCase {
    std::string id;
    std::string description;
    double weightKg;
    double heightCm;
    int ageYears;
    bool hasBounds;
    double weightMin;
    double weightMax;
    double heightMin;
    double heightMax;
    bool groundTruthCrossesBoundary;
    double expectedBoundaryCrossed; // 0 if none
};

struct SystemBenchmarkMetrics {
    std::string systemName;
    int totalEvaluated;
    int correctBoundaryCrossDetections;
    int unsupportedDefiniteInterpretations; // Cases where system asserted a single category despite crossing bounds
    int stabilityAwareCases;
    int verificationRecommendationsProvided;
    double averageProcessingTimeUs;
    double reproducibilityScorePercent;
};

struct EvaluationReport {
    std::vector<EvaluationTestCase> testCases;
    SystemBenchmarkMetrics baselineA;      // Conventional point-estimate
    SystemBenchmarkMetrics baselineB;      // Naive interval
    SystemBenchmarkMetrics fitMetricISVE;  // FitMetric Candidate (ISVE)
    std::string summaryAnalysis;
};

class ExperimentalEvaluation {
public:
    static std::vector<EvaluationTestCase> getStandardEvaluationDataset();
    static EvaluationReport runEvaluation(FitMetricService& service);
    static std::string reportToJson(const EvaluationReport& report);
};

} // namespace fitmetric
