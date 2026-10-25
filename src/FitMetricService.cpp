#include "FitMetricService.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace fitmetric {

FitMetricService::FitMetricService(std::shared_ptr<HistoryRepository> repository)
    : bmiCalc_(new BMICalculator()),
      bmrCalc_(new BMRCalculator()),
      tdeeCalc_(new TDEECalculator()),
      stabilityEngine_(new InterpretationStabilityEngine()),
      repository_(repository ? repository : std::make_shared<HistoryRepository>()) {}

std::string FitMetricService::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* tmPtr = std::localtime(&timeNow);
    std::ostringstream ss;
    if (tmPtr) {
        ss << std::put_time(tmPtr, "%Y-%m-%d %H:%M:%S");
    } else {
        ss << "2026-10-25 12:00:00";
    }
    return ss.str();
}

std::string FitMetricService::generateId(const std::string& prefix) const {
    static long counter = 1000;
    counter++;
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    std::ostringstream ss;
    ss << prefix << "-" << (ms % 100000) << "-" << counter;
    return ss.str();
}

EvidenceTrace FitMetricService::processAssessment(MeasurementRecord record, bool saveToHistory) {
    // 1. Validation step
    InputValidator::validate(record);

    std::string timestamp = record.getTimestamp().empty() ? getCurrentTimestamp() : record.getTimestamp();
    record.setTimestamp(timestamp);

    std::string sessionId = record.getRecordId().empty() ? generateId("sess") : record.getRecordId();
    record.setRecordId(sessionId);

    std::string resultId = generateId("res");
    std::string traceId = generateId("trace");

    // 2. Perform Calculations
    double nominalBmi = bmiCalc_->calculate(record);
    Interval bmiInterval = bmiCalc_->calculateInterval(record);
    BMICategory nominalCategory = bmiCalc_->determineCategory(nominalBmi, record.getAgeYears());

    double nominalBmr = bmrCalc_->calculate(record);
    double nominalTdee = tdeeCalc_->calculateWithBMR(nominalBmr, record.getActivityLevel());

    // 3. Evaluate Stability with ISVE
    StabilityAssessmentResult stabRes = stabilityEngine_->assess(record, nominalBmi, bmiInterval);

    // 4. Construct Structured Evidence Trace
    EvidenceTrace trace;
    trace.setTraceId(traceId);
    trace.setResultId(resultId);
    trace.setTimestamp(timestamp);

    trace.setInputs(
        record.getWeightKg(), record.getHeightCm(), record.getAgeYears(),
        record.getSex(), record.getActivityLevel(), record.getProfileContext(),
        record.getWeightBoundsKg(), record.getHeightBoundsCm(),
        record.getWeightSource(), record.getHeightSource()
    );

    trace.setVersions(
        bmiCalc_->getFormulaVersion(),
        bmrCalc_->getFormulaVersion(),
        tdeeCalc_->getFormulaVersion(),
        stabilityEngine_->getRuleDefinition().getVersion()
    );

    trace.setCalculatedMetrics(nominalBmi, nominalBmr, nominalTdee, nominalCategory, bmiInterval);

    trace.setStabilityAssessment(
        stabRes.state,
        stabRes.crossedBoundaries,
        stabRes.decisionReason,
        stabRes.athleteContextNote
    );

    trace.setVerificationResults(stabRes.verificationCandidates, stabRes.verificationSummary);

    // 5. Persist to History Repository if requested
    if (saveToHistory && repository_) {
        repository_->saveSessionAndResult(
            record,
            sessionId,
            resultId,
            nominalBmi,
            nominalBmr,
            nominalTdee,
            bmiCategoryToString(nominalCategory),
            trace
        );
    }

    return trace;
}

std::vector<SavedResultRecord> FitMetricService::getHistory(const std::string& profileId) const {
    if (!repository_) return {};
    return repository_->getHistoryByProfileId(profileId);
}

SavedResultRecord FitMetricService::getResult(const std::string& resultId) const {
    if (!repository_) throw PersistenceException("Repository not initialized");
    return repository_->getResultById(resultId);
}

} // namespace fitmetric
