#include "BMRCalculator.hpp"
#include <cmath>

namespace fitmetric {

double BMRCalculator::calculate(const MeasurementRecord& record) {
    double w = record.getWeightKg();
    double h = record.getHeightCm();
    int a = record.getAgeYears();

    if (w <= 0.0 || h <= 0.0 || a <= 0) {
        throw CalculationException("Weight, height, and age must be positive for BMR calculation.");
    }

    // Mifflin-St Jeor formula:
    // Base: 10 * W + 6.25 * H - 5 * A
    double base = (10.0 * w) + (6.25 * h) - (5.0 * a);
    double bmr = 0.0;

    if (record.getSex() == Sex::Male) {
        bmr = base + 5.0;
    } else {
        bmr = base - 161.0;
    }

    // Round to 1 decimal place
    return std::round(bmr * 10.0) / 10.0;
}

} // namespace fitmetric
