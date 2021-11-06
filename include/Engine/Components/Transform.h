#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Utils/glm.h"
#include "ECS/Component.h"

struct Position {
    glm::vec3 position{};
    glm::vec3 scale = glm::vec3(1.0f);
    glm::quat rotate{};
};

enum class PositionType : u8 {
    Local = 0,
    Global
};

class Transform final : public ECS::Component<Transform> {
private:
    Position position{};
    glm::mat4 transformMat = glm::mat4(1.0f);
    glm::vec3 up = WorldUp;
    glm::vec3 right = WorldRight;
    glm::vec3 forward = WorldFront;
    PositionType positionType = PositionType::Local;

    void recalculate();

protected:
    void UpdateTree() final;

public:
    static inline const glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
    static inline const glm::vec3 WorldFront{0.0f, 0.0f, 1.0f};
    static inline const glm::vec3 WorldRight{1.0f, 0.0f, 0.0f};

    Transform();

    [[nodiscard]] Position GetLocalPosition() const;
    [[nodiscard]] Position GetGlobalPosition() const;

    void SetLocalPosition(Position newPosition);
    void SetGlobalPosition(Position newPosition);

    [[nodiscard]] PositionType GetPositionType() const noexcept { return positionType; }
    void SetPositionType(PositionType newPositionType);

    [[nodiscard]] inline const glm::mat4 &GetMat() const noexcept { return transformMat; }

    [[nodiscard]] inline glm::vec3 Right() const noexcept { return right; }
    [[nodiscard]] inline glm::vec3 Up() const noexcept { return up; }
    [[nodiscard]] inline glm::vec3 Forward() const noexcept { return forward; }

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json &j) final;
};

#endif
