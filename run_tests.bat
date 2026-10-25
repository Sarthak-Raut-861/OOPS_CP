@echo off
echo ========================================================
echo Running FitMetric Automated Acceptance and Unit Tests
echo ========================================================

if not exist bin\fitmetric_test.exe (
    echo Test binary not found. Building now...
    call build.bat
)

bin\fitmetric_test.exe
pause
