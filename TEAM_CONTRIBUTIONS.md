# FitMetric — Team Workload & Contribution Breakdown (4 Members)

This document provides a balanced and equitable division of responsibilities for a **4-member project team**, featuring **1 Research & Documentation Lead (Non-Coding)** and **3 Developers (Equally Balanced Coding Workload)**.

---

## Team Overview & Work Distribution

```
+---------------------------------------------------------------------------------------------------------+
| Member 1: Research, Clinical Domain & Documentation Lead    [NON-CODING / RESEARCH & REQUIREMENTS]      |
| Member 2: Core OOPS Architecture & Mathematical Calculators [C++ DEVELOPER 1: CALCULATION ENGINE]      |
| Member 3: ISVE Stability Engine & Clinical Decision Logic   [C++ DEVELOPER 2: VERIFICATION ALGORITHMS]  |
| Member 4: Systems Networking, Persistence & Web Interface   [C++ DEVELOPER 3: SYSTEMS & FULL-STACK]     |
+---------------------------------------------------------------------------------------------------------+
```

---

## 👤 Member 1: Research, Clinical Domain & Documentation Lead
**Role:** Non-Coding / Academic Research, Requirements Engineering & Documentation Architect

### Viva Pitch:
> *"I led the medical literature review, clinical requirements engineering, experimental benchmark design, and project documentation. I researched WHO and CDC clinical guidelines, identified why conventional calculators produce false certainty, formulated the 8 standardized clinical test scenarios, and defined the sensitivity-reduction verification protocols."*

### Key Responsibilities & Deliverables:
1. **Clinical Literature Review & Standards:**
   - Researched World Health Organization (WHO) BMI cutoff thresholds ($18.5, 25.0, 30.0, 35.0, 40.0$).
   - Researched CDC/WHO pediatric guidelines to establish why adult BMI cutoffs are clinically invalid for patients under 18.
   - Evaluated Mifflin-St Jeor vs. Harris-Benedict BMR formulas to select the most clinically accurate metabolic equation.
2. **Formulated the Core Research Problem (False Certainty):**
   - Researched real-world measurement error tolerances: bathroom scale sensor noise ($\pm 0.5\text{ kg}$ to $\pm 1.5\text{ kg}$) and spinal diurnal height fluctuations ($\pm 1.0\text{ cm}$ to $\pm 2.0\text{ cm}$).
   - Demonstrated mathematically why a patient with nominal BMI $25.01$ is often falsely diagnosed as "Overweight" due to sensor noise.
