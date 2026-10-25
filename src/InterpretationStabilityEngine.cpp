#include "InterpretationStabilityEngine.hpp"
#include <sstream>
#include <iomanip>

namespace fitmetric {

InterpretationStabilityEngine::InterpretationStabilityEngine()
    : ruleDefinition_(), verificationSelector_() {}

StabilityAssessmentResult InterpretationStabilityEngine::assess(
    const MeasurementRecord& record,
    double nominalBmi,
    const Interval& bmiInterval) const {

    StabilityAssessmentResult result;
    result.bmiInterval = bmiInterval;

    // Athlete context note
    if (record.getProfileContext() == ProfileContext::Athlete) {
        result.athleteContextNote =
            "Athlete profile active: BMI is a screening ratio and does not differentiate between dense muscle mass "
            "and adipose tissue. A high BMI does not indicate excessive body fat or metabolic risk in muscular individuals; "
            "direct body composition assessment (e.g. DEXA, calipers) is recommended.";
    } else {
        result.athleteContextNote =
            "General user profile: Adult BMI classification is a screening threshold, not a medical diagnosis or direct adiposity measurement.";
    }

    // 1. Age check: if under 18, categorical adult BMI is out-of-scope
    if (record.getAgeYears() < ruleDefinition_.getMinimumAgeAdult()) {
        result.state = StabilityState::STABILITY_NOT_ASSESSABLE;
        result.decisionReason =
            "User age (" + std::to_string(record.getAgeYears()) + " years) is below adult threshold (" +
            std::to_string(ruleDefinition_.getMinimumAgeAdult()) + " years). Adult BMI categories and stability rules "
            "must not be applied to pediatric or adolescent individuals; age-and-sex-specific BMI percentiles are required.";
        result.verificationSummary = "No verification action applicable for out-of-scope age profile.";
        return result;
    }

    // 2. Check if measurement uncertainty bounds were supplied
    if (!bmiInterval.isDefined() || !record.hasAnyBounds()) {
        result.state = StabilityState::STABILITY_NOT_ASSESSABLE;
        result.decisionReason =
            "Supported measurement uncertainty bounds were not provided. The system cannot assess classification "
            "stability across measurement ranges without declared tolerances. (Absence of uncertainty bounds is distinct "
            "from proven instability).";
        result.verificationSummary = "Stability assessment requires declaring measurement bounds or instrument tolerance.";
        return result;
    }

    // 3. Evaluate crossed category boundaries
    std::vector<double> crossed = ruleDefinition_.getCrossedBoundaries(bmiInterval.getMin(), bmiInterval.getMax());
    result.crossedBoundaries = crossed;

    if (!crossed.empty()) {
        result.state = StabilityState::CROSSES_CATEGORY_BOUNDARY;
        std::ostringstream oss;
        oss << "The calculated BMI interval " << bmiInterval.toString()
            << " crosses WHO adult classification boundary at ";
        for (size_t i = 0; i < crossed.size(); ++i) {
            oss << crossed[i] << (i + 1 < crossed.size() ? ", " : "");
        }
        oss << ". Classification is sensitive to measurement variance within the declared bounds and cannot be assigned definitively.";
        result.decisionReason = oss.str();

        // Run Verification Selector to prioritize actions
        result.verificationCandidates = verificationSelector_.evaluateAndRank(record, bmiInterval, crossed);
        result.verificationSummary = verificationSelector_.generateTopActionSummary(result.verificationCandidates);
    } else {
        result.state = StabilityState::STABLE_WITHIN_DECLARED_BOUNDS;
        std::ostringstream oss;
        oss << "The entire calculated BMI interval " << bmiInterval.toString()
            << " falls completely within a single adult classification category. "
            << "Classification remains invariant across all values within the declared measurement tolerances.";
        result.decisionReason = oss.str();
        result.verificationSummary = "Classification is stable within declared bounds; additional measurement verification is not required.";
    }

    return result;
}

} // namespace fitmetric
