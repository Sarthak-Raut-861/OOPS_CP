#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "Interval.hpp"
#include "RuleDefinition.hpp"
#include "VerificationSelector.hpp"
#include "EvidenceTrace.hpp"
#include <memory>
#include <string>

namespace fitmetric {

struct StabilityAssessmentResult {
    StabilityState state;
    Interval bmiInterval;
    std::vector<double> crossedBoundaries;
    std::string decisionReason;
    std::string athleteContextNote;
    std::vector<VerificationCandidate> verificationCandidates;
    std::string verificationSummary;
};

class InterpretationStabilityEngine {
private:
    RuleDefinition ruleDefinition_;
    VerificationSelector verificationSelector_;

public:
    InterpretationStabilityEngine();

    StabilityAssessmentResult assess(
        const MeasurementRecord& record,
        double nominalBmi,
        const Interval& bmiInterval) const;

    const RuleDefinition& getRuleDefinition() const { return ruleDefinition_; }
    const VerificationSelector& getVerificationSelector() const { return verificationSelector_; }
};

} // namespace fitmetric
