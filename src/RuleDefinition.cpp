#include "RuleDefinition.hpp"

namespace fitmetric {

RuleDefinition::RuleDefinition()
    : ruleId_("WHO-ADULT-BMI-RULESET"),
      version_(ISVE_RULE_VERSION),
      authority_("World Health Organization (WHO) Technical Report Series 854"),
      minimumAgeAdult_(18) {
    thresholds_.push_back({18.5, "Underweight", "Normal-weight range", "Threshold separating underweight from normal weight"});
    thresholds_.push_back({25.0, "Normal-weight range", "Overweight", "Threshold separating normal weight from overweight"});
    thresholds_.push_back({30.0, "Overweight", "Obesity range", "Threshold separating overweight from obesity"});
}

std::vector<double> RuleDefinition::getCrossedBoundaries(double minVal, double maxVal) const {
    std::vector<double> crossed;
    for (const auto& t : thresholds_) {
        if (minVal < t.value && maxVal > t.value) {
            crossed.push_back(t.value);
        }
    }
    return crossed;
}

} // namespace fitmetric
