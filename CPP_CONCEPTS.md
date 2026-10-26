# FitMetric — C++ & Object-Oriented Programming (OOPS) Concepts Guide

This document is a dedicated reference listing all **C++ language features and Object-Oriented Programming (OOPS) concepts** implemented in the FitMetric codebase. Each concept includes its definition, rationale, code snippet, and the exact file where it is implemented.

---

## Table of Contents
1. [Core OOPS Concepts](#1-core-oops-concepts)
   - [1. Classes and Objects](#1-classes-and-objects)
   - [2. Encapsulation & Access Specifiers](#2-encapsulation--access-specifiers)
   - [3. Abstraction & Abstract Base Classes](#3-abstraction--abstract-base-classes)
   - [4. Inheritance Hierarchy](#4-inheritance-hierarchy)
   - [5. Runtime Polymorphism (Dynamic Binding)](#5-runtime-polymorphism-dynamic-binding)
   - [6. Compile-Time Polymorphism (Operator Overloading)](#6-compile-time-polymorphism-operator-overloading)
   - [7. Virtual Destructors](#7-virtual-destructors)
2. [Advanced C++ Language Features](#2-advanced-c-language-features)
   - [8. Const-Correctness](#8-const-correctness)
   - [9. Scoped Enumerations (`enum class`)](#9-scoped-enumerations-enum-class)
   - [10. Namespaces](#10-namespaces)
   - [11. Resource Acquisition Is Initialization (RAII)](#11-resource-acquisition-is-initialization-raii)
   - [12. Parameter Passing by Const Reference](#12-parameter-passing-by-const-reference)
3. [C++ Standard Template Library (STL)](#3-c-standard-template-library-stl)
   - [13. STL Containers (`std::vector`, `std::string`, `std::pair`)](#13-stl-containers)
   - [14. String Streams & Formatting](#14-string-streams--formatting)
   - [15. File Stream I/O (`std::ifstream`, `std::ofstream`)](#15-file-stream-io)
   - [16. Modern Chrono Library (`std::chrono`)](#16-modern-chrono-library)
   - [17. Standard Math Functions & Algorithms](#17-standard-math-functions--algorithms)
4. [Software Design Patterns Implemented in C++](#4-software-design-patterns-implemented-in-c)
5. [Viva Quick Reference Cheat Sheet](#5-viva-quick-reference-cheat-sheet)

---

## 1. Core OOPS Concepts

### 1. Classes and Objects
- **Definition:** User-defined types that package data attributes and member functions into cohesive entities.
- **Implementation:** Defined across the entire system. For example, `MeasurementRecord` models a patient's physical measurements and uncertainty margins as an instantiated object.
- **File Reference:** [`include/MeasurementRecord.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MeasurementRecord.hpp)
```cpp
MeasurementRecord record(77.0, 175.5, 30, Sex::Male, ActivityLevel::ModeratelyActive, ProfileContext::GeneralUser);
```

---

### 2. Encapsulation & Access Specifiers (`private` / `public`)
- **Definition:** Bundling data and functions while restricting direct outside access to internal state variables to protect data integrity.
- **Implementation:** Member variables use the `m_` or trailing underscore convention and are strictly `private`. Public getters provide read-only access, while setters enforce boundary validation rules.
- **File Reference:** [`include/MeasurementRecord.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MeasurementRecord.hpp#L10-L32)
```cpp
class MeasurementRecord {
private:
    double weightKg_;          // Private: Cannot be modified directly
    double heightCm_;
    int ageYears_;
    Interval weightBoundsKg_;

public:
    double getWeightKg() const { return weightKg_; } // Public read-only getter
    void setWeightBoundsKg(double minKg, double maxKg, const std::string& source);
};
```

---

### 3. Abstraction & Abstract Base Classes
- **Definition:** Hiding implementation details and exposing only essential interfaces. An abstract class cannot be instantiated on its own and declares at least one pure virtual function (`= 0`).
- **Implementation:** `MetricCalculator` serves as the abstract base class. It defines the calculation interface (`calculate()` and `calculateInterval()`) while leaving specific formula implementations to subclasses.
- **File Reference:** [`include/MetricCalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MetricCalculator.hpp#L10-L19)
```cpp
namespace fitmetric {

class MetricCalculator {
public:
    virtual ~MetricCalculator() = default;

    // Pure virtual methods define an abstract interface
    virtual double calculate(const MeasurementRecord& record) const = 0;
    virtual Interval calculateInterval(const MeasurementRecord& record) const = 0;
};

} // namespace fitmetric
```

---

### 4. Inheritance Hierarchy
- **Definition:** Deriving specialized classes from a general base class to reuse code, establish subtyping, and achieve code organization.
- **Implementation:** `BMICalculator`, `BMRCalculator`, and `TDEECalculator` publicly inherit from `MetricCalculator`.
- **File References:**
  - [`include/BMICalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/BMICalculator.hpp#L9)
  - [`include/BMRCalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/BMRCalculator.hpp#L9)
  - [`include/TDEECalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/TDEECalculator.hpp#L10)
```cpp
class BMICalculator : public MetricCalculator { ... };
class BMRCalculator : public MetricCalculator { ... };
class TDEECalculator : public MetricCalculator { ... };
```

---

### 5. Runtime Polymorphism (Dynamic Binding)
- **Definition:** Calling a virtual member function through a base class pointer or reference, resolving the call to the appropriate derived class method at runtime using virtual method tables (vtable).
- **Implementation:** The service invokes `calculate()` and `calculateInterval()` polymorphically on derived instances decorated with the `override` specifier.
- **File Reference:** [`include/BMICalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/BMICalculator.hpp#L12-L13)
```cpp
class BMICalculator : public MetricCalculator {
public:
    double calculate(const MeasurementRecord& record) const override;
    Interval calculateInterval(const MeasurementRecord& record) const override;
};
```

---

### 6. Compile-Time Polymorphism (Operator Overloading)
- **Definition:** Redefining standard C++ operators (`+`, `-`, `*`, `/`, `==`) to work natively on user-defined objects.
- **Implementation:** The `Interval` class overloads arithmetic operators to compute continuous error intervals naturally.
- **File Reference:** [`include/Interval.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/Interval.hpp#L30-L37)
```cpp
class Interval {
public:
    Interval operator+(const Interval& other) const;
    Interval operator-(const Interval& other) const;
    Interval operator*(const Interval& other) const;
    Interval operator/(const Interval& other) const;
    bool operator==(const Interval& other) const;
};
```
*Usage in calculations:*
```cpp
Interval heightMeters = heightCmInterval / Interval(100.0, 100.0);
Interval bmiInterval = weightKgInterval / (heightMeters * heightMeters);
```

---

### 7. Virtual Destructors
- **Definition:** Declaring a virtual destructor in a base class ensures that when a derived object is deleted through a base class pointer, the derived destructor is invoked first, preventing resource leaks.
- **File Reference:** [`include/MetricCalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MetricCalculator.hpp#L13)
```cpp
virtual ~MetricCalculator() = default;
```

---

## 2. Advanced C++ Language Features

### 8. Const-Correctness
- **Definition:** Applying the `const` keyword to variables, parameters, and member functions to guarantee immutability and prevent side-effects.
- **Implementation:**
  - Member functions that do not alter class state are marked `const`.
  - Parameters are received as `const` references (`const MeasurementRecord& record`).
- **File Reference:** [`include/InterpretationStabilityEngine.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/InterpretationStabilityEngine.hpp)
```cpp
StabilityCertificate evaluateStability(const Interval& metricInterval,
                                       const std::vector<RuleDefinition>& rules) const;
```

---

### 9. Scoped Enumerations (`enum class`)
- **Definition:** Strongly typed enumerations introduced in modern C++ that do not implicitly convert to integers and avoid global namespace pollution.
- **File Reference:** [`include/Common.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/Common.hpp#L9-L31)
```cpp
enum class Sex {
    Male,
    Female,
    Other
};

enum class ActivityLevel {
    Sedentary,
    LightlyActive,
    ModeratelyActive,
    VeryActive,
    ExtremelyActive
};

enum class StabilityState {
    STABLE_WITHIN_DECLARED_BOUNDS,
    CROSSES_CATEGORY_BOUNDARY,
    STABILITY_NOT_ASSESSABLE
};
```

---

### 10. Namespaces
- **Definition:** Scope-partitioning construct used to group related classes and functions while preventing naming collisions with other libraries or symbols.
- **File Reference:** Used across all headers:
```cpp
namespace fitmetric {
    // All classes, interfaces, and functions reside here
}
```

---

### 11. Resource Acquisition Is Initialization (RAII)
- **Definition:** A core C++ idiom where resource management (files, memory, locks) is tied to object lifetime. Resources are acquired during construction and released automatically during destruction.
- **Implementation:** File streams (`std::ofstream`, `std::ifstream`) open on construction and automatically flush and close when going out of scope, eliminating dangling file locks.
- **File Reference:** [`src/HistoryRepository.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/src/HistoryRepository.cpp#L48-L62)

---

### 12. Parameter Passing by Const Reference (`const Type&`)
- **Definition:** Passing complex objects by reference instead of by value, avoiding costly deep copies while protecting the original object from being modified.
- **File Reference:** Used across all service functions:
```cpp
EvidenceTrace processAssessment(const MeasurementRecord& record, bool persistToHistory);
```

---

## 3. C++ Standard Template Library (STL)

### 13. STL Containers
- `std::vector<T>`: Dynamic arrays used for storing history records, rule definitions, and verification recommendations.
  - File: [`include/RuleDefinition.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/RuleDefinition.hpp)
- `std::string`: Safe character sequence manipulation avoiding C-style string buffer overruns.
- `std::pair<T1, T2>`: Grouping paired values (e.g., metric label and value).

---

### 14. String Streams & Formatting
- **Implementation:** `std::stringstream`, `std::fixed`, and `std::setprecision` are used for JSON building and float formatting without locale discrepancies.
- **File Reference:** [`include/JsonHelper.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/JsonHelper.hpp#L20-L28)
```cpp
static std::string formatDouble(double val, int precision = 2) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << val;
    return ss.str();
}
```

---

### 15. File Stream I/O (`std::ifstream`, `std::ofstream`)
- **Implementation:** Reading and writing serialized JSON session history to disk (`data/fitmetric_history.json`).
- **File Reference:** [`src/HistoryRepository.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/src/HistoryRepository.cpp#L18-L38)
```cpp
std::ifstream inFile(filePath_);
if (inFile.is_open()) {
    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    // Parse JSON records...
}
```

---

### 16. Modern Chrono Library (`std::chrono`)
- **Implementation:** Using `std::chrono::high_resolution_clock` to benchmark engine execution speeds with sub-microsecond precision.
- **File Reference:** [`src/ExperimentalEvaluation.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/src/ExperimentalEvaluation.cpp#L116-L144)
```cpp
auto start = std::chrono::high_resolution_clock::now();
EvidenceTrace trace = service.processAssessment(rec, false);
auto end = std::chrono::high_resolution_clock::now();

double elapsedUs = std::chrono::duration<double, std::micro>(end - start).count();
```

---

### 17. Standard Math Functions & Algorithms
- `std::min` & `std::max`: Computing bound unions and intersections in interval calculus.
- `std::pow`: Calculating quadratic height terms ($H^2$) in [`src/BMICalculator.cpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/src/BMICalculator.cpp).

---

## 4. Software Design Patterns Implemented in C++

| Design Pattern | Implementation in FitMetric | File Reference |
| :--- | :--- | :--- |
| **Strategy Pattern** | `BMICalculator`, `BMRCalculator`, and `TDEECalculator` act as interchangeable calculation algorithms adhering to the `MetricCalculator` interface. | [`include/MetricCalculator.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/MetricCalculator.hpp) |
| **Repository Pattern** | `HistoryRepository` encapsulates data storage operations, isolating the rest of the application from file access and JSON parsing details. | [`include/HistoryRepository.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/HistoryRepository.hpp) |
| **Facade / Service Pattern** | `FitMetricService` provides a unified, high-level interface that orchestrates validation, calculation, stability checking, and history logging. | [`include/FitMetricService.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/FitMetricService.hpp) |
| **Value Object Pattern** | `Interval` is an immutable, self-validating value type that represents mathematical intervals without identity tracking. | [`include/Interval.hpp`](file:///c:/Users/Sarthak%20Raut/Desktop/OOPS%20CP/include/Interval.hpp) |

---

## 5. Viva Quick Reference Cheat Sheet

| Question | Short Answer | C++ Keyword / Mechanism |
| :--- | :--- | :--- |
| **Where is Encapsulation?** | Member variables are private in `MeasurementRecord` and `EvidenceTrace`; accessed via validated getters/setters. | `private`, `public` |
| **Where is an Abstract Class?** | `MetricCalculator` has pure virtual methods and cannot be instantiated. | `virtual ... = 0;` |
| **Where is Inheritance?** | `BMICalculator`, `BMRCalculator`, and `TDEECalculator` extend `MetricCalculator`. | `: public MetricCalculator` |
| **Where is Polymorphism?** | Dynamic runtime overriding via `override` + compile-time operator overloading. | `override`, `operator+`, `operator/` |
| **Where is Operator Overloading?** | In `Interval.hpp`, overloads `+`, `-`, `*`, `/`, `==` for range arithmetic. | `operator+`, `operator-`, etc. |
| **Why use Virtual Destructors?** | Ensures correct derived class destruction through base class pointers. | `virtual ~Base() = default;` |
| **Why use Scoped Enums?** | Prevents implicit integer conversion and global namespace contamination. | `enum class` |
| **Why pass by Const Reference?** | Avoids deep copying overhead while guaranteeing caller data cannot be mutated. | `const Type&` |
| **Where is RAII used?** | `std::ifstream` and `std::ofstream` auto-flush and auto-close upon leaving scope. | Object lifetime cleanup |
| **How is performance measured?** | High resolution clock in `std::chrono` measures execution in microseconds. | `std::chrono::high_resolution_clock` |

---

*FitMetric — Object-Oriented Programming (OOPS) Course Project Reference*
