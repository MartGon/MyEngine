#include "Transform.h"
#include "SceneManager.h"

Transform::Transform() : Component()
{
	position = Vector2<float>(0, 0);
	scale = Vector2<float>(1, 1);
	zRotation = 0;
	rotationCenter = nullptr;
}

ComponentPacket* Transform::toComponentPacket()
{
	return new TransformPacket(this);
}

void Transform::updateFromComponentPacket(ComponentPacket* component_packet)
{
	if (component_packet->sub_type != ComponentPacketType::COMPONENT_TRANSFORM)
		return;

	// Update transform
	if (TransformPacket* transform_packet = static_cast<TransformPacket*>(component_packet))
	{
		// Set parent
		if (transform_packet->parent_id == -1)
			parent = nullptr;
		else if (SceneManager::scene->gameObjectMap.find(transform_packet->parent_id) == SceneManager::scene->gameObjectMap.end())
			parent = &SceneManager::scene->gameObjectMap.at(gameObject->id)->transform;

		position = transform_packet->position;
		zRotation = transform_packet->zRotation;
		scale = transform_packet->scale;
		if (rotationCenter)
			*rotationCenter = transform_packet->rotationCenter;
	}
}
