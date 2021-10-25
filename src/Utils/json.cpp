#include "Utils/json.h"

#include "Log.h"
#include <fstream>

json JsonUtils::TryParse(const std::string &str) noexcept
{
    json j;

    try
    {
        j = json::parse(str);
    }
    catch (const json::parse_error &ex)
    {
        logger->Error("JSON parse error! %s, with id: %i, in byte: %llu", ex.what(), ex.id, ex.byte);
    }

    return j;
}

namespace ns {
    void to_json(json &j, const glm::vec2 &vector)
    {
        j = json{{"value", {vector.x, vector.y}}};
    }

    void from_json(const json &j, glm::vec2 &vector)
    {
        vector.x = j["value"][0];
        vector.y = j["value"][1];
    }

    void to_json(json &j, const glm::vec3 &vector)
    {
        j = json{{"value", {vector.x, vector.y, vector.z}}};
    }

    void from_json(const json &j, glm::vec3 &vector)
    {
        vector.x = j["value"][0];
        vector.y = j["value"][1];
        vector.z = j["value"][2];
    }

    void to_json(json &j, const glm::vec4 &vector)
    {
        j = json{{"value", {vector.x, vector.y, vector.z, vector.w}}};
    }

    void from_json(const json &j, glm::vec4 &vector)
    {
        vector.x = j["value"][0];
        vector.y = j["value"][1];
        vector.z = j["value"][2];
        vector.w = j["value"][3];
    }

    void to_json(json &j, const glm::mat2 &mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[1][0], mat[1][1]}}};
    }

    void from_json(const json &j, glm::mat2 &mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[1][0] = j["value"][2];
        mat[1][1] = j["value"][3];
    }

    void to_json(json &j, const glm::mat3 &mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[0][2],
                            mat[1][0], mat[1][1], mat[1][2],
                            mat[2][0], mat[2][1], mat[2][2]}}};
    }

    void from_json(const json &j, glm::mat3 &mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[0][2] = j["value"][2];

        mat[1][0] = j["value"][3];
        mat[1][1] = j["value"][4];
        mat[1][2] = j["value"][5];

        mat[2][0] = j["value"][6];
        mat[2][1] = j["value"][7];
        mat[2][2] = j["value"][8];
    }

    void to_json(json &j, const glm::mat4 &mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[0][2], mat[0][3],
                            mat[1][0], mat[1][1], mat[1][2], mat[1][3],
                            mat[2][0], mat[2][1], mat[2][2], mat[2][3],
                            mat[3][0], mat[3][1], mat[3][2], mat[3][3]}}};
    }

    void from_json(const json &j, glm::mat4 &mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[0][2] = j["value"][2];
        mat[0][3] = j["value"][3];

        mat[1][0] = j["value"][4];
        mat[1][1] = j["value"][5];
        mat[1][2] = j["value"][6];
        mat[1][3] = j["value"][7];

        mat[2][0] = j["value"][8];
        mat[2][1] = j["value"][9];
        mat[2][2] = j["value"][10];
        mat[2][3] = j["value"][11];

        mat[3][0] = j["value"][12];
        mat[3][1] = j["value"][13];
        mat[3][2] = j["value"][14];
        mat[3][3] = j["value"][15];
    }
}
