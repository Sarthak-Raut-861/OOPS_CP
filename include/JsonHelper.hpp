#pragma once

#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace fitmetric {

class JsonHelper {
public:
    static std::string trim(const std::string& str) {
        size_t start = 0;
        while (start < str.size() && (std::isspace(str[start]) || str[start] == '"')) {
            start++;
        }
        size_t end = str.size();
        while (end > start && (std::isspace(str[end - 1]) || str[end - 1] == '"')) {
            end--;
        }
        return str.substr(start, end - start);
    }

    // Extract a string property from a JSON string: "key": "value"
    static std::string getString(const std::string& json, const std::string& key, const std::string& defaultVal = "") {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return defaultVal;

        size_t colon = json.find(':', pos + pattern.size());
        if (colon == std::string::npos) return defaultVal;

        size_t startQuote = json.find('"', colon + 1);
        if (startQuote == std::string::npos) return defaultVal;

        size_t endQuote = json.find('"', startQuote + 1);
        if (endQuote == std::string::npos) return defaultVal;

        return json.substr(startQuote + 1, endQuote - startQuote - 1);
    }

    // Extract a numeric double property: "key": 123.45
    static double getDouble(const std::string& json, const std::string& key, double defaultVal = 0.0) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return defaultVal;

        size_t colon = json.find(':', pos + pattern.size());
        if (colon == std::string::npos) return defaultVal;

        size_t numStart = colon + 1;
        while (numStart < json.size() && (std::isspace(json[numStart]) || json[numStart] == '"')) {
            numStart++;
        }
        if (numStart >= json.size()) return defaultVal;

        size_t numEnd = numStart;
        while (numEnd < json.size() && (std::isdigit(json[numEnd]) || json[numEnd] == '.' || json[numEnd] == '-' || json[numEnd] == '+')) {
            numEnd++;
        }

        std::string numStr = json.substr(numStart, numEnd - numStart);
        if (numStr.empty()) return defaultVal;

        try {
            return std::stod(numStr);
        } catch (...) {
            return defaultVal;
        }
    }

    // Extract integer
    static int getInt(const std::string& json, const std::string& key, int defaultVal = 0) {
        return static_cast<int>(getDouble(json, key, defaultVal));
    }

    // Extract boolean
    static bool getBool(const std::string& json, const std::string& key, bool defaultVal = false) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return defaultVal;

        size_t colon = json.find(':', pos + pattern.size());
        if (colon == std::string::npos) return defaultVal;

        std::string sub = json.substr(colon + 1, 10);
        if (sub.find("true") != std::string::npos || sub.find("TRUE") != std::string::npos || sub.find("1") != std::string::npos) {
            return true;
        }
        if (sub.find("false") != std::string::npos || sub.find("FALSE") != std::string::npos || sub.find("0") != std::string::npos) {
            return false;
        }
        return defaultVal;
    }

    // Check if key exists and is not null
    static bool hasKey(const std::string& json, const std::string& key) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return false;
        size_t colon = json.find(':', pos + pattern.size());
        if (colon == std::string::npos) return false;
        std::string rest = json.substr(colon + 1, 8);
        if (rest.find("null") != std::string::npos) return false;
        return true;
    }
};

} // namespace fitmetric
