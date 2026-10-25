#pragma once

#include "Common.hpp"
#include <string>
#include <vector>

namespace fitmetric {

struct BoundaryThreshold {
    double value;
    std::string lowerCategory;
    std::string upperCategory;
    std::string description;
};

class RuleDefinition {
private:
    std::string ruleId_;
    std::string version_;
    std::string authority_;
    int minimumAgeAdult_;
    std::vector<BoundaryThreshold> thresholds_;

public:
    RuleDefinition();

    const std::string& getRuleId() const { return ruleId_; }
    const std::string& getVersion() const { return version_; }
    const std::string& getAuthority() const { return authority_; }
    int getMinimumAgeAdult() const { return minimumAgeAdult_; }
    const std::vector<BoundaryThreshold>& getThresholds() const { return thresholds_; }

    std::vector<double> getCrossedBoundaries(double minVal, double maxVal) const;
};

} // namespace fitmetric
