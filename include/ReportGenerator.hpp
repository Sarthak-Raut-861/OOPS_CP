#pragma once

#include "Common.hpp"
#include "EvidenceTrace.hpp"
#include <string>

namespace fitmetric {

class ReportGenerator {
public:
    static std::string generateTextReport(const EvidenceTrace& trace);
    static std::string generateMarkdownReport(const EvidenceTrace& trace);
};

} // namespace fitmetric
