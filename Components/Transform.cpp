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
		else if (Transform* new_parent = &SceneManager::scene->getGameObjectById(transform_packet->parent_id)->transform)
			parent = new_parent;

		position = transform_packet->position;
		zRotation = transform_packet->zRotation;
		scale = transform_packet->scale;
		if (transform_packet->isRotationCenterNull)
			rotationCenter = nullptr;
		else
		{
			// Delete previous entry
			if (rotationCenter)
				delete rotationCenter;

			rotationCenter = new Vector2<int>(transform_packet->rotationCenter.x, transform_packet->rotationCenter.y);
		}
	}
}
