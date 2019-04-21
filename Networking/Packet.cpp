#include "Packet.h"
#include "Navigator.h"

// Constructors

Packet::Packet()
{
}

Packet::Packet(PacketType type)
{
	packetType = type;
}

Packet::~Packet()
{
}

// Component Packet

ComponentPacket::ComponentPacket(ComponentPacketType sub_type, Component* component) : Packet(COMPONENT_PACKET)
{
	this->sub_type = sub_type;
	if(GameObject* go = component->gameObject)
		this->gameobject_id = go->id;
}

// Navigator Packet

NavigatorPacket::NavigatorPacket(Navigator* nav) : ComponentPacket(COMPONENT_NAVIGATOR, nav)
{
	// Cinematic
	this->direction = nav->getDirection();
	this->speed = nav->speed;

	// Kinematic
	this->isKinematic = nav->isKinematic;
	this->stopAtInflectionPoint = nav->stopAtInflectionPoint;
	this->acceleration = nav->acceleration;
}

size_t NavigatorPacket::getSize()
{
	return sizeof(NavigatorPacket);
}

// Transform Packet

TransformPacket::TransformPacket(Transform* transform) : ComponentPacket(COMPONENT_TRANSFORM, transform)
{
	if (transform->parent)
		this->parent_id = transform->parent->gameObject->id;
	this->position = transform->position;
	this->scale = transform->scale;
	this->zRotation = transform->zRotation;
	if(transform->rotationCenter)
		this->rotationCenter = *transform->rotationCenter;
}

size_t TransformPacket::getSize()
{
	return sizeof(TransformPacket);
}