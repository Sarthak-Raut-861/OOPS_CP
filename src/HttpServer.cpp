#include "HttpServer.hpp"
#include "JsonHelper.hpp"
#include "ExperimentalEvaluation.hpp"
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace fitmetric {

HttpServer::HttpServer(int port, const std::string& webRoot, std::shared_ptr<FitMetricService> service)
    : port_(port), webRoot_(webRoot), service_(service), isRunning_(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::stop() {
    isRunning_ = false;
}

std::string HttpServer::getMimeType(const std::string& filepath) {
    if (filepath.find(".html") != std::string::npos || filepath.find(".htm") != std::string::npos) return "text/html; charset=utf-8";
    if (filepath.find(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (filepath.find(".js") != std::string::npos) return "application/javascript; charset=utf-8";
    if (filepath.find(".json") != std::string::npos) return "application/json; charset=utf-8";
    if (filepath.find(".svg") != std::string::npos) return "image/svg+xml";
    if (filepath.find(".png") != std::string::npos) return "image/png";
    if (filepath.find(".jpg") != std::string::npos || filepath.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (filepath.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain; charset=utf-8";
}

std::string HttpServer::handleStaticFile(const std::string& rawPath) {
    std::string path = rawPath;
    size_t qPos = path.find('?');
    if (qPos != std::string::npos) {
        path = path.substr(0, qPos);
    }
    if (path == "/" || path == "") {
        path = "/index.html";
    }

    // Sanitize path against directory traversal
    if (path.find("..") != std::string::npos) {
        return "HTTP/1.1 403 Forbidden\r\nContent-Length: 9\r\n\r\nForbidden";
    }

    std::string fullPath = webRoot_ + path;
    std::ifstream file(fullPath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        // Fallback to index.html for SPA routes
        fullPath = webRoot_ + "/index.html";
        file.open(fullPath.c_str(), std::ios::binary);
        if (!file.is_open()) {
            std::string notFound = "{\"error\": \"File not found\"}";
            return "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: " +
                   std::to_string(notFound.size()) + "\r\n\r\n" + notFound;
        }
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::string mime = getMimeType(path);

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << mime << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n\r\n";
    response << content;
    return response.str();
}

std::string HttpServer::handleApiRequest(const std::string& method, const std::string& path, const std::string& body) {
    std::string cleanPath = path;
    size_t qPos = cleanPath.find('?');
    std::string queryStr = "";
    if (qPos != std::string::npos) {
        queryStr = cleanPath.substr(qPos + 1);
        cleanPath = cleanPath.substr(0, qPos);
    }

    if (method == "OPTIONS") {
        return "HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
               "Access-Control-Allow-Headers: Content-Type\r\n"
               "Content-Length: 0\r\n\r\n";
    }

    try {
        // Endpoint 1: POST /api/v1/calculate or POST /api/v1/assess-stability
        if (method == "POST" && (cleanPath == "/api/v1/calculate" || cleanPath == "/api/v1/assess-stability" || cleanPath == "/api/v1/measurements")) {
            double weight = JsonHelper::getDouble(body, "weightKg", 0.0);
            double height = JsonHelper::getDouble(body, "heightCm", 0.0);
            int age = JsonHelper::getInt(body, "ageYears", 0);
            std::string sexStr = JsonHelper::getString(body, "sex", "male");
            std::string actStr = JsonHelper::getString(body, "activityLevel", "sedentary");
            std::string ctxStr = JsonHelper::getString(body, "profileContext", "general");
            std::string profileId = JsonHelper::getString(body, "profileId", "profile-default");

            MeasurementRecord rec(
                weight, height, age,
                stringToSex(sexStr),
                stringToActivityLevel(actStr),
                stringToProfileContext(ctxStr),
                profileId
            );

            // Check optional bounds
            if (JsonHelper::hasKey(body, "weightMin") && JsonHelper::hasKey(body, "weightMax")) {
                double wMin = JsonHelper::getDouble(body, "weightMin", 0.0);
                double wMax = JsonHelper::getDouble(body, "weightMax", 0.0);
                std::string wSrc = JsonHelper::getString(body, "weightSource", "User declared tolerance");
                if (wMin > 0 && wMax >= wMin) {
                    rec.setWeightBoundsKg(wMin, wMax, wSrc);
                }
            }

            if (JsonHelper::hasKey(body, "heightMin") && JsonHelper::hasKey(body, "heightMax")) {
                double hMin = JsonHelper::getDouble(body, "heightMin", 0.0);
                double hMax = JsonHelper::getDouble(body, "heightMax", 0.0);
                std::string hSrc = JsonHelper::getString(body, "heightSource", "User declared tolerance");
                if (hMin > 0 && hMax >= hMin) {
                    rec.setHeightBoundsCm(hMin, hMax, hSrc);
                }
            }

            bool save = (cleanPath == "/api/v1/measurements" || JsonHelper::getBool(body, "save", true));
            EvidenceTrace trace = service_->processAssessment(rec, save);
            std::string respJson = trace.toJsonString();

            return "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Content-Length: " + std::to_string(respJson.size()) + "\r\n\r\n" + respJson;
        }

        // Endpoint 2: GET /api/v1/history
        if (method == "GET" && cleanPath == "/api/v1/history") {
            std::string profileId = "";
            size_t pPos = queryStr.find("profileId=");
            if (pPos != std::string::npos) {
                profileId = queryStr.substr(pPos + 10);
                size_t amp = profileId.find('&');
                if (amp != std::string::npos) profileId = profileId.substr(0, amp);
            }

            auto hist = service_->getHistory(profileId);
            std::ostringstream ss;
            ss << "[\n";
            for (size_t i = 0; i < hist.size(); ++i) {
                ss << "  " << hist[i].fullTraceJson;
                if (i + 1 < hist.size()) ss << ",\n";
                else ss << "\n";
            }
            ss << "]";
            std::string respJson = ss.str();

            return "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Content-Length: " + std::to_string(respJson.size()) + "\r\n\r\n" + respJson;
        }

        // Endpoint 3: GET /api/v1/results/{id}
        if (method == "GET" && cleanPath.find("/api/v1/results/") == 0) {
            std::string resultId = cleanPath.substr(std::string("/api/v1/results/").size());
            SavedResultRecord res = service_->getResult(resultId);
            return "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Content-Length: " + std::to_string(res.fullTraceJson.size()) + "\r\n\r\n" + res.fullTraceJson;
        }

        // Endpoint 4: GET /api/v1/experimental-evaluation
        if (method == "GET" && cleanPath == "/api/v1/experimental-evaluation") {
            EvaluationReport eval = ExperimentalEvaluation::runEvaluation(*service_);
            std::string respJson = ExperimentalEvaluation::reportToJson(eval);

            return "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Content-Length: " + std::to_string(respJson.size()) + "\r\n\r\n" + respJson;
        }

        // Endpoint 5: POST /api/v1/report
        if (method == "POST" && cleanPath == "/api/v1/report") {
            // Takes trace json or calculates and returns markdown report
            double weight = JsonHelper::getDouble(body, "weightKg", 70.0);
            double height = JsonHelper::getDouble(body, "heightCm", 175.0);
            int age = JsonHelper::getInt(body, "ageYears", 25);
            std::string sexStr = JsonHelper::getString(body, "sex", "male");
            std::string actStr = JsonHelper::getString(body, "activityLevel", "sedentary");
            std::string ctxStr = JsonHelper::getString(body, "profileContext", "general");

            MeasurementRecord rec(
                weight, height, age,
                stringToSex(sexStr),
                stringToActivityLevel(actStr),
                stringToProfileContext(ctxStr)
            );

            if (JsonHelper::hasKey(body, "weightMin") && JsonHelper::hasKey(body, "weightMax")) {
                rec.setWeightBoundsKg(JsonHelper::getDouble(body, "weightMin", 0.0),
                                      JsonHelper::getDouble(body, "weightMax", 0.0), "User tolerance");
            }
            if (JsonHelper::hasKey(body, "heightMin") && JsonHelper::hasKey(body, "heightMax")) {
                rec.setHeightBoundsCm(JsonHelper::getDouble(body, "heightMin", 0.0),
                                      JsonHelper::getDouble(body, "heightMax", 0.0), "User tolerance");
            }

            EvidenceTrace trace = service_->processAssessment(rec, false);
            std::string reportMd = ReportGenerator::generateMarkdownReport(trace);
            std::ostringstream ss;
            ss << "{\"markdown\": \"";
            for (char c : reportMd) {
                if (c == '"') ss << "\\\"";
                else if (c == '\\') ss << "\\\\";
                else if (c == '\n') ss << "\\n";
                else if (c == '\r') continue;
                else ss << c;
            }
            ss << "\"}";
            std::string respJson = ss.str();

            return "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Content-Length: " + std::to_string(respJson.size()) + "\r\n\r\n" + respJson;
        }

        // Unknown API route
        std::string err = "{\"error\": \"Unknown API endpoint: " + cleanPath + "\"}";
        return "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: application/json\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Content-Length: " + std::to_string(err.size()) + "\r\n\r\n" + err;

    } catch (const ValidationException& ex) {
        std::string err = "{\"error\": \"" + std::string(ex.what()) + "\", \"type\": \"ValidationException\"}";
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: " +
               std::to_string(err.size()) + "\r\n\r\n" + err;
    } catch (const CalculationException& ex) {
        std::string err = "{\"error\": \"" + std::string(ex.what()) + "\", \"type\": \"CalculationException\"}";
        return "HTTP/1.1 422 Unprocessable Entity\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: " +
               std::to_string(err.size()) + "\r\n\r\n" + err;
    } catch (const std::exception& ex) {
        std::string err = "{\"error\": \"" + std::string(ex.what()) + "\", \"type\": \"InternalException\"}";
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: " +
               std::to_string(err.size()) + "\r\n\r\n" + err;
    }
}

void HttpServer::start() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[FitMetric Server] WSAStartup failed.\n";
        return;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "[FitMetric Server] Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return;
    }

    BOOL opt = TRUE;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);

    if (bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[FitMetric Server] Bind failed on port " << port_ << ": " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return;
    }

    if (listen(listenSock, 10) == SOCKET_ERROR) {
        std::cerr << "[FitMetric Server] Listen failed: " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return;
    }

    isRunning_ = true;
    std::cout << "========================================================\n";
    std::cout << "  FitMetric REST API and Web Server Started!            \n";
    std::cout << "  Core Engine: C++ with ISVE (OOPS Architecture)        \n";
    std::cout << "  Listening URL: http://localhost:" << port_ << "/      \n";
    std::cout << "========================================================\n" << std::endl;

    while (isRunning_) {
        // Set up select timeout so server can cleanly terminate if needed
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(listenSock, &readSet);

        timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int sel = select(0, &readSet, NULL, NULL, &tv);
        if (sel <= 0) continue;

        sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCKET) continue;

        char buffer[16384];
        int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string reqStr(buffer, bytesReceived);

            std::istringstream reqStream(reqStr);
            std::string reqLine;
            std::getline(reqStream, reqLine);
            std::istringstream lineStream(reqLine);
            std::string method, path, proto;
            lineStream >> method >> path >> proto;

            // Direct search for Content-Length
            int contentLength = 0;
            size_t clPos = reqStr.find("Content-Length:");
            if (clPos == std::string::npos) clPos = reqStr.find("content-length:");
            if (clPos != std::string::npos) {
                size_t valStart = clPos + 15;
                while (valStart < reqStr.size() && (reqStr[valStart] == ' ' || reqStr[valStart] == ':')) valStart++;
                try {
                    contentLength = std::stoi(reqStr.substr(valStart));
                } catch (...) {}
            }

            // Extract Body
            size_t headerEnd = reqStr.find("\r\n\r\n");
            std::string body = "";
            if (headerEnd != std::string::npos) {
                body = reqStr.substr(headerEnd + 4);
            } else {
                headerEnd = reqStr.find("\n\n");
                if (headerEnd != std::string::npos) {
                    body = reqStr.substr(headerEnd + 2);
                }
            }

            // If more bytes are expected for the body, keep receiving
            while (static_cast<int>(body.size()) < contentLength) {
                int n = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0) break;
                body.append(buffer, n);
            }

            std::string response;
            if (path.find("/api/v1/") == 0) {
                response = handleApiRequest(method, path, body);
            } else {
                response = handleStaticFile(path);
            }

            send(clientSock, response.c_str(), static_cast<int>(response.size()), 0);
        }

        closesocket(clientSock);
    }

    closesocket(listenSock);
    WSACleanup();
}

} // namespace fitmetric
