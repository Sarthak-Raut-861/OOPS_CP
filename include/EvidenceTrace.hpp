#pragma once

#include "Common.hpp"
#include "Interval.hpp"
#include "VerificationSelector.hpp"
#include <string>
#include <vector>

namespace fitmetric {

class EvidenceTrace {
private:
    std::string traceId_;
    std::string resultId_;
    std::string timestamp_;

    // Input values
    double weightKg_;
    double heightCm_;
    int ageYears_;
    Sex sex_;
    ActivityLevel activity_;
    ProfileContext profileContext_;

    // Bounds
    Interval weightBoundsKg_;
    Interval heightBoundsCm_;
    std::string weightSource_;
    std::string heightSource_;

    // Versioning
    std::string bmiFormulaVersion_;
    std::string bmrFormulaVersion_;
    std::string tdeeFormulaVersion_;
    std::string ruleVersion_;

    // Calculation results
    double nominalBmi_;
    double nominalBmr_;
    double nominalTdee_;
    BMICategory nominalCategory_;
    Interval bmiInterval_;

    // Stability & Decision
    StabilityState stabilityState_;
    std::vector<double> crossedBoundaries_;
    std::string decisionReason_;
    std::string athleteContextDisclaimer_;

    // Verification Candidates
    std::vector<VerificationCandidate> verificationCandidates_;
    std::string verificationSummary_;

public:
    EvidenceTrace();

    // Getters
    const std::string& getTraceId() const { return traceId_; }
    const std::string& getResultId() const { return resultId_; }
    const std::string& getTimestamp() const { return timestamp_; }

    double getWeightKg() const { return weightKg_; }
    double getHeightCm() const { return heightCm_; }
    int getAgeYears() const { return ageYears_; }
    Sex getSex() const { return sex_; }
    ActivityLevel getActivityLevel() const { return activity_; }
    ProfileContext getProfileContext() const { return profileContext_; }

    const Interval& getWeightBoundsKg() const { return weightBoundsKg_; }
    const Interval& getHeightBoundsCm() const { return heightBoundsCm_; }
    const std::string& getWeightSource() const { return weightSource_; }
    const std::string& getHeightSource() const { return heightSource_; }

    const std::string& getBmiFormulaVersion() const { return bmiFormulaVersion_; }
    const std::string& getBmrFormulaVersion() const { return bmrFormulaVersion_; }
    const std::string& getTdeeFormulaVersion() const { return tdeeFormulaVersion_; }
    const std::string& getRuleVersion() const { return ruleVersion_; }

    double getNominalBmi() const { return nominalBmi_; }
    double getNominalBmr() const { return nominalBmr_; }
    double getNominalTdee() const { return nominalTdee_; }
    BMICategory getNominalCategory() const { return nominalCategory_; }
    const Interval& getBmiInterval() const { return bmiInterval_; }

    StabilityState getStabilityState() const { return stabilityState_; }
    const std::vector<double>& getCrossedBoundaries() const { return crossedBoundaries_; }
    const std::string& getDecisionReason() const { return decisionReason_; }
    const std::string& getAthleteContextDisclaimer() const { return athleteContextDisclaimer_; }

    const std::vector<VerificationCandidate>& getVerificationCandidates() const { return verificationCandidates_; }
    const std::string& getVerificationSummary() const { return verificationSummary_; }

    // Setters
    void setTraceId(const std::string& id) { traceId_ = id; }
    void setResultId(const std::string& id) { resultId_ = id; }
    void setTimestamp(const std::string& ts) { timestamp_ = ts; }

    void setInputs(double weightKg, double heightCm, int ageYears, Sex sex,
                   ActivityLevel activity, ProfileContext ctx,
                   const Interval& wb, const Interval& hb,
                   const std::string& wSrc, const std::string& hSrc);

    void setVersions(const std::string& bmiVer, const std::string& bmrVer,
                     const std::string& tdeeVer, const std::string& ruleVer);

    void setCalculatedMetrics(double bmi, double bmr, double tdee,
                              BMICategory cat, const Interval& interval);

    void setStabilityAssessment(StabilityState state,
                                const std::vector<double>& crossed,
                                const std::string& reason,
                                const std::string& athleteNote);

    void setVerificationResults(const std::vector<VerificationCandidate>& candidates,
                                const std::string& summary);

    // Serialization for API and Persistence
    std::string toJsonString() const;
};

} // namespace fitmetric
