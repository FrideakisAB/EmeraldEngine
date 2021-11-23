#ifndef SHADERTOOLS_H
#define SHADERTOOLS_H

#include <string>
#include <variant>
#include "Platform.h"
#include "Utils/glm.h"
#include "Utils/json.h"

namespace Shaders {
    enum class ShaderValue : u8 {
        Int = 0,
        UnsignedInt,
        Float,
        Double,
        Vector2,
        Vector3,
        Vector4,
        Mat2,
        Mat3,
        Mat4,
        Sampler1D,
        Sampler2D,
        SamplerCube
    };

    constexpr bool IsSampler(ShaderValue value)
    {
        return (value == ShaderValue::Sampler1D || value == ShaderValue::Sampler2D || value == ShaderValue::SamplerCube);
    }

    struct ShaderParam {
        std::string name;
        ShaderValue valueType;
    };

    struct ShaderParamValue {
        ShaderValue valueType;
        std::variant<i32, u32, f32, f64,
                glm::vec2, glm::vec3, glm::vec4,
                glm::mat2, glm::mat3, glm::mat4> value;
    };

    void to_json(json &j, const Shaders::ShaderParamValue &paramValue);
    void from_json(const json &j, Shaders::ShaderParamValue &paramValue);
}

#endif
