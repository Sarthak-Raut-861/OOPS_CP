#include "HistoryRepository.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace fitmetric {

HistoryRepository::HistoryRepository(const std::string& dbFilePath)
    : dbFilePath_(dbFilePath) {
    loadFromFile();
    if (profiles_.empty()) {
        ProfileRecord defaultProfile;
        defaultProfile.profileId = "profile-default";
        defaultProfile.displayName = "Alex Morgan";
        defaultProfile.dateCreated = "2026-10-25 10:00:00";
        defaultProfile.profileContext = ProfileContext::GeneralUser;
        profiles_.push_back(defaultProfile);

        ProfileRecord athleteProfile;
        athleteProfile.profileId = "profile-athlete";
        athleteProfile.displayName = "Jordan Vance (Track Athlete)";
        athleteProfile.dateCreated = "2026-10-25 10:00:00";
        athleteProfile.profileContext = ProfileContext::Athlete;
        profiles_.push_back(athleteProfile);
        saveToFile();
    }
}

void HistoryRepository::saveProfile(const ProfileRecord& profile) {
    for (auto& p : profiles_) {
        if (p.profileId == profile.profileId) {
            p = profile;
            saveToFile();
            return;
        }
    }
    profiles_.push_back(profile);
    saveToFile();
}

ProfileRecord HistoryRepository::getProfile(const std::string& profileId) {
    for (const auto& p : profiles_) {
        if (p.profileId == profileId) {
            return p;
        }
    }
    ProfileRecord fallback;
    fallback.profileId = profileId;
    fallback.displayName = "User (" + profileId + ")";
    fallback.dateCreated = "2026-10-25 10:00:00";
    fallback.profileContext = ProfileContext::GeneralUser;
    return fallback;
}

void HistoryRepository::saveSessionAndResult(
    const MeasurementRecord& record,
    const std::string& sessionId,
    const std::string& resultId,
    double bmi, double bmr, double tdee,
    const std::string& bmiCat,
    const EvidenceTrace& trace) {

    SavedSessionRecord sess;
    sess.sessionId = sessionId;
    sess.profileId = record.getProfileId().empty() ? "profile-default" : record.getProfileId();
    sess.measurementDate = record.getTimestamp();
    sess.weightKg = record.getWeightKg();
    sess.heightCm = record.getHeightCm();
    sess.ageYears = record.getAgeYears();
    sess.equationSelection = BMR_FORMULA_VERSION;
    sess.activityLevel = activityLevelToString(record.getActivityLevel());
    sess.athleteContext = profileContextToString(record.getProfileContext());
    sess.hasBounds = record.hasAnyBounds();
    sess.weightMin = record.hasWeightBounds() ? record.getWeightBoundsKg().getMin() : record.getWeightKg();
    sess.weightMax = record.hasWeightBounds() ? record.getWeightBoundsKg().getMax() : record.getWeightKg();
    sess.heightMin = record.hasHeightBounds() ? record.getHeightBoundsCm().getMin() : record.getHeightCm();
    sess.heightMax = record.hasHeightBounds() ? record.getHeightBoundsCm().getMax() : record.getHeightCm();
    sess.weightSource = record.getWeightSource();
    sess.heightSource = record.getHeightSource();

    sessions_.push_back(sess);

    SavedResultRecord res;
    res.resultId = resultId;
    res.sessionId = sessionId;
    res.bmiValue = bmi;
    res.bmrValue = bmr;
    res.tdeeValue = tdee;
    res.bmiCategory = bmiCat;
    res.formulaVersion = BMI_FORMULA_VERSION;
    res.calculationTimestamp = trace.getTimestamp();
    res.traceId = trace.getTraceId();
    res.stabilityState = stabilityStateToString(trace.getStabilityState());
    res.decisionReason = trace.getDecisionReason();
    res.verificationSummary = trace.getVerificationSummary();
    res.fullTraceJson = trace.toJsonString();

    results_.push_back(res);
    saveToFile();
}

std::vector<SavedResultRecord> HistoryRepository::getHistoryByProfileId(const std::string& profileId) const {
    std::vector<SavedResultRecord> filtered;
    for (const auto& res : results_) {
        // Find session
        for (const auto& sess : sessions_) {
            if (sess.sessionId == res.sessionId && (profileId.empty() || sess.profileId == profileId)) {
                filtered.push_back(res);
                break;
            }
        }
    }
    return filtered;
}

SavedResultRecord HistoryRepository::getResultById(const std::string& resultId) const {
    for (const auto& r : results_) {
        if (r.resultId == resultId) {
            return r;
        }
    }
    throw PersistenceException("Result record not found for resultId: " + resultId);
}

std::vector<SavedResultRecord> HistoryRepository::getAllResults() const {
    return results_;
}

