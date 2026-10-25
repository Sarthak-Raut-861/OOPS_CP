#pragma once

#include "MetricCalculator.hpp"

namespace fitmetric {

class BMICalculator : public MetricCalculator {
public:
    BMICalculator() = default;
    ~BMICalculator() override = default;

    // Implementation of pure virtual calculate
    double calculate(const MeasurementRecord& record) override;

    // Supported Interval Calculation:
    // BMI min = Wmin / Hmax^2
    // BMI max = Wmax / Hmin^2
    Interval calculateInterval(const MeasurementRecord& record) const;

    // Categorization logic
    BMICategory determineCategory(double bmiValue, int ageYears) const;

    std::string getMetricName() const override { return "Body Mass Index (BMI)"; }
    std::string getFormulaVersion() const override { return BMI_FORMULA_VERSION; }
    std::string getDescription() const override {
        return "BMI = weight (kg) / (height (m))^2. Standard screening metric for body mass relative to stature.";
    }
};

} // namespace fitmetric
