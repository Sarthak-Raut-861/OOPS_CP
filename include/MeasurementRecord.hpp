#pragma once

#include "Common.hpp"
#include "Interval.hpp"
#include <string>

namespace fitmetric {

class MeasurementRecord {
private:
    std::string recordId_;
    std::string profileId_;
    std::string timestamp_;

    double weightKg_;          // Nominal weight in kg
    double heightCm_;          // Nominal height in cm
    int ageYears_;             // Age in years
    Sex sex_;                  // Biological sex for BMR formula
    ActivityLevel activity_;   // Activity multiplier profile
    ProfileContext context_;   // GeneralUser or Athlete

    Interval weightBoundsKg_;  // Optional supported range [Wmin, Wmax]
    Interval heightBoundsCm_;  // Optional supported range [Hmin, Hmax]

    std::string weightSource_; // e.g., "Digital bathroom scale (+/- 0.5kg)"
    std::string heightSource_; // e.g., "Wall stadiometer (+/- 1.0cm)"

public:
    MeasurementRecord()
        : weightKg_(0.0), heightCm_(0.0), ageYears_(0),
          sex_(Sex::Male), activity_(ActivityLevel::Sedentary),
          context_(ProfileContext::GeneralUser) {}

    MeasurementRecord(double weightKg, double heightCm, int ageYears,
                      Sex sex, ActivityLevel activity, ProfileContext context,
                      const std::string& profileId = "default-user",
                      const std::string& timestamp = "")
        : profileId_(profileId), timestamp_(timestamp),
          weightKg_(weightKg), heightCm_(heightCm), ageYears_(ageYears),
          sex_(sex), activity_(activity), context_(context) {}

    // Getters
    const std::string& getRecordId() const { return recordId_; }
    const std::string& getProfileId() const { return profileId_; }
    const std::string& getTimestamp() const { return timestamp_; }

    double getWeightKg() const { return weightKg_; }
    double getHeightCm() const { return heightCm_; }
    double getHeightM() const { return heightCm_ / 100.0; }
    int getAgeYears() const { return ageYears_; }
    Sex getSex() const { return sex_; }
    ActivityLevel getActivityLevel() const { return activity_; }
    ProfileContext getProfileContext() const { return context_; }

    const Interval& getWeightBoundsKg() const { return weightBoundsKg_; }
    const Interval& getHeightBoundsCm() const { return heightBoundsCm_; }
    Interval getHeightBoundsM() const {
        if (!heightBoundsCm_.isDefined()) return Interval();
        return Interval(heightBoundsCm_.getMin() / 100.0, heightBoundsCm_.getMax() / 100.0);
    }

    const std::string& getWeightSource() const { return weightSource_; }
    const std::string& getHeightSource() const { return heightSource_; }

    bool hasWeightBounds() const { return weightBoundsKg_.isDefined(); }
    bool hasHeightBounds() const { return heightBoundsCm_.isDefined(); }
    bool hasBothBounds() const { return hasWeightBounds() && hasHeightBounds(); }
    bool hasAnyBounds() const { return hasWeightBounds() || hasHeightBounds(); }

    // Setters
    void setRecordId(const std::string& id) { recordId_ = id; }
    void setProfileId(const std::string& id) { profileId_ = id; }
    void setTimestamp(const std::string& ts) { timestamp_ = ts; }
    void setWeightKg(double w) { weightKg_ = w; }
    void setHeightCm(double h) { heightCm_ = h; }
    void setAgeYears(int age) { ageYears_ = age; }
    void setSex(Sex s) { sex_ = s; }
    void setActivityLevel(ActivityLevel act) { activity_ = act; }
    void setProfileContext(ProfileContext ctx) { context_ = ctx; }

    void setWeightBoundsKg(double minKg, double maxKg, const std::string& source = "") {
        weightBoundsKg_ = Interval(minKg, maxKg);
        weightSource_ = source;
    }

    void setHeightBoundsCm(double minCm, double maxCm, const std::string& source = "") {
        heightBoundsCm_ = Interval(minCm, maxCm);
        heightSource_ = source;
    }
};

} // namespace fitmetric
