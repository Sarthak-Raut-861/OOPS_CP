#include "BMICalculator.hpp"
#include <cmath>

namespace fitmetric {

double BMICalculator::calculate(const MeasurementRecord& record) {
    double weightKg = record.getWeightKg();
    double heightM = record.getHeightM();

    if (heightM <= 0.0) {
        throw CalculationException("Height must be greater than zero for BMI calculation.");
    }
    if (weightKg <= 0.0) {
        throw CalculationException("Weight must be greater than zero for BMI calculation.");
    }

    double bmi = weightKg / (heightM * heightM);
    // Round to 2 decimal places for standard reporting
    return std::round(bmi * 100.0) / 100.0;
}

Interval BMICalculator::calculateInterval(const MeasurementRecord& record) const {
    if (!record.hasAnyBounds()) {
        return Interval(); // Undefined interval
    }

    double wMin = record.hasWeightBounds() ? record.getWeightBoundsKg().getMin() : record.getWeightKg();
    double wMax = record.hasWeightBounds() ? record.getWeightBoundsKg().getMax() : record.getWeightKg();

    double hMinM = record.hasHeightBounds() ? (record.getHeightBoundsCm().getMin() / 100.0) : record.getHeightM();
    double hMaxM = record.hasHeightBounds() ? (record.getHeightBoundsCm().getMax() / 100.0) : record.getHeightM();

    if (hMinM <= 0.0 || hMaxM <= 0.0) {
        throw CalculationException("Height bounds must be positive for interval calculation.");
    }

    // Interval extreme formula:
    // BMI min = Wmin / (Hmax)^2
    // BMI max = Wmax / (Hmin)^2
    double bmiMin = wMin / (hMaxM * hMaxM);
    double bmiMax = wMax / (hMinM * hMinM);

    bmiMin = std::round(bmiMin * 100.0) / 100.0;
    bmiMax = std::round(bmiMax * 100.0) / 100.0;

    return Interval(bmiMin, bmiMax);
}

BMICategory BMICalculator::determineCategory(double bmiValue, int ageYears) const {
    // If under 18 years of age, out of scope for adult categories
    if (ageYears < 18) {
        return BMICategory::OutOfScope;
    }

    if (bmiValue < BMI_THRESHOLD_UNDERWEIGHT) {
        return BMICategory::Underweight;
    } else if (bmiValue < BMI_THRESHOLD_NORMAL) {
        return BMICategory::NormalWeight;
    } else if (bmiValue < BMI_THRESHOLD_OVERWEIGHT) {
        return BMICategory::Overweight;
    } else {
        return BMICategory::ObesityRange;
    }
}

} // namespace fitmetric
