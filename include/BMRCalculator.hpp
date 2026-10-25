#pragma once

#include "MetricCalculator.hpp"

namespace fitmetric {

class BMRCalculator : public MetricCalculator {
public:
    BMRCalculator() = default;
    ~BMRCalculator() override = default;

    double calculate(const MeasurementRecord& record) override;

    std::string getMetricName() const override { return "Basal Metabolic Rate (BMR)"; }
    std::string getFormulaVersion() const override { return BMR_FORMULA_VERSION; }
    std::string getDescription() const override {
        return "Mifflin-St Jeor equation: Estimates resting daily energy expenditure (kcal/day) based on weight, height, age, and sex.";
    }
};

} // namespace fitmetric
