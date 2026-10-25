#pragma once

#include "MetricCalculator.hpp"
#include "BMRCalculator.hpp"
#include <memory>

namespace fitmetric {

class TDEECalculator : public MetricCalculator {
private:
    std::unique_ptr<BMRCalculator> bmrCalculator_; // Object composition

public:
    TDEECalculator() : bmrCalculator_(new BMRCalculator()) {}
    ~TDEECalculator() override = default;

    double calculate(const MeasurementRecord& record) override;

    double calculateWithBMR(double bmr, ActivityLevel activity) const;

    std::string getMetricName() const override { return "Total Daily Energy Expenditure (TDEE)"; }
    std::string getFormulaVersion() const override { return TDEE_FORMULA_VERSION; }
    std::string getDescription() const override {
        return "TDEE = BMR * Activity Factor. Approximates total daily calorie burn based on physical activity profile.";
    }
};

} // namespace fitmetric
