#include "Packet.h"
#include "Navigator.h"
#include "Transform.h"
#include "GameObject.h"

#include <typeinfo>

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

// GameObject Update Packet

GameObjectUpdatePacket::GameObjectUpdatePacket()
{
	packetType = GAMEOBJECT_UPDATE_PACKET;
}

GameObjectUpdatePacket::GameObjectUpdatePacket(GameObject* gameobject) : GameObjectUpdatePacket()
{
	this->gameobject_id = gameobject->id;
	this->isActive = gameobject->isActive;
	//this->updateFromClient = gameobject->updateFromClient;
}

// GameObject Create Packet
GameObjectCreatePacket::GameObjectCreatePacket()
{
	packetType = GAMEOBJECT_CREATE_PACKET;
}

GameObjectCreatePacket::GameObjectCreatePacket(GameObject* gameobject) : GameObjectCreatePacket()
{
	this->gameobject_template_id = gameobject->template_id;
	this->gameobject_id = gameobject->id;
}

// Component Packet

ComponentPacket::ComponentPacket(ComponentPacketType sub_type, Component* component) : Packet(COMPONENT_PACKET)
{
	this->sub_type = sub_type;
	this->id = component->id;
	if(GameObject* go = component->gameObject)
		this->gameobject_id = go->id;
	this->isEnabled = component->isEnabled;
}

ComponentPacket::ComponentPacket(Component* component) : ComponentPacket(component->getComponentPacketType(), component)
{

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

// Transform Packet

TransformPacket::TransformPacket(Transform* transform) : ComponentPacket(COMPONENT_TRANSFORM, transform)
{
	if (transform->parent)
		this->parent_id = transform->parent->gameObject->id;
	this->position = transform->position;
	this->scale = transform->scale;
	this->zRotation = transform->zRotation;
	if (transform->rotationCenter)
	{
		this->isRotationCenterNull = false;
		this->rotationCenter = *transform->rotationCenter;
	}
	else
	{
		this->isRotationCenterNull = true;
	}
}

