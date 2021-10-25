#include "Render/ShaderTools.h"

namespace ns {
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
            to_json(j["value"], std::get<glm::vec2>(paramValue.value));
            break;

        case Shaders::ShaderValue::Vector3:
            to_json(j["value"], std::get<glm::vec3>(paramValue.value));
            break;

        case Shaders::ShaderValue::Vector4:
            to_json(j["value"], std::get<glm::vec4>(paramValue.value));
            break;

        case Shaders::ShaderValue::Mat2:
            to_json(j["value"], std::get<glm::mat2>(paramValue.value));
            break;

        case Shaders::ShaderValue::Mat3:
            to_json(j["value"], std::get<glm::mat3>(paramValue.value));
            break;

        case Shaders::ShaderValue::Mat4:
            to_json(j["value"], std::get<glm::mat4>(paramValue.value));
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
        {
            glm::vec2 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        case Shaders::ShaderValue::Vector3:
        {
            glm::vec3 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        case Shaders::ShaderValue::Vector4:
        {
            glm::vec4 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        case Shaders::ShaderValue::Mat2:
        {
            glm::mat2 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        case Shaders::ShaderValue::Mat3:
        {
            glm::mat3 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        case Shaders::ShaderValue::Mat4:
        {
            glm::mat4 value;
            from_json(j["value"], value);
            paramValue.value = value;
            break;
        }

        default:
            break;
        }
    }
}
