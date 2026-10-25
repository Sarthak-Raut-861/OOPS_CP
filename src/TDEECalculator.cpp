#include "TDEECalculator.hpp"
#include <cmath>

namespace fitmetric {

double TDEECalculator::calculate(const MeasurementRecord& record) {
    double bmr = bmrCalculator_->calculate(record);
    return calculateWithBMR(bmr, record.getActivityLevel());
}

double TDEECalculator::calculateWithBMR(double bmr, ActivityLevel activity) const {
    if (bmr <= 0.0) {
        throw CalculationException("BMR must be positive for TDEE calculation.");
    }
    double factor = activityLevelMultiplier(activity);
    double tdee = bmr * factor;
    // Round to 1 decimal place
    return std::round(tdee * 10.0) / 10.0;
}

} // namespace fitmetric
