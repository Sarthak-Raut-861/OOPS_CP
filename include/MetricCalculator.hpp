#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "Interval.hpp"
#include <string>

namespace fitmetric {

// Abstract Base Class defining common interface for metric calculations (Abstraction)
class MetricCalculator {
public:
    virtual ~MetricCalculator() = default;

    // Pure virtual function for polymorphic execution (Polymorphism)
    virtual double calculate(const MeasurementRecord& record) = 0;

    virtual std::string getMetricName() const = 0;
    virtual std::string getFormulaVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

} // namespace fitmetric
