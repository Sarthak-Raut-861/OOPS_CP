#pragma once

#include "Common.hpp"
#include "FitMetricService.hpp"
#include <string>
#include <memory>

namespace fitmetric {

class HttpServer {
private:
    int port_;
    std::string webRoot_;
    std::shared_ptr<FitMetricService> service_;
    bool isRunning_;

    std::string handleApiRequest(const std::string& method, const std::string& path, const std::string& body);
    std::string handleStaticFile(const std::string& path);
    std::string getMimeType(const std::string& filepath);

public:
    HttpServer(int port, const std::string& webRoot, std::shared_ptr<FitMetricService> service);
    ~HttpServer();

    void start();
    void stop();
};

} // namespace fitmetric
