#include "EvidenceTrace.hpp"
#include <sstream>
#include <iomanip>

namespace fitmetric {

EvidenceTrace::EvidenceTrace()
    : weightKg_(0.0), heightCm_(0.0), ageYears_(0),
      sex_(Sex::Male), activity_(ActivityLevel::Sedentary),
      profileContext_(ProfileContext::GeneralUser),
      nominalBmi_(0.0), nominalBmr_(0.0), nominalTdee_(0.0),
      nominalCategory_(BMICategory::OutOfScope),
      stabilityState_(StabilityState::STABILITY_NOT_ASSESSABLE) {}

void EvidenceTrace::setInputs(double weightKg, double heightCm, int ageYears, Sex sex,
                              ActivityLevel activity, ProfileContext ctx,
                              const Interval& wb, const Interval& hb,
                              const std::string& wSrc, const std::string& hSrc) {
    weightKg_ = weightKg;
    heightCm_ = heightCm;
    ageYears_ = ageYears;
    sex_ = sex;
    activity_ = activity;
    profileContext_ = ctx;
    weightBoundsKg_ = wb;
    heightBoundsCm_ = hb;
    weightSource_ = wSrc;
    heightSource_ = hSrc;
}

void EvidenceTrace::setVersions(const std::string& bmiVer, const std::string& bmrVer,
                                const std::string& tdeeVer, const std::string& ruleVer) {
    bmiFormulaVersion_ = bmiVer;
    bmrFormulaVersion_ = bmrVer;
    tdeeFormulaVersion_ = tdeeVer;
    ruleVersion_ = ruleVer;
}

void EvidenceTrace::setCalculatedMetrics(double bmi, double bmr, double tdee,
                                         BMICategory cat, const Interval& interval) {
    nominalBmi_ = bmi;
    nominalBmr_ = bmr;
    nominalTdee_ = tdee;
    nominalCategory_ = cat;
    bmiInterval_ = interval;
}

void EvidenceTrace::setStabilityAssessment(StabilityState state,
                                           const std::vector<double>& crossed,
                                           const std::string& reason,
                                           const std::string& athleteNote) {
    stabilityState_ = state;
    crossedBoundaries_ = crossed;
    decisionReason_ = reason;
    athleteContextDisclaimer_ = athleteNote;
}

void EvidenceTrace::setVerificationResults(const std::vector<VerificationCandidate>& candidates,
                                           const std::string& summary) {
    verificationCandidates_ = candidates;
    verificationSummary_ = summary;
}

static std::string escapeJsonString(const std::string& input) {
    std::ostringstream ss;
    for (char c : input) {
        if (c == '"') ss << "\\\"";
        else if (c == '\\') ss << "\\\\";
        else if (c == '\b') ss << "\\b";
        else if (c == '\f') ss << "\\f";
        else if (c == '\n') ss << "\\n";
        else if (c == '\r') ss << "\\r";
        else if (c == '\t') ss << "\\t";
        else ss << c;
    }
    return ss.str();
}

std::string EvidenceTrace::toJsonString() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"traceId\": \"" << escapeJsonString(traceId_) << "\",\n";
    ss << "  \"resultId\": \"" << escapeJsonString(resultId_) << "\",\n";
    ss << "  \"timestamp\": \"" << escapeJsonString(timestamp_) << "\",\n";

    ss << "  \"inputs\": {\n";
    ss << "    \"weightKg\": " << weightKg_ << ",\n";
    ss << "    \"heightCm\": " << heightCm_ << ",\n";
    ss << "    \"ageYears\": " << ageYears_ << ",\n";
    ss << "    \"sex\": \"" << sexToString(sex_) << "\",\n";
    ss << "    \"activityLevel\": \"" << escapeJsonString(activityLevelToString(activity_)) << "\",\n";
    ss << "    \"profileContext\": \"" << profileContextToString(profileContext_) << "\",\n";
    ss << "    \"weightBoundsKg\": " << (weightBoundsKg_.isDefined() ?
          ("{\"min\": " + std::to_string(weightBoundsKg_.getMin()) + ", \"max\": " + std::to_string(weightBoundsKg_.getMax()) + "}") : "null") << ",\n";
    ss << "    \"heightBoundsCm\": " << (heightBoundsCm_.isDefined() ?
          ("{\"min\": " + std::to_string(heightBoundsCm_.getMin()) + ", \"max\": " + std::to_string(heightBoundsCm_.getMax()) + "}") : "null") << ",\n";
    ss << "    \"weightSource\": \"" << escapeJsonString(weightSource_) << "\",\n";
    ss << "    \"heightSource\": \"" << escapeJsonString(heightSource_) << "\"\n";
    ss << "  },\n";

