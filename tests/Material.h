#include "Render/Material.h"
#include "Engine/Components/MaterialComponent.h"

TEST_CASE( "Material work test", "[Material]" )
{
    SECTION("Material base test")
    {
        Material material;

        REQUIRE( material.GetTypeId() == boost::typeindex::type_id<Material>().hash_code() );

        material.Shader = "Test";

        auto *material2 = static_cast<Material*>(material.Clone());

        REQUIRE( material.Shader == material2->Shader );

        material.Uniforms.insert({"test", {Shaders::ShaderValue::Float, 10.0f}});

        json saveJson = material.SerializeObj();
        material.UnSerializeObj(saveJson);

        REQUIRE( material.Shader == "Test" );
        REQUIRE( std::get<f32>(material.Uniforms["test"].value) == 10.0f );

        delete material2;
    }

    SECTION("Uniforms save test")
    {
        Material material;

        material.Uniforms.insert({"Int", {Shaders::ShaderValue::Int, 1}});
        material.Uniforms.insert({"UnsignedInt", {Shaders::ShaderValue::UnsignedInt, 1u}});
        material.Uniforms.insert({"Float", {Shaders::ShaderValue::Float, 1.0f}});
        material.Uniforms.insert({"Double", {Shaders::ShaderValue::Double, 1.0}});
        material.Uniforms.insert({"Vector2", {Shaders::ShaderValue::Vector2, glm::vec2(1.0f)}});
        material.Uniforms.insert({"Vector3", {Shaders::ShaderValue::Vector3, glm::vec3(1.0f)}});
        material.Uniforms.insert({"Vector4", {Shaders::ShaderValue::Vector4, glm::vec4(1.0f)}});
        material.Uniforms.insert({"Mat2", {Shaders::ShaderValue::Mat2, glm::mat2(1.0f)}});
        material.Uniforms.insert({"Mat3", {Shaders::ShaderValue::Mat3, glm::mat3(1.0f)}});
        material.Uniforms.insert({"Mat4", {Shaders::ShaderValue::Mat4, glm::mat4(1.0f)}});

        json saveJson = material.SerializeObj();
        material.UnSerializeObj(saveJson);

        REQUIRE( std::get<i32>(material.Uniforms["Int"].value) == 1 );
        REQUIRE( std::get<u32>(material.Uniforms["UnsignedInt"].value) == 1u );
        REQUIRE( std::get<f32>(material.Uniforms["Float"].value) == 1.0f );
        REQUIRE( std::get<f64>(material.Uniforms["Double"].value) == 1.0 );
        REQUIRE( std::get<glm::vec2>(material.Uniforms["Vector2"].value) == glm::vec2(1.0f) );
        REQUIRE( std::get<glm::vec3>(material.Uniforms["Vector3"].value) == glm::vec3(1.0f) );
        REQUIRE( std::get<glm::vec4>(material.Uniforms["Vector4"].value) == glm::vec4(1.0f) );
        REQUIRE( std::get<glm::mat2>(material.Uniforms["Mat2"].value) == glm::mat2(1.0f) );
        REQUIRE( std::get<glm::mat3>(material.Uniforms["Mat3"].value) == glm::mat3(1.0f) );
        REQUIRE( std::get<glm::mat4>(material.Uniforms["Mat4"].value) == glm::mat4(1.0f) );
    }

    SECTION("MaterialComponent test")
    {
        class Test : public IAsset {
        public:
            [[nodiscard]] size_t GetTypeId() const noexcept final
            {
                return 1;
            }
        };

        auto *material = new MaterialComponent();

        REQUIRE( material->IsValid() == false );

        material->SetMaterial(std::make_shared<Test>());

        REQUIRE( material->IsValid() == false );

        auto mat = std::make_shared<Material>();

        material->SetMaterial(mat);

        REQUIRE( material->IsValid() == true );
        REQUIRE( material->GetMaterial() == mat.get() );
        REQUIRE( material->GetMaterialHandle() == mat );
    }
}
