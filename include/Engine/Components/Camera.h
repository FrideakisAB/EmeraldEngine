#ifndef CAMERA_H
#define CAMERA_H

#include "Platform.h"
#include "ECS/Component.h"

enum class Projection : u8 {
    Perspective = 0,
    Orthographic
};

class Camera final : public ECS::Component<Camera> {
public:
    f32 Fov = 60.0f;
    f32 Ratio = 1.5f;
    f32 NearClip = 0.1f;
    f32 FarClip = 100.0f;
    Projection Proj = Projection::Perspective;

    [[nodiscard]] glm::mat4 GetVPMatrix(u32 width, u32 height) const;
    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    [[nodiscard]] glm::mat4 GetProjMatrix(u32 width, u32 height) const;

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json &j) final;
};

#endif
