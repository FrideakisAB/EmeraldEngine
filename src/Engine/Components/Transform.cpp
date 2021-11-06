#include "Engine/Components/Transform.h"

#include "Engine/Engine.h"
#include "ECS/EntityManager.h"
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform()
{
    treeLock = true;
}

Position Transform::GetLocalPosition() const
{
    if (positionType == PositionType::Local)
        return position;
    else
    {
        ECS::EntityId parent = Engine::Get().GetEntityManager().GetEntity(owner)->GetParentId();
        if (auto *transform = Engine::Get().GetComponentManager().GetComponent<Transform>(parent); transform != nullptr)
        {
            glm::mat4 localMat = glm::inverse(transform->transformMat) * transformMat;

            Position localPosition;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(localMat, localPosition.scale, localPosition.rotate, localPosition.position, skew, perspective);

            return localPosition;
        }
        else
            return position;
    }
}

Position Transform::GetGlobalPosition() const
{
    if (positionType == PositionType::Global)
        return position;
    else
    {
        Position globalPosition;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transformMat, globalPosition.scale, globalPosition.rotate, globalPosition.position, skew, perspective);

        return globalPosition;
    }
}

void Transform::SetLocalPosition(Position newPosition)
{
    if (positionType == PositionType::Local)
    {
        position = newPosition;
        recalculate();
    }
    else
    {
        positionType = PositionType::Local;
        position = newPosition;
        recalculate();
        position = GetGlobalPosition();
        positionType = PositionType::Global;
    }
}

void Transform::SetGlobalPosition(Position newPosition)
{
    if (positionType == PositionType::Global)
    {
        position = newPosition;
        recalculate();
    }
    else
    {
        positionType = PositionType::Global;
        position = newPosition;
        recalculate();
        position = GetLocalPosition();
        positionType = PositionType::Local;
    }
}

void Transform::SetPositionType(PositionType newPositionType)
{
    if (positionType != newPositionType)
    {
        if (positionType == PositionType::Local)
            position = GetLocalPosition();
        else
            position = GetGlobalPosition();

        positionType = newPositionType;

        recalculate();
    }
}

void Transform::UpdateTree()
{
    if (positionType == PositionType::Local)
    {
        ECS::EntityId parent = Engine::Get().GetEntityManager().GetEntity(owner)->GetParentId();
        if (auto *transform = Engine::Get().GetComponentManager().GetComponent<Transform>(parent); transform != nullptr)
        {
            glm::mat4 mat = glm::inverse(transform->transformMat) * transformMat;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(mat, position.scale, position.rotate, position.position, skew, perspective);
        }

        recalculate();
    }
}

json Transform::SerializeObj() const
{
    json data;

    data["cmpName"] = boost::typeindex::type_id<Transform>().pretty_name();

    data["position"] = {position.position.x, position.position.y, position.position.z};
    data["rotate"] = {position.rotate.w, position.rotate.x, position.rotate.y, position.rotate.z};
    data["scale"] = {position.scale.x, position.scale.y, position.scale.z};
    data["positionType"] = static_cast<u8>(positionType);

    return data;
}

void Transform::UnSerializeObj(const json &j)
{
    position.position = {j["position"][0], j["position"][1], j["position"][2]};
    position.rotate = {j["rotate"][0], j["rotate"][1], j["rotate"][2], j["rotate"][3]};
    position.scale = {j["scale"][0], j["scale"][1], j["scale"][2]};
    positionType = static_cast<PositionType>(j["positionType"].get<u8>());

    recalculate();
}

void Transform::recalculate()
{
    auto &CM = Engine::Get().GetComponentManager();

    glm::mat4 parentMat = glm::mat4(1.0f);

    ECS::IEntity *entity = Engine::Get().GetEntityManager().GetEntity(owner);
    if (positionType == PositionType::Local)
    {
        ECS::EntityId parent = entity->GetParentId();
        if (auto *transform = CM.GetComponent<Transform>(parent); transform != nullptr)
            parentMat = transform->transformMat;
    }

    parentMat = glm::translate(parentMat, position.position);
    parentMat = parentMat * glm::mat4_cast(position.rotate);
    parentMat = glm::scale(parentMat, position.scale);

    forward = position.rotate * WorldFront;
    up = position.rotate * WorldUp;
    right = position.rotate * WorldRight;

    transformMat = parentMat;

    for (auto &child : entity->GetChildIterable())
    {
        auto childTransform = child.GetComponent<Transform>();
        if (childTransform != nullptr && childTransform->positionType == PositionType::Local)
            childTransform->recalculate();
    }
}