    ss << "  \"versions\": {\n";
    ss << "    \"bmiFormula\": \"" << escapeJsonString(bmiFormulaVersion_) << "\",\n";
    ss << "    \"bmrFormula\": \"" << escapeJsonString(bmrFormulaVersion_) << "\",\n";
    ss << "    \"tdeeFormula\": \"" << escapeJsonString(tdeeFormulaVersion_) << "\",\n";
    ss << "    \"ruleVersion\": \"" << escapeJsonString(ruleVersion_) << "\"\n";
    ss << "  },\n";

    ss << "  \"results\": {\n";
    ss << "    \"bmi\": " << nominalBmi_ << ",\n";
    ss << "    \"bmr\": " << nominalBmr_ << ",\n";
    ss << "    \"tdee\": " << nominalTdee_ << ",\n";
    ss << "    \"nominalCategory\": \"" << escapeJsonString(bmiCategoryToString(nominalCategory_)) << "\",\n";
    ss << "    \"bmiInterval\": " << (bmiInterval_.isDefined() ?
          ("{\"min\": " + std::to_string(bmiInterval_.getMin()) + ", \"max\": " + std::to_string(bmiInterval_.getMax()) + "}") : "null") << "\n";
    ss << "  },\n";

    ss << "  \"stability\": {\n";
    ss << "    \"state\": \"" << stabilityStateToString(stabilityState_) << "\",\n";
    ss << "    \"crossedBoundaries\": [";
    for (size_t i = 0; i < crossedBoundaries_.size(); ++i) {
        ss << crossedBoundaries_[i] << (i + 1 < crossedBoundaries_.size() ? ", " : "");
    }
    ss << "],\n";
    ss << "    \"decisionReason\": \"" << escapeJsonString(decisionReason_) << "\",\n";
    ss << "    \"athleteContextDisclaimer\": \"" << escapeJsonString(athleteContextDisclaimer_) << "\"\n";
    ss << "  },\n";

    ss << "  \"verification\": {\n";
    ss << "    \"summary\": \"" << escapeJsonString(verificationSummary_) << "\",\n";
    ss << "    \"candidates\": [\n";
    for (size_t i = 0; i < verificationCandidates_.size(); ++i) {
        const auto& c = verificationCandidates_[i];
        ss << "      {\n";
        ss << "        \"rank\": " << c.rank << ",\n";
        ss << "        \"targetMeasurement\": \"" << escapeJsonString(c.targetMeasurement) << "\",\n";
        ss << "        \"proposedTool\": \"" << escapeJsonString(c.proposedTool) << "\",\n";
        ss << "        \"targetUncertainty\": " << c.targetUncertainty << ",\n";
        ss << "        \"unit\": \"" << c.unit << "\",\n";
        ss << "        \"simulatedInterval\": " << (c.simulatedInterval.isDefined() ?
              ("{\"min\": " + std::to_string(c.simulatedInterval.getMin()) + ", \"max\": " + std::to_string(c.simulatedInterval.getMax()) + "}") : "null") << ",\n";
        ss << "        \"intervalReductionPercent\": " << c.intervalReductionPercent << ",\n";
        ss << "        \"sensitivity\": " << c.sensitivity << ",\n";
        ss << "        \"resolvesAmbiguity\": " << (c.resolvesAmbiguity ? "true" : "false") << ",\n";
        ss << "        \"score\": " << c.score << ",\n";
        ss << "        \"rationale\": \"" << escapeJsonString(c.rationale) << "\"\n";
        ss << "      }" << (i + 1 < verificationCandidates_.size() ? ",\n" : "\n");
    }
    ss << "    ]\n";
    ss << "  }\n";
    ss << "}";

    return ss.str();
}

} // namespace fitmetric
