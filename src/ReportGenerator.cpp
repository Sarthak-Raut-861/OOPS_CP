#include "ReportGenerator.hpp"
#include <sstream>
#include <iomanip>

namespace fitmetric {

std::string ReportGenerator::generateMarkdownReport(const EvidenceTrace& trace) {
    std::ostringstream ss;
    ss << "# FitMetric Clinical & Metabolic Assessment Report\n\n";
    ss << "**Session Trace ID:** `" << trace.getTraceId() << "`  \n";
    ss << "**Assessment Timestamp:** " << trace.getTimestamp() << "  \n";
    ss << "**Profile Context:** " << profileContextToString(trace.getProfileContext()) << "  \n";
    ss << "**Governing Rule Version:** `" << trace.getRuleVersion() << "`  \n\n";

    ss << "## 1. Input Measurements & Declared Tolerances\n";
    ss << "- **Nominal Weight:** " << trace.getWeightKg() << " kg ";
    if (trace.getWeightBoundsKg().isDefined()) {
        ss << "(Supported Range: " << trace.getWeightBoundsKg().toString() << " kg, Source: " << trace.getWeightSource() << ")";
    } else {
        ss << "(Uncertainty: Not declared)";
    }
    ss << "\n";

    ss << "- **Nominal Height:** " << trace.getHeightCm() << " cm (" << std::fixed << std::setprecision(2) << (trace.getHeightCm() / 100.0) << " m) ";
    if (trace.getHeightBoundsCm().isDefined()) {
        ss << "(Supported Range: " << trace.getHeightBoundsCm().toString() << " cm, Source: " << trace.getHeightSource() << ")";
    } else {
        ss << "(Uncertainty: Not declared)";
    }
    ss << "\n";

    ss << "- **Age:** " << trace.getAgeYears() << " years | **Sex:** " << sexToString(trace.getSex())
       << " | **Activity Level:** " << activityLevelToString(trace.getActivityLevel()) << "\n\n";

    ss << "## 2. Calculated Metrics\n";
    ss << "| Metric | Calculated Value | Reference / Equation | Formula Version |\n";
    ss << "|---|---|---|---|\n";
    ss << "| **Body Mass Index (BMI)** | **" << trace.getNominalBmi() << " kg/m²** | "
       << bmiCategoryToString(trace.getNominalCategory()) << " | `" << trace.getBmiFormulaVersion() << "` |\n";
    ss << "| **Basal Metabolic Rate (BMR)** | **" << trace.getNominalBmr() << " kcal/day** | Mifflin–St Jeor ("
       << sexToString(trace.getSex()) << ") | `" << trace.getBmrFormulaVersion() << "` |\n";
    ss << "| **Total Daily Energy (TDEE)** | **" << trace.getNominalTdee() << " kcal/day** | PAL Factor Multiplier | `"
       << trace.getTdeeFormulaVersion() << "` |\n\n";

    ss << "## 3. Interpretation Stability & Verification Engine (ISVE) Verdict\n";
    ss << "- **Stability State:** `" << stabilityStateToString(trace.getStabilityState()) << "`\n";
    if (trace.getBmiInterval().isDefined()) {
        ss << "- **Supported BMI Interval:** " << trace.getBmiInterval().toString() << " kg/m² (Width: "
           << std::setprecision(2) << trace.getBmiInterval().getWidth() << ")\n";
    }
    if (!trace.getCrossedBoundaries().empty()) {
        ss << "- **Category Boundaries Crossed:** ";
        for (double b : trace.getCrossedBoundaries()) ss << b << " ";
        ss << "\n";
    }
    ss << "- **Engine Decision Rationale:** " << trace.getDecisionReason() << "\n\n";

    if (!trace.getVerificationCandidates().empty()) {
        ss << "### Recommended Measurement Verification Protocol\n";
        ss << "> " << trace.getVerificationSummary() << "\n\n";
        ss << "| Priority | Measurement | Target Tool & Precision | Interval Contraction | dBMI Sensitivity | Ambiguity Resolved? |\n";
        ss << "|---|---|---|---|---|---|\n";
        for (const auto& c : trace.getVerificationCandidates()) {
            ss << "| Rank " << c.rank << " | " << c.targetMeasurement << " | " << c.proposedTool
               << " | " << c.intervalReductionPercent << "% | " << c.sensitivity << " | "
               << (c.resolvesAmbiguity ? "Yes" : "Partial") << " |\n";
        }
        ss << "\n";
    }

    ss << "## 4. Contextual Disclaimers & Assumptions\n";
    ss << "- " << trace.getAthleteContextDisclaimer() << "\n";
    ss << "- BMR and TDEE are population-level statistical estimates and do not represent direct calorimetric measurements.\n";
    ss << "- FitMetric is an object-oriented clinical assessment prototype and does not provide formal medical diagnoses.\n";

    return ss.str();
}

std::string ReportGenerator::generateTextReport(const EvidenceTrace& trace) {
    return generateMarkdownReport(trace);
}

} // namespace fitmetric
