#include "Render/ShaderTools.h"

namespace Shaders {
    void to_json(json &j, const Shaders::ShaderParamValue &paramValue)
    {
        j = json{{"valueType", paramValue.valueType}};

        switch (paramValue.valueType)
        {
        case Shaders::ShaderValue::Int:
            j["value"] = std::get<i32>(paramValue.value);
            break;

        case Shaders::ShaderValue::UnsignedInt:
            j["value"] = std::get<u32>(paramValue.value);
            break;

        case Shaders::ShaderValue::Float:
            j["value"] = std::get<f32>(paramValue.value);
            break;

        case Shaders::ShaderValue::Double:
            j["value"] = std::get<f64>(paramValue.value);
            break;

        case Shaders::ShaderValue::Vector2:
            j["value"] = std::get<glm::vec2>(paramValue.value);
            break;

        case Shaders::ShaderValue::Vector3:
            j["value"] = std::get<glm::vec3>(paramValue.value);
            break;

        case Shaders::ShaderValue::Vector4:
            j["value"] = std::get<glm::vec4>(paramValue.value);
            break;

        case Shaders::ShaderValue::Mat2:
            j["value"] = std::get<glm::mat2>(paramValue.value);
            break;

        case Shaders::ShaderValue::Mat3:
            j["value"] = std::get<glm::mat3>(paramValue.value);
            break;

        case Shaders::ShaderValue::Mat4:
            j["value"] = std::get<glm::mat4>(paramValue.value);
            break;

        default:
            break;
        }
    }

    void from_json(const json &j, Shaders::ShaderParamValue &paramValue)
    {
        j.at("valueType").get_to(paramValue.valueType);

        switch (paramValue.valueType)
        {
        case Shaders::ShaderValue::Int:
            paramValue.value = j["value"].get<i32>();
            break;

        case Shaders::ShaderValue::UnsignedInt:
            paramValue.value = j["value"].get<u32>();
            break;

        case Shaders::ShaderValue::Float:
            paramValue.value = j["value"].get<f32>();
            break;

        case Shaders::ShaderValue::Double:
            paramValue.value = j["value"].get<f64>();
            break;

        case Shaders::ShaderValue::Vector2:
            paramValue.value = j["value"].get<glm::vec2>();
            break;

        case Shaders::ShaderValue::Vector3:
            paramValue.value = j["value"].get<glm::vec3>();
            break;

        case Shaders::ShaderValue::Vector4:
            paramValue.value = j["value"].get<glm::vec4>();
            break;

        case Shaders::ShaderValue::Mat2:
            paramValue.value = j["value"].get<glm::mat2>();
            break;

        case Shaders::ShaderValue::Mat3:
            paramValue.value = j["value"].get<glm::mat3>();
            break;

        case Shaders::ShaderValue::Mat4:
            paramValue.value = j["value"].get<glm::mat4>();
            break;

        default:
            break;
        }
    }
}
