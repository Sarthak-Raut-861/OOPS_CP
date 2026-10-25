@echo off
echo ========================================================
echo Building FitMetric C++ Engine and Web Server
echo ========================================================

if not exist bin mkdir bin
if not exist data mkdir data

echo [1/2] Compiling FitMetric Server (fitmetric_server.exe)...
g++ -std=c++14 -O2 -Iinclude ^
    src/InputValidator.cpp ^
    src/BMICalculator.cpp ^
    src/BMRCalculator.cpp ^
    src/TDEECalculator.cpp ^
    src/RuleDefinition.cpp ^
    src/VerificationSelector.cpp ^
    src/EvidenceTrace.cpp ^
    src/InterpretationStabilityEngine.cpp ^
    src/HistoryRepository.cpp ^
    src/ReportGenerator.cpp ^
    src/FitMetricService.cpp ^
    src/ExperimentalEvaluation.cpp ^
    src/HttpServer.cpp ^
    src/main.cpp ^
    -lws2_32 -o bin/fitmetric_server.exe

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Server compilation failed!
    exit /b %ERRORLEVEL%
)
echo [SUCCESS] bin/fitmetric_server.exe built successfully.

echo [2/2] Compiling FitMetric Test Runner (fitmetric_test.exe)...
g++ -std=c++14 -O2 -Iinclude ^
    src/InputValidator.cpp ^
    src/BMICalculator.cpp ^
    src/BMRCalculator.cpp ^
    src/TDEECalculator.cpp ^
    src/RuleDefinition.cpp ^
    src/VerificationSelector.cpp ^
    src/EvidenceTrace.cpp ^
    src/InterpretationStabilityEngine.cpp ^
    src/HistoryRepository.cpp ^
    src/ReportGenerator.cpp ^
    src/FitMetricService.cpp ^
    src/ExperimentalEvaluation.cpp ^
    tests/test_runner.cpp ^
    -lws2_32 -o bin/fitmetric_test.exe

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Test runner compilation failed!
    exit /b %ERRORLEVEL%
)
echo [SUCCESS] bin/fitmetric_test.exe built successfully.
echo ========================================================
echo Build Complete!
