#include "translation/GoogleTranslator.h"

#include <cpr/cpr.h>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace translation {
    static std::unordered_map<Language, std::string> languageToCode = {
        {Language::English, "en"},
        {Language::Deutsch, "de"},
        {Language::Russian, "ru"},
        {Language::Unknown, "auto"},
        {Language::Default, "auto"},
    };

    static std::string queryParam(const std::string& key, const std::string& value);

    /**  @throws std::runtime_error */
    static std::string parseResponse(const std::string& jsonStr);

    /**  @throws std::runtime_error */
    std::string GoogleTranslator::translate(Language from, Language to,
                                            const std::string& text) const {
        std::ostringstream oss;
        oss << queryParam("client", "gtx");
        oss << "&" << queryParam("sl", languageToCode.at(from));
        oss << "&" << queryParam("tl", languageToCode.at(to));
        oss << "&" << queryParam("dt", "t");
        oss << "&" << queryParam("q", text);

        const auto url = "https://translate.googleapis.com/translate_a/single?" + oss.str();
        const auto response = Get(cpr::Url{url});

        if (response.status_code != 200) {
            throw std::runtime_error(
                "Google translation API respond with status " + std::to_string(response.status_code));
        }

        return parseResponse(response.text);
    }

    static std::string queryParam(const std::string& key, const std::string& value) {
        return cpr::util::urlEncode(key) + "=" + cpr::util::urlEncode(value);
    }

    /**  @throws std::runtime_error */
    std::string parseResponse(const std::string& jsonStr) {
        json data;

        try {
            data = json::parse(jsonStr);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error parsing response: ") + e.what());
        }

        if (!data.is_array()) {
            throw std::runtime_error("Unexpected response format: root is not array");
        }

        std::ostringstream result;

        if (!data.empty() && data[0].is_array()) {
            for (const auto& seg : data[0]) {
                if (seg.is_array() && !seg.empty() && seg[0].is_string()) {
                    result << seg[0].get<std::string>();
                }
            }
        }

        return result.str();
    }
}