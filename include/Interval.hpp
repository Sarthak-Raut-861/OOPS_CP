#pragma once

#include "Common.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace fitmetric {

class Interval {
private:
    double minVal_;
    double maxVal_;
    bool defined_;

public:
    Interval() : minVal_(0.0), maxVal_(0.0), defined_(false) {}

    Interval(double minVal, double maxVal) : minVal_(minVal), maxVal_(maxVal), defined_(true) {
        if (minVal > maxVal) {
            throw ValidationException("Interval lower bound cannot exceed upper bound.");
        }
        if (minVal < 0.0 || maxVal < 0.0) {
            throw ValidationException("Interval bounds for physical measurements must be non-negative.");
        }
    }

    bool isDefined() const { return defined_; }
    double getMin() const { return minVal_; }
    double getMax() const { return maxVal_; }
    double getWidth() const { return defined_ ? (maxVal_ - minVal_) : 0.0; }
    double getMidpoint() const { return defined_ ? ((minVal_ + maxVal_) / 2.0) : 0.0; }

    bool contains(double val) const {
        if (!defined_) return false;
        return (val >= minVal_ && val <= maxVal_);
    }

    bool contains(const Interval& other) const {
        if (!defined_ || !other.defined_) return false;
        return (minVal_ <= other.minVal_ && maxVal_ >= other.maxVal_);
    }

    bool overlaps(const Interval& other) const {
        if (!defined_ || !other.defined_) return false;
        return !(maxVal_ < other.minVal_ || minVal_ > other.maxVal_);
    }

    // A threshold is crossed if the interval straddles it (strictly starts below and ends above or at threshold)
    bool crosses(double threshold) const {
        if (!defined_) return false;
        return (minVal_ < threshold && maxVal_ > threshold);
    }

    std::string toString(int precision = 2) const {
        if (!defined_) return "[undefined]";
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision)
            << "[" << minVal_ << ", " << maxVal_ << "]";
        return oss.str();
    }
};

} // namespace fitmetric
