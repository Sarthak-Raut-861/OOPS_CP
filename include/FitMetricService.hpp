#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "InputValidator.hpp"
#include "BMICalculator.hpp"
#include "BMRCalculator.hpp"
#include "TDEECalculator.hpp"
#include "InterpretationStabilityEngine.hpp"
#include "EvidenceTrace.hpp"
#include "HistoryRepository.hpp"
#include "ReportGenerator.hpp"
#include <memory>
#include <string>
#include <chrono>

namespace fitmetric {

class FitMetricService {
private:
    std::unique_ptr<BMICalculator> bmiCalc_;
    std::unique_ptr<BMRCalculator> bmrCalc_;
    std::unique_ptr<TDEECalculator> tdeeCalc_;
    std::unique_ptr<InterpretationStabilityEngine> stabilityEngine_;
    std::shared_ptr<HistoryRepository> repository_;

    std::string generateId(const std::string& prefix) const;
    std::string getCurrentTimestamp() const;

public:
    FitMetricService(std::shared_ptr<HistoryRepository> repository = nullptr);
    ~FitMetricService() = default;

    // Main facade orchestrator
    EvidenceTrace processAssessment(MeasurementRecord record, bool saveToHistory = true);

    std::vector<SavedResultRecord> getHistory(const std::string& profileId = "") const;
    SavedResultRecord getResult(const std::string& resultId) const;
    std::shared_ptr<HistoryRepository> getRepository() const { return repository_; }
};

} // namespace fitmetric
