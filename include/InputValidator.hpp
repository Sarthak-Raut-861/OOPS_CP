#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include <string>
#include <vector>

namespace fitmetric {

class InputValidator {
public:
    static void validate(const MeasurementRecord& record);
    static bool isAdult(int ageYears);
    static bool isValidWeight(double weightKg);
    static bool isValidHeight(double heightCm);
    static bool isValidAge(int ageYears);
};

} // namespace fitmetric