3. **Designed the 8 Experimental Test Scenarios (`TC-01` to `TC-08`):**
   - Designed the exact patient profiles, error bounds, and boundary-straddling scenarios used in the benchmark suite ([`src/ExperimentalEvaluation.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/src/ExperimentalEvaluation.cpp)).
4. **Formulated Verification Protocols:**
   - Researched clinical verification tools (wall-mounted stadiometers, medical beam scales) and established their variance-reduction percentages (75% for weight, 85% for height).
5. **Authored Complete Documentation Suite:**
   - Authored all technical guides and user manuals: [`README.md`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/README.md), [`PROJECT_EXPLANATION.md`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/PROJECT_EXPLANATION.md), [`WEBSITE_FEATURES_GUIDE.md`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/WEBSITE_FEATURES_GUIDE.md), and [`ISVE_CORE_EXPLANATION.md`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/ISVE_CORE_EXPLANATION.md).

---

## 👤 Member 2: Core OOPS Architecture & Mathematical Calculators
**Role:** C++ Developer 1 — Core OOPS Engine & Calculation Pipeline

### Viva Pitch:
> *"I was responsible for designing the core object-oriented class hierarchy, abstract interfaces, concrete metric calculators, interval operator overloading, and input validation preconditions."*

### Key Responsibilities & Deliverables:
1. **Abstract Base Class Architecture:**
   - Designed [`MetricCalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MetricCalculator.hpp) with pure virtual functions (`calculate()`, `calculateInterval()`) and a virtual destructor.
2. **Concrete Metric Calculators:**
   - [`BMICalculator.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/BMICalculator.hpp): Quetelet index calculation and interval bounds.
   - [`BMRCalculator.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/BMRCalculator.hpp): Gender-specific Mifflin-St Jeor formula implementation.
   - [`TDEECalculator.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/TDEECalculator.hpp): Activity level multiplier matrix ($1.2$ to $1.9$).
3. **Compile-Time Operator Overloading:**
   - Implemented overloaded operators (`+`, `-`, `*`, `/`, `==`) on the [`Interval`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/Interval.hpp) class to perform continuous interval calculus.
4. **Input Validation & Sanity Constraints:**
   - Implemented [`InputValidator.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/InputValidator.hpp) to reject out-of-range physical inputs (e.g., negative weight, zero height).
5. **Testing Contribution:** Authored Unit Tests 1–10 in [`tests/test_runner.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/tests/test_runner.cpp) validating calculation precision and operator overloading.

### Key OOPS Concepts:
- Abstract Classes & Pure Virtual Functions (`= 0`)
- Inheritance Hierarchy (`: public MetricCalculator`)
- Runtime Polymorphism (`override`)
- Compile-Time Polymorphism (Operator Overloading)
- Strategy Pattern

---

## 👤 Member 3: ISVE Stability Engine & Clinical Decision Logic
**Role:** C++ Developer 2 — Decision Algorithms, Clinical Guardrails & Performance Benchmarking

### Viva Pitch:
> *"I developed the ISVE stability certification algorithm, the sensitivity-ranked verification selector, the pediatric guardrail intercept, and the high-resolution chrono benchmark suite."*

### Key Responsibilities & Deliverables:
1. **Boundary-Crossing Detection Algorithm:**
   - Implemented [`InterpretationStabilityEngine.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/InterpretationStabilityEngine.hpp) to evaluate interval intersections against WHO category thresholds.
2. **Sensitivity Analysis & Action Generator:**
   - Implemented [`VerificationSelector.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/VerificationSelector.hpp) to calculate partial variance contributions and rank corrective actions (scale calibration vs. stadiometer).
3. **Rule Modeling & Pediatric Guardrail:**
   - Implemented [`RuleDefinition.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/RuleDefinition.hpp) and pediatric safety logic that blocks adult WHO rules for patients under 18.
4. **Encapsulated Audit Trace:**
   - Implemented [`EvidenceTrace.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/EvidenceTrace.hpp) to encapsulate metric intervals, certificates, and clinical warnings into an immutable report.
5. **Chrono Benchmark Suite & Testing:**
   - Built [`ExperimentalEvaluation.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/ExperimentalEvaluation.hpp) using `std::chrono::high_resolution_clock` to benchmark microsecond execution speeds ($148\text{ }\mu\text{s}$) across Baseline A, Baseline B, and FitMetric.
   - Authored Unit Tests 11–20 in [`tests/test_runner.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/tests/test_runner.cpp) validating stability states and pediatric guardrails.

### Key OOPS Concepts:
- Encapsulation & Data Protection
- Scoped Enumerations (`enum class StabilityState`)
- Value Object Pattern
- Algorithmic Decision Trees & Sensitivity Analysis

---

## 👤 Member 4: Systems Networking, Persistence & Web Interface
**Role:** C++ Developer 3 — Systems Programming, Database Persistence & Full-Stack Interface

### Viva Pitch:
> *"I developed the custom C++ Windows Sockets HTTP microservice, the file-backed JSON database persistence layer, and the responsive web dashboard."*

### Key Responsibilities & Deliverables:
1. **Embedded Windows Sockets HTTP Server:**
   - Built [`HttpServer.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/HttpServer.hpp) from scratch using native Windows Sockets 2 (`ws2_32.lib`) (`WSAStartup`, socket creation, binding, listening, and client connection loop).
   - Implemented an HTTP/1.1 protocol parser supporting `GET`, `POST`, `OPTIONS`, and TCP `Content-Length` tracking across packet fragments.
2. **Persistence & Custom JSON Engine:**
   - Implemented the Repository Pattern in [`HistoryRepository.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/HistoryRepository.hpp) to save and load session history from `data/fitmetric_history.json`.
   - Built the zero-dependency JSON builder and parser in [`JsonHelper.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/JsonHelper.hpp) using string manipulation and `std::stringstream`.
3. **Facade Service Orchestrator:**
   - Built [`FitMetricService.hpp/.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/FitMetricService.hpp) linking the HTTP server endpoints to calculations, stability checks, and persistence.
4. **Web Frontend Dashboard:**
   - Built the frontend client (`web/index.html`, `web/css/style.css`, `web/js/app.js`) with responsive 2-column layout, dynamic status badge rendering, and asynchronous `fetch()` API calls.
5. **Testing Contribution:** Authored Unit Tests 21–26 in [`tests/test_runner.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/tests/test_runner.cpp) validating JSON serialization, deserialization, and persistence file round-trips.

### Key OOPS Concepts:
- Repository Pattern
- Facade / Service Pattern
- Resource Acquisition Is Initialization (RAII) with file streams
- Socket Programming & Separation of Concerns

---

## 📊 Summary Workload Distribution Table

| Member | Focus Domain | Files Owned | Key OOPS / Engineering Concepts |
| :--- | :--- | :--- | :--- |
| **Member 1** *(Research / Non-Coding)* | **Clinical Research, Requirements & Documentation** | `README.md`, `PROJECT_EXPLANATION.md`, `ISVE_CORE_EXPLANATION.md`, `WEBSITE_FEATURES_GUIDE.md` | False Certainty Analysis, WHO/CDC Guidelines, Benchmark Test Design (`TC-01` to `TC-08`), Variance Reduction Rates |
| **Member 2** *(C++ Dev 1)* | **Core OOPS Architecture & Calculators** | `MetricCalculator.hpp`, `BMICalculator.cpp`, `BMRCalculator.cpp`, `TDEECalculator.cpp`, `Interval.hpp`, `InputValidator.cpp` | Abstract Base Classes, Pure Virtual Functions, Inheritance, Operator Overloading, Strategy Pattern |
| **Member 3** *(C++ Dev 2)* | **ISVE Stability Engine & Clinical Decision Logic** | `InterpretationStabilityEngine.cpp`, `VerificationSelector.cpp`, `RuleDefinition.cpp`, `EvidenceTrace.cpp`, `ExperimentalEvaluation.cpp` | Encapsulation, Scoped Enums, Value Objects, Sensitivity Analysis, `std::chrono` Benchmarking |
| **Member 4** *(C++ Dev 3)* | **Systems Networking, Persistence & UI** | `HttpServer.cpp`, `HistoryRepository.cpp`, `JsonHelper.hpp`, `FitMetricService.cpp`, `web/` assets (`html`, `css`, `js`) | Repository Pattern, Facade Pattern, RAII File Streams, Winsock2 Socket Programming, Asynchronous REST APIs |

---

*FitMetric — Equal 4-Member Workload Distribution Matrix*
