@echo off
echo ========================================================
echo Starting FitMetric C++ REST API and Web Server
echo ========================================================

if not exist bin\fitmetric_server.exe (
    echo Binary not found. Building now...
    call build.bat
)

echo Launching FitMetric Server on http://localhost:8080/
start http://localhost:8080/
bin\fitmetric_server.exe 8080
