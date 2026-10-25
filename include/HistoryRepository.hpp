#pragma once

#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "EvidenceTrace.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace fitmetric {

struct ProfileRecord {
    std::string profileId;
    std::string displayName;
    std::string dateCreated;
    ProfileContext profileContext;
};

struct SavedSessionRecord {
    std::string sessionId;
    std::string profileId;
    std::string measurementDate;
    double weightKg;
    double heightCm;
    int ageYears;
    std::string equationSelection;
    std::string activityLevel;
    std::string athleteContext;
    bool hasBounds;
    double weightMin;
    double weightMax;
    double heightMin;
    double heightMax;
    std::string weightSource;
    std::string heightSource;
};

struct SavedResultRecord {
    std::string resultId;
    std::string sessionId;
    double bmiValue;
    double bmrValue;
    double tdeeValue;
    std::string bmiCategory;
    std::string formulaVersion;
    std::string calculationTimestamp;
    std::string traceId;
    std::string stabilityState;
    std::string decisionReason;
    std::string verificationSummary;
    std::string fullTraceJson;
};

class HistoryRepository {
private:
    std::string dbFilePath_;
    std::vector<ProfileRecord> profiles_;
    std::vector<SavedSessionRecord> sessions_;
    std::vector<SavedResultRecord> results_;

    void loadFromFile();
    void saveToFile();

public:
    explicit HistoryRepository(const std::string& dbFilePath = "data/fitmetric_history.json");

    void saveProfile(const ProfileRecord& profile);
    ProfileRecord getProfile(const std::string& profileId);

    void saveSessionAndResult(
        const MeasurementRecord& record,
        const std::string& sessionId,
        const std::string& resultId,
        double bmi, double bmr, double tdee,
        const std::string& bmiCat,
        const EvidenceTrace& trace);

    std::vector<SavedResultRecord> getHistoryByProfileId(const std::string& profileId) const;
    SavedResultRecord getResultById(const std::string& resultId) const;
    std::vector<SavedResultRecord> getAllResults() const;
    size_t getResultCount() const { return results_.size(); }
};

} // namespace fitmetric
