#include "Render/Material.h"

IAsset *Material::Clone() const
{
    return new Material(std::cref(*this));
}

json Material::SerializeObj() const
{
    json data;

    data["nameType"] = boost::typeindex::type_id<Material>().pretty_name();

    data["shaderName"] = Shader;
    data["uniformsMap"] = Uniforms;
    data["samplersMap"] = {};

    for (const auto&[name, handle]: Samplers)
    {
        if (!handle->GetName().empty())
            data["samplersMap"].emplace_back(json{name, handle->GetName()});
    }

    return data;
}

void Material::UnSerializeObj(const json &j)
{
    Shader = j["shaderName"];
    Uniforms = j["uniformsMap"].get<decltype(Uniforms)>();

    //TODO: sampler unserialize
}
