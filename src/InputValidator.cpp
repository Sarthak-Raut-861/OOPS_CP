#include "InputValidator.hpp"

namespace fitmetric {

bool InputValidator::isAdult(int ageYears) {
    return ageYears >= 18;
}

bool InputValidator::isValidWeight(double weightKg) {
    return weightKg > 0.0 && weightKg <= 650.0;
}

bool InputValidator::isValidHeight(double heightCm) {
    return heightCm > 0.0 && heightCm <= 300.0;
}

bool InputValidator::isValidAge(int ageYears) {
    return ageYears > 0 && ageYears <= 130;
}

void InputValidator::validate(const MeasurementRecord& record) {
    // Check weight
    if (record.getWeightKg() <= 0.0) {
        throw ValidationException("Weight must be strictly greater than 0 kg. Provided: " +
                                  std::to_string(record.getWeightKg()));
    }
    if (!isValidWeight(record.getWeightKg())) {
        throw ValidationException("Weight is outside realistic biological bounds (0 - 650 kg). Provided: " +
                                  std::to_string(record.getWeightKg()));
    }

    // Check height
    if (record.getHeightCm() <= 0.0) {
        throw ValidationException("Height must be strictly greater than 0 cm. Provided: " +
                                  std::to_string(record.getHeightCm()));
    }
    if (!isValidHeight(record.getHeightCm())) {
        throw ValidationException("Height is outside realistic biological bounds (0 - 300 cm). Provided: " +
                                  std::to_string(record.getHeightCm()));
    }

    // Check age
    if (record.getAgeYears() <= 0) {
        throw ValidationException("Age must be strictly positive. Provided: " +
                                  std::to_string(record.getAgeYears()));
    }
    if (!isValidAge(record.getAgeYears())) {
        throw ValidationException("Age is outside realistic human bounds (1 - 130 years). Provided: " +
                                  std::to_string(record.getAgeYears()));
    }

    // Check weight bounds if defined
    if (record.hasWeightBounds()) {
        const Interval& wb = record.getWeightBoundsKg();
        if (wb.getMin() <= 0.0) {
            throw ValidationException("Weight lower bound must be greater than 0 kg.");
        }
        if (wb.getMin() > wb.getMax()) {
            throw ValidationException("Weight lower bound exceeds upper bound.");
        }
        if (record.getWeightKg() < wb.getMin() || record.getWeightKg() > wb.getMax()) {
            // Note: nominal weight should reasonably lie within the declared bounds
            throw ValidationException("Nominal weight (" + std::to_string(record.getWeightKg()) +
                                      " kg) falls outside declared bounds " + wb.toString());
        }
    }

    // Check height bounds if defined
    if (record.hasHeightBounds()) {
        const Interval& hb = record.getHeightBoundsCm();
        if (hb.getMin() <= 0.0) {
            throw ValidationException("Height lower bound must be greater than 0 cm.");
        }
        if (hb.getMin() > hb.getMax()) {
            throw ValidationException("Height lower bound exceeds upper bound.");
        }
        if (record.getHeightCm() < hb.getMin() || record.getHeightCm() > hb.getMax()) {
            throw ValidationException("Nominal height (" + std::to_string(record.getHeightCm()) +
                                      " cm) falls outside declared bounds " + hb.toString());
        }
    }
}

} // namespace fitmetric
