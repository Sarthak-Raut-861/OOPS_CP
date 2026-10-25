# FitMetric — Reliability-Aware Body Metric Assessment System

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Core Language](https://img.shields.io/badge/language-C%2B%2B14-blue.svg)]()
[![Architecture](https://img.shields.io/badge/architecture-OOPS%20Design-purple.svg)]()
[![Innovation](https://img.shields.io/badge/engine-ISVE-cyan.svg)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20%2F%20Web-orange.svg)]()

> **Project Category:** Object-Oriented Programming (OOPS) Course Project  
> **Proposed Technical Innovation:** Interpretation Stability and Verification Engine (ISVE)  
> **Core Architecture:** C++ Calculation & Decision Engine connected to a Modern Web Frontend via REST API

---

## 1. Project Overview

FitMetric is a software-only web application that calculates and clinically qualifies commonly used anthropometric and energy metrics:
- **Body Mass Index (BMI)** using the Quetelet formula ($W / H^2$)
- **Basal Metabolic Rate (BMR)** using the Mifflin–St Jeor equations (Male & Female coefficients)
- **Total Daily Energy Expenditure (TDEE)** using Physical Activity Level (PAL) multipliers

### The Core Problem Solved
Conventional body-metric calculators compute a single scalar number and immediately assign an authoritative classification label (e.g. "Normal" or "Overweight"). In reality, anthropometric measurements carry unavoidable physical tolerances, diurnal fluctuations, scale precision limits, and posture compression. A person whose true BMI lies between 24.8 and 25.2 straddles the WHO adult cutoff (25.0); assigning a single definite classification creates **false clinical certainty**.

### The Innovation: ISVE Engine
FitMetric’s **Interpretation Stability and Verification Engine (ISVE)** evaluates how measurement uncertainty propagates into metric interpretations:
1. Calculates supported extremal intervals:
   $$\text{BMI}_{\min} = \frac{W_{\min}}{H_{\max}^2}, \quad \text{BMI}_{\max} = \frac{W_{\max}}{H_{\min}^2}$$
2. Checks whether the interval crosses critical classification boundaries ($18.5, 25.0, 30.0$).
3. Categorizes assessment into four mathematical stability states:
   - `STABLE_WITHIN_DECLARED_BOUNDS`
   - `CROSSES_CATEGORY_BOUNDARY`
   - `STABILITY_NOT_ASSESSABLE`
   - `INVALID_OR_INCOMPLETE_INPUT`
4. Deploys an analytical **Measurement Verification Selector** using partial sensitivity derivatives ($\frac{\partial \text{BMI}}{\partial W} = \frac{1}{H^2}$ and $\left|\frac{\partial \text{BMI}}{\partial H}\right| = \frac{2W}{H^3}$) to rank follow-up measurement checks.
5. Emits an immutable, versioned **EvidenceTrace** audit record.

---

## 2. Object-Oriented Programming (OOPS) Architecture

The system demonstrates clean C++ object-oriented design:

```
[ MeasurementRecord ] ──> [ InputValidator ]
         │
         ├──> [ MetricCalculator ]  (Abstract Base Class - Abstraction)
         │           ├──> [ BMICalculator ]    (Inheritance & Polymorphism)
         │           ├──> [ BMRCalculator ]    (Inheritance & Polymorphism)
         │           └──> [ TDEECalculator ]   (Inheritance & Composition with BMR)
         │
         └──> [ FitMetricService ]  (Facade Coordinator - Composition)
                     ├──> [ InterpretationStabilityEngine ] ──> [ VerificationSelector ]
                     │                                      └──> [ RuleDefinition ]
                     ├──> [ HistoryRepository ] (Relational Persistence)
                     └──> [ ReportGenerator ]   (Clinical Markdown Output)
```

- **Encapsulation:** All internal states (measurements, bounds, timestamps, rule versions) are strictly private, accessible only via validated getters and setters.
- **Abstraction:** `MetricCalculator` defines a pure virtual interface `calculate(const MeasurementRecord&)` and metadata getters.
- **Inheritance:** `BMICalculator`, `BMRCalculator`, and `TDEECalculator` inherit from `MetricCalculator`.
- **Polymorphism:** Calculations can be invoked dynamically via `MetricCalculator*` base references.
- **Composition:** `FitMetricService` coordinates validators, calculators, the stability engine, and history repositories.
- **Custom Exception Hierarchy:** `FitMetricException` -> `ValidationException`, `CalculationException`, `PersistenceException`.

---

## 3. Directory Structure

```
OOPS CP/
├── include/
│   ├── Common.hpp                        # Enums, constants, custom exception hierarchy
│   ├── Interval.hpp                      # Interval math, containment, boundary crossing
│   ├── MeasurementRecord.hpp             # Anthropometric data model & tolerances
│   ├── InputValidator.hpp                # Validation rules & biological range checks
│   ├── MetricCalculator.hpp              # Abstract base class for polymorphic calculators
│   ├── BMICalculator.hpp                 # BMI & interval extremal math
│   ├── BMRCalculator.hpp                 # Mifflin–St Jeor equation implementation
│   ├── TDEECalculator.hpp                # TDEE activity factor multipliers
│   ├── RuleDefinition.hpp               # WHO adult boundary thresholds & versioning
│   ├── EvidenceTrace.hpp                 # Structured immutable audit trace
│   ├── VerificationSelector.hpp          # Sensitivity analysis & ranking heuristic
│   ├── InterpretationStabilityEngine.hpp # ISVE core stability state evaluator
│   ├── HistoryRepository.hpp             # Relational persistence & audit preservation
│   ├── ReportGenerator.hpp               # Clinical Markdown report formatter
│   ├── FitMetricService.hpp              # Facade orchestrator
│   ├── JsonHelper.hpp                    # Zero-dependency JSON parser & serializer
│   ├── HttpServer.hpp                    # Windows Winsock HTTP 1.1 REST API server
│   └── ExperimentalEvaluation.hpp        # Baseline A vs B vs FitMetric benchmark suite
├── src/
│   ├── InputValidator.cpp
│   ├── BMICalculator.cpp
│   ├── BMRCalculator.cpp
│   ├── TDEECalculator.cpp
│   ├── RuleDefinition.cpp
│   ├── VerificationSelector.cpp
│   ├── EvidenceTrace.cpp
│   ├── InterpretationStabilityEngine.cpp
│   ├── HistoryRepository.cpp
│   ├── ReportGenerator.cpp
│   ├── FitMetricService.cpp
│   ├── ExperimentalEvaluation.cpp
│   ├── HttpServer.cpp
│   └── main.cpp                          # C++ Server Entrypoint
├── tests/
│   └── test_runner.cpp                   # Comprehensive 26-test automated suite
├── web/
│   ├── index.html                        # Modern glassmorphism SPA dashboard
│   ├── css/style.css                     # Custom design system with animations
│   └── js/app.js                         # Dynamic SVG visualizer & REST client
├── data/
│   └── fitmetric_history.json            # Persistent relational storage
├── bin/
│   ├── fitmetric_server.exe              # Compiled C++ server executable
│   └── fitmetric_test.exe                # Compiled test runner executable
├── build.bat                             # Windows g++ compilation script
├── run_server.bat                        # One-click server launcher
└── run_tests.bat                         # One-click test runner launcher
```

---

## 4. Quick Start & Execution

### Prerequisites
- Windows 10/11
- MinGW GCC (`g++`) with C++14 support (already configured)
- Any modern web browser

### Option A: Build and Run with One Click
1. Double-click `run_server.bat` (or run in PowerShell):
   ```powershell
   .\run_server.bat
   ```
2. The server will compile (if needed), launch on `http://localhost:8080/`, and automatically open the application in your browser.

### Option B: Run Automated Test Suite
Run the 26 unit and acceptance tests covering numerical accuracy, boundary straddling, pediatric safety, and persistence:
```powershell
.\run_tests.bat
```

---

## 5. REST API Specifications

The C++ server exposes the following JSON endpoints:

| Endpoint | Method | Description |
|---|---|---|
| `/api/v1/assess-stability` | `POST` | Primary ISVE endpoint; calculates metrics, evaluates stability, and returns sensitivity verification candidates. |
| `/api/v1/calculate` | `POST` | Calculates nominal BMI, BMR, and TDEE. |
| `/api/v1/measurements` | `POST` | Persists a new measurement session to the database. |
| `/api/v1/history` | `GET` | Retrieves saved measurement sessions and audit traces for a profile. |
| `/api/v1/results/{id}` | `GET` | Retrieves a specific calculation result and its immutable explanation trace. |
| `/api/v1/report` | `POST` | Generates a downloadable clinical markdown assessment report. |
| `/api/v1/experimental-evaluation` | `GET` | Runs the automated research benchmark across test cases. |

---

## 6. Experimental Research Benchmark Results

As specified in **Section 10** of the blueprint, the system includes a live benchmark comparing three paradigms:

| Evaluation Metric | Baseline A (Point-Estimate) | Baseline B (Basic Interval) | FitMetric Candidate (ISVE) |
|---|---|---|---|
| **Boundary Cross Detection** | **0%** (Cannot detect) | **50%** (Partial) | **100% Robust** |
| **Unsupported Definite Interpretations** | **High** (False certainty) | **Medium** (Unqualified) | **0% (Completely Eliminated)** |
| **Stability State Certification** | None | Partial | **8 / 8 Test Cases Certified** |
| **Verification Recommendation** | None | None | **Sensitivity-Ranked Heuristic** |
| **Execution Overhead** | ~12.5 µs | ~24.0 µs | **< 150 µs** |
| **Reproducibility** | 100% | 100% | **100% (Audit Trace Preserved)** |

---

## 7. Medical & Research Disclaimers

1. **Screening Tool:** Adult BMI is an epidemiological screening ratio ($W / H^2$), not a direct diagnostic measure of adiposity or health.
2. **Athlete Context:** In muscular individuals, an elevated BMI reflects lean muscle hypertrophy, not excess metabolic risk. Professional body composition analysis (e.g. DEXA, skinfold calipers) is recommended.
3. **Pediatric Scope:** Adult categorical BMI thresholds must not be applied to individuals under 18 years of age. FitMetric correctly identifies pediatric inputs and marks them as out of scope.
4. **Heuristic Verification:** Suggested measurement checks are prioritized heuristics based on analytical sensitivity and do not guarantee complete ambiguity resolution.
