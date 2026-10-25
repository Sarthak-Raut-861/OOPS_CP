#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

namespace fitmetric {

// Enums
enum class Sex {
    Male,
    Female
};

enum class ActivityLevel {
    Sedentary,         // 1.20
    LightlyActive,     // 1.375
    ModeratelyActive,  // 1.55
    VeryActive,        // 1.725
    ExtremelyActive    // 1.90
};

enum class ProfileContext {
    GeneralUser,
    Athlete
};

enum class StabilityState {
    STABLE_WITHIN_DECLARED_BOUNDS,
    CROSSES_CATEGORY_BOUNDARY,
    STABILITY_NOT_ASSESSABLE,
    INVALID_OR_INCOMPLETE_INPUT
};

enum class BMICategory {
    Underweight,        // < 18.5
    NormalWeight,       // 18.5 - < 25.0
    Overweight,         // 25.0 - < 30.0
    ObesityRange,       // >= 30.0
    OutOfScope          // E.g., under 18 years of age
};

// Formula and Rule Version Constants
const std::string BMI_FORMULA_VERSION = "BMI-QUETELET-1832-V1.0";
const std::string BMR_FORMULA_VERSION = "BMR-MIFFLIN-ST-JEOR-1990-V1.0";
const std::string TDEE_FORMULA_VERSION = "TDEE-PAL-FAO-WHO-2004-V1.0";
const std::string ISVE_RULE_VERSION = "ISVE-WHO-ADULT-V1.0";

// Threshold boundaries for adult BMI
const double BMI_THRESHOLD_UNDERWEIGHT = 18.5;
const double BMI_THRESHOLD_NORMAL = 25.0;
const double BMI_THRESHOLD_OVERWEIGHT = 30.0;

// Conversion helper functions
inline std::string sexToString(Sex s) {
    return (s == Sex::Male) ? "male" : "female";
}

inline Sex stringToSex(const std::string& str) {
    if (str == "female" || str == "Female" || str == "FEMALE") return Sex::Female;
    return Sex::Male;
}

inline std::string activityLevelToString(ActivityLevel level) {
    switch (level) {
        case ActivityLevel::Sedentary: return "Sedentary (1.20)";
        case ActivityLevel::LightlyActive: return "Lightly Active (1.375)";
        case ActivityLevel::ModeratelyActive: return "Moderately Active (1.55)";
        case ActivityLevel::VeryActive: return "Very Active (1.725)";
        case ActivityLevel::ExtremelyActive: return "Extremely Active (1.90)";
        default: return "Sedentary";
    }
}

inline double activityLevelMultiplier(ActivityLevel level) {
    switch (level) {
        case ActivityLevel::Sedentary: return 1.20;
        case ActivityLevel::LightlyActive: return 1.375;
        case ActivityLevel::ModeratelyActive: return 1.55;
        case ActivityLevel::VeryActive: return 1.725;
        case ActivityLevel::ExtremelyActive: return 1.90;
        default: return 1.20;
    }
}

inline ActivityLevel stringToActivityLevel(const std::string& str) {
    if (str == "light" || str == "lightly_active" || str == "1.375") return ActivityLevel::LightlyActive;
    if (str == "moderate" || str == "moderately_active" || str == "1.55") return ActivityLevel::ModeratelyActive;
    if (str == "very" || str == "very_active" || str == "1.725") return ActivityLevel::VeryActive;
    if (str == "extreme" || str == "extremely_active" || str == "1.90") return ActivityLevel::ExtremelyActive;
    return ActivityLevel::Sedentary;
}

inline std::string profileContextToString(ProfileContext ctx) {
    return (ctx == ProfileContext::Athlete) ? "Athlete" : "General User";
}

inline ProfileContext stringToProfileContext(const std::string& str) {
    if (str == "athlete" || str == "Athlete") return ProfileContext::Athlete;
    return ProfileContext::GeneralUser;
}

inline std::string stabilityStateToString(StabilityState state) {
    switch (state) {
        case StabilityState::STABLE_WITHIN_DECLARED_BOUNDS:
            return "STABLE_WITHIN_DECLARED_BOUNDS";
        case StabilityState::CROSSES_CATEGORY_BOUNDARY:
            return "CROSSES_CATEGORY_BOUNDARY";
        case StabilityState::STABILITY_NOT_ASSESSABLE:
            return "STABILITY_NOT_ASSESSABLE";
        case StabilityState::INVALID_OR_INCOMPLETE_INPUT:
            return "INVALID_OR_INCOMPLETE_INPUT";
        default:
            return "UNKNOWN";
    }
}

inline std::string bmiCategoryToString(BMICategory cat) {
    switch (cat) {
        case BMICategory::Underweight: return "Underweight";
        case BMICategory::NormalWeight: return "Normal-weight range";
        case BMICategory::Overweight: return "Overweight";
        case BMICategory::ObesityRange: return "Obesity range";
        case BMICategory::OutOfScope: return "Out-of-scope (under 18 years)";
        default: return "Unknown";
    }
}

// Custom Exception Classes for OOP Exception Handling
class FitMetricException : public std::runtime_error {
public:
    explicit FitMetricException(const std::string& message)
        : std::runtime_error(message) {}
};

class ValidationException : public FitMetricException {
public:
    explicit ValidationException(const std::string& message)
        : FitMetricException("Validation Error: " + message) {}
};

class CalculationException : public FitMetricException {
public:
    explicit CalculationException(const std::string& message)
        : FitMetricException("Calculation Error: " + message) {}
};

class PersistenceException : public FitMetricException {
public:
    explicit PersistenceException(const std::string& message)
        : FitMetricException("Persistence Error: " + message) {}
};

} // namespace fitmetric
