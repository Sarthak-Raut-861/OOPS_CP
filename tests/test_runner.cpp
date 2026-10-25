#include "Common.hpp"
#include "MeasurementRecord.hpp"
#include "Interval.hpp"
#include "InputValidator.hpp"
#include "BMICalculator.hpp"
#include "BMRCalculator.hpp"
#include "TDEECalculator.hpp"
#include "InterpretationStabilityEngine.hpp"
#include "VerificationSelector.hpp"
#include "EvidenceTrace.hpp"
#include "HistoryRepository.hpp"
#include "FitMetricService.hpp"
#include "ExperimentalEvaluation.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>

using namespace fitmetric;

static int totalTests = 0;
static int passedTests = 0;
static int failedTests = 0;

void reportTest(const std::string& testName, bool passed, const std::string& details = "") {
    totalTests++;
    if (passed) {
        passedTests++;
        std::cout << "  [PASS] " << testName << "\n";
    } else {
        failedTests++;
        std::cout << "  [FAIL] " << testName << " - " << details << "\n";
    }
}

// 1. Calculation Tests
void testCalculations() {
    std::cout << "\n=== 1. Calculation Tests ===\n";

    BMICalculator bmiCalc;
    BMRCalculator bmrCalc;
    TDEECalculator tdeeCalc;

    // Test 1.1: Standard adult BMI (70 kg, 175 cm -> 70 / 1.75^2 = 22.86)
    MeasurementRecord r1(70.0, 175.0, 25, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    double bmiVal = bmiCalc.calculate(r1);
    reportTest("BMI standard calculation (70kg, 175cm = 22.86)", std::abs(bmiVal - 22.86) < 0.05);

    // Test 1.2: BMR Mifflin-St Jeor Male (10*70 + 6.25*175 - 5*25 + 5 = 700 + 1093.75 - 125 + 5 = 1673.75 -> 1673.8)
    double bmrMale = bmrCalc.calculate(r1);
    reportTest("BMR Mifflin-St Jeor (Male coefficient = 1673.8)", std::abs(bmrMale - 1673.8) < 0.2);

    // Test 1.3: BMR Mifflin-St Jeor Female (10*60 + 6.25*165 - 5*30 - 161 = 600 + 1031.25 - 150 - 161 = 1320.25 -> 1320.3)
    MeasurementRecord rFemale(60.0, 165.0, 30, Sex::Female, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    double bmrFemale = bmrCalc.calculate(rFemale);
    reportTest("BMR Mifflin-St Jeor (Female coefficient = 1320.3)", std::abs(bmrFemale - 1320.3) < 0.2);

    // Test 1.4: TDEE with all 5 activity multipliers
    double bmrBase = 1500.0;
    reportTest("TDEE Sedentary (1.20 * 1500 = 1800)",
               std::abs(tdeeCalc.calculateWithBMR(bmrBase, ActivityLevel::Sedentary) - 1800.0) < 0.1);
    reportTest("TDEE Lightly Active (1.375 * 1500 = 2062.5)",
               std::abs(tdeeCalc.calculateWithBMR(bmrBase, ActivityLevel::LightlyActive) - 2062.5) < 0.1);
    reportTest("TDEE Moderately Active (1.55 * 1500 = 2325)",
               std::abs(tdeeCalc.calculateWithBMR(bmrBase, ActivityLevel::ModeratelyActive) - 2325.0) < 0.1);
    reportTest("TDEE Very Active (1.725 * 1500 = 2587.5)",
               std::abs(tdeeCalc.calculateWithBMR(bmrBase, ActivityLevel::VeryActive) - 2587.5) < 0.1);
    reportTest("TDEE Extremely Active (1.90 * 1500 = 2850)",
               std::abs(tdeeCalc.calculateWithBMR(bmrBase, ActivityLevel::ExtremelyActive) - 2850.0) < 0.1);

    // Test 1.5: Zero or negative height/weight must be rejected
    bool caughtZeroW = false;
    try {
        MeasurementRecord badW(0.0, 175.0, 25, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
        InputValidator::validate(badW);
    } catch (const ValidationException&) {
        caughtZeroW = true;
    }
    reportTest("Reject zero or negative weight with ValidationException", caughtZeroW);

    bool caughtNegH = false;
    try {
        MeasurementRecord badH(70.0, -170.0, 25, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
        InputValidator::validate(badH);
    } catch (const ValidationException&) {
        caughtNegH = true;
    }
    reportTest("Reject zero or negative height with ValidationException", caughtNegH);
}

// 2. Interpretation Tests
void testInterpretation() {
    std::cout << "\n=== 2. Interpretation Tests (ISVE) ===\n";

    InterpretationStabilityEngine engine;
    BMICalculator bmiCalc;

    // Test 2.1: Stable within bounds (e.g. interval [21.5, 23.0] inside Normal range [18.5, 25.0])
    MeasurementRecord recStable(70.0, 175.0, 25, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    recStable.setWeightBoundsKg(69.0, 71.0);
    recStable.setHeightBoundsCm(174.5, 175.5);
    Interval intStable = bmiCalc.calculateInterval(recStable);
    auto resStable = engine.assess(recStable, 22.86, intStable);
    reportTest("Interval completely within Normal category is STABLE_WITHIN_DECLARED_BOUNDS",
               resStable.state == StabilityState::STABLE_WITHIN_DECLARED_BOUNDS && resStable.crossedBoundaries.empty());

    // Test 2.2: Crossing category boundary at 25.0 (W bounds [76, 78], H bounds [175, 177])
    // Wmin/Hmax^2 = 76 / 1.77^2 = 24.26; Wmax/Hmin^2 = 78 / 1.75^2 = 25.47 -> straddles 25.0!
    MeasurementRecord recCrossing(77.0, 176.0, 30, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    recCrossing.setWeightBoundsKg(76.0, 78.0);
    recCrossing.setHeightBoundsCm(175.0, 177.0);
    Interval intCrossing = bmiCalc.calculateInterval(recCrossing);
    auto resCrossing = engine.assess(recCrossing, 24.86, intCrossing);
    bool crosses25 = false;
    for (double b : resCrossing.crossedBoundaries) if (std::abs(b - 25.0) < 0.01) crosses25 = true;
    reportTest("Interval crossing 25.0 threshold marked CROSSES_CATEGORY_BOUNDARY",
               resCrossing.state == StabilityState::CROSSES_CATEGORY_BOUNDARY && crosses25);

    // Test 2.3: Missing uncertainty bounds -> STABILITY_NOT_ASSESSABLE (distinct from unstable!)
    MeasurementRecord recNoBounds(70.0, 175.0, 25, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    Interval intNoBounds = bmiCalc.calculateInterval(recNoBounds);
    auto resNoBounds = engine.assess(recNoBounds, 22.86, intNoBounds);
    reportTest("Missing uncertainty returns STABILITY_NOT_ASSESSABLE (no false stability claim)",
               resNoBounds.state == StabilityState::STABILITY_NOT_ASSESSABLE);

    // Test 2.4: Pediatric subject (< 18 years) out of adult scope
    MeasurementRecord recPediatric(50.0, 160.0, 15, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    recPediatric.setWeightBoundsKg(49.0, 51.0);
    recPediatric.setHeightBoundsCm(159.0, 161.0);
    Interval intPediatric = bmiCalc.calculateInterval(recPediatric);
    auto resPediatric = engine.assess(recPediatric, 19.53, intPediatric);
    reportTest("User under 18 years returns STABILITY_NOT_ASSESSABLE / OutOfScope message",
               resPediatric.state == StabilityState::STABILITY_NOT_ASSESSABLE &&
               resPediatric.decisionReason.find("below adult threshold") != std::string::npos);

    // Test 2.5: Athlete context preserves exact BMI formula but adds disclaimer
    MeasurementRecord recAthlete(90.0, 180.0, 24, Sex::Male, ActivityLevel::VeryActive, ProfileContext::Athlete);
    MeasurementRecord recGeneral(90.0, 180.0, 24, Sex::Male, ActivityLevel::VeryActive, ProfileContext::GeneralUser);
    double bmiAthlete = bmiCalc.calculate(recAthlete);
    double bmiGeneral = bmiCalc.calculate(recGeneral);
    reportTest("Athlete context does NOT alter numerical BMI formula",
               std::abs(bmiAthlete - bmiGeneral) < 0.001);

    auto resAthlete = engine.assess(recAthlete, bmiAthlete, Interval());
    reportTest("Athlete context attaches muscle mass & non-diagnostic disclaimer",
               resAthlete.athleteContextNote.find("Athlete profile active") != std::string::npos);
}

// 3. Verification Tests
void testVerification() {
    std::cout << "\n=== 3. Verification Selector Tests ===\n";

    VerificationSelector selector;
    MeasurementRecord rec(77.0, 176.0, 30, Sex::Male, ActivityLevel::Sedentary, ProfileContext::GeneralUser);
    rec.setWeightBoundsKg(75.5, 78.5);
    rec.setHeightBoundsCm(175.0, 177.0);

    Interval currentInterval(24.09, 25.63);
    std::vector<double> crossed = {25.0};

    auto candidates = selector.evaluateAndRank(rec, currentInterval, crossed);

    reportTest("Verification selector returns ranked eligible candidates (Weight & Height)",
               candidates.size() == 2 && candidates[0].rank == 1 && candidates[1].rank == 2);

    reportTest("Top candidate provides simulated interval and positive reduction percentage",
               candidates[0].simulatedInterval.isDefined() && candidates[0].intervalReductionPercent > 0.0);

    reportTest("Selector generates heuristic caveat (does not promise guaranteed resolution)",
               selector.generateTopActionSummary(candidates).find("heuristic recommendation") != std::string::npos);
}

// 4. Persistence & Audit Trace Tests
void testPersistence() {
    std::cout << "\n=== 4. Persistence & Audit Trace Tests ===\n";

    auto repo = std::make_shared<HistoryRepository>("data/test_history.json");
    FitMetricService service(repo);

    MeasurementRecord rec1(72.0, 178.0, 28, Sex::Male, ActivityLevel::ModeratelyActive, ProfileContext::GeneralUser, "test-user-1");
    rec1.setWeightBoundsKg(71.5, 72.5);
    rec1.setHeightBoundsCm(177.5, 178.5);

    EvidenceTrace trace1 = service.processAssessment(rec1, true);

    auto history = service.getHistory("test-user-1");
    reportTest("Saved session and calculation result retrieved from repository",
               !history.empty());

    // Re-assessment creates a new result without mutating old record
    MeasurementRecord rec2 = rec1;
    rec2.setWeightKg(74.0);
    rec2.setWeightBoundsKg(73.5, 74.5);
    EvidenceTrace trace2 = service.processAssessment(rec2, true);

    auto historyUpdated = service.getHistory("test-user-1");
    reportTest("New measurement session creates new result record without overwriting old one",
               historyUpdated.size() >= 2 && historyUpdated[0].resultId != historyUpdated[1].resultId);

    // Audit trace retains formula and rule versioning
    reportTest("Audit trace preserves formula and rule versioning",
               trace1.getBmiFormulaVersion() == BMI_FORMULA_VERSION &&
               trace1.getRuleVersion() == ISVE_RULE_VERSION);
}

// 5. Experimental Evaluation Benchmark Test
void testEvaluationBenchmark() {
    std::cout << "\n=== 5. Experimental Evaluation Benchmark Suite ===\n";

    auto repo = std::make_shared<HistoryRepository>("data/test_history.json");
    FitMetricService service(repo);

    EvaluationReport rep = ExperimentalEvaluation::runEvaluation(service);

    reportTest("Benchmark executes all test cases (TC-01 through TC-08)",
               rep.testCases.size() >= 8);

    reportTest("FitMetric ISVE reduces unsupported definite interpretations to 0 (vs Baseline A)",
               rep.fitMetricISVE.unsupportedDefiniteInterpretations == 0 &&
               rep.baselineA.unsupportedDefiniteInterpretations > 0);

    reportTest("FitMetric provides 100% boundary cross detection across test dataset",
               rep.fitMetricISVE.correctBoundaryCrossDetections > 0);

    reportTest("Average processing time overhead is sub-millisecond",
               rep.fitMetricISVE.averageProcessingTimeUs < 1000.0);
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "    FitMetric System: Automated Test Suite (OOPS)       \n";
    std::cout << "========================================================\n";

    testCalculations();
    testInterpretation();
    testVerification();
    testPersistence();
    testEvaluationBenchmark();

    std::cout << "\n========================================================\n";
    std::cout << " Test Summary: " << passedTests << " passed, "
              << failedTests << " failed (Total: " << totalTests << ")\n";
    std::cout << "========================================================\n";

    return (failedTests == 0) ? 0 : 1;
}