void HistoryRepository::loadFromFile() {
    std::ifstream in(dbFilePath_.c_str());
    if (!in.is_open()) return;

    // Read full content
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    // If file has content, parse records (minimalist line-based persistence serializer for robustness)
    // We will save in clean delimited format or parse serialized structure
    std::istringstream stream(content);
    std::string line;
    std::string mode = "";
    while (std::getline(stream, line)) {
        if (line == "===PROFILES===") { mode = "PROFILES"; continue; }
        if (line == "===SESSIONS===") { mode = "SESSIONS"; continue; }
        if (line == "===RESULTS===") { mode = "RESULTS"; continue; }
        if (line.empty() || line[0] == '#') continue;

        if (mode == "PROFILES") {
            std::stringstream ss(line);
            std::string id, name, date, ctxStr;
            if (std::getline(ss, id, '\t') && std::getline(ss, name, '\t') &&
                std::getline(ss, date, '\t') && std::getline(ss, ctxStr, '\t')) {
                ProfileRecord p;
                p.profileId = id;
                p.displayName = name;
                p.dateCreated = date;
                p.profileContext = (ctxStr == "Athlete") ? ProfileContext::Athlete : ProfileContext::GeneralUser;
                profiles_.push_back(p);
            }
        } else if (mode == "SESSIONS") {
            std::stringstream ss(line);
            SavedSessionRecord s;
            std::string hasB;
            if (std::getline(ss, s.sessionId, '\t') &&
                std::getline(ss, s.profileId, '\t') &&
                std::getline(ss, s.measurementDate, '\t') &&
                ss >> s.weightKg && ss.ignore(1) &&
                ss >> s.heightCm && ss.ignore(1) &&
                ss >> s.ageYears && ss.ignore(1) &&
                std::getline(ss, s.equationSelection, '\t') &&
                std::getline(ss, s.activityLevel, '\t') &&
                std::getline(ss, s.athleteContext, '\t') &&
                std::getline(ss, hasB, '\t') &&
                ss >> s.weightMin && ss.ignore(1) &&
                ss >> s.weightMax && ss.ignore(1) &&
                ss >> s.heightMin && ss.ignore(1) &&
                ss >> s.heightMax && ss.ignore(1) &&
                std::getline(ss, s.weightSource, '\t') &&
                std::getline(ss, s.heightSource, '\t')) {
                s.hasBounds = (hasB == "1" || hasB == "true");
                sessions_.push_back(s);
            }
        } else if (mode == "RESULTS") {
            std::stringstream ss(line);
            SavedResultRecord r;
            std::string traceEncoded;
            if (std::getline(ss, r.resultId, '\t') &&
                std::getline(ss, r.sessionId, '\t') &&
                ss >> r.bmiValue && ss.ignore(1) &&
                ss >> r.bmrValue && ss.ignore(1) &&
                ss >> r.tdeeValue && ss.ignore(1) &&
                std::getline(ss, r.bmiCategory, '\t') &&
                std::getline(ss, r.formulaVersion, '\t') &&
                std::getline(ss, r.calculationTimestamp, '\t') &&
                std::getline(ss, r.traceId, '\t') &&
                std::getline(ss, r.stabilityState, '\t') &&
                std::getline(ss, r.decisionReason, '\t') &&
                std::getline(ss, r.verificationSummary, '\t') &&
                std::getline(ss, traceEncoded)) {
                // Decode simple newlines if stored
                for (size_t i = 0; i < traceEncoded.size(); ++i) {
                    if (traceEncoded[i] == '\\' && i + 1 < traceEncoded.size() && traceEncoded[i+1] == 'n') {
                        r.fullTraceJson += '\n';
                        ++i;
                    } else {
                        r.fullTraceJson += traceEncoded[i];
                    }
                }
                results_.push_back(r);
            }
        }
    }
}

void HistoryRepository::saveToFile() {
    std::ofstream out(dbFilePath_.c_str());
    if (!out.is_open()) return;

    out << "===PROFILES===\n";
    for (const auto& p : profiles_) {
        out << p.profileId << "\t" << p.displayName << "\t" << p.dateCreated << "\t"
            << profileContextToString(p.profileContext) << "\n";
    }

    out << "===SESSIONS===\n";
    for (const auto& s : sessions_) {
        out << s.sessionId << "\t" << s.profileId << "\t" << s.measurementDate << "\t"
            << s.weightKg << "\t" << s.heightCm << "\t" << s.ageYears << "\t"
            << s.equationSelection << "\t" << s.activityLevel << "\t"
            << s.athleteContext << "\t" << (s.hasBounds ? "1" : "0") << "\t"
            << s.weightMin << "\t" << s.weightMax << "\t"
            << s.heightMin << "\t" << s.heightMax << "\t"
            << s.weightSource << "\t" << s.heightSource << "\n";
    }

    out << "===RESULTS===\n";
    for (const auto& r : results_) {
        std::string traceSingleLine;
        for (char c : r.fullTraceJson) {
            if (c == '\n') traceSingleLine += "\\n";
            else if (c == '\r') continue;
            else if (c == '\t') traceSingleLine += ' ';
            else traceSingleLine += c;
        }
        out << r.resultId << "\t" << r.sessionId << "\t"
            << r.bmiValue << "\t" << r.bmrValue << "\t" << r.tdeeValue << "\t"
            << r.bmiCategory << "\t" << r.formulaVersion << "\t"
            << r.calculationTimestamp << "\t" << r.traceId << "\t"
            << r.stabilityState << "\t" << r.decisionReason << "\t"
            << r.verificationSummary << "\t" << traceSingleLine << "\n";
    }
    out.close();
}

} // namespace fitmetric
