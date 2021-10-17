#ifndef JSON_H
#define JSON_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace JsonUtils {
    json TryParse(const std::string &str) noexcept;
}

#endif
