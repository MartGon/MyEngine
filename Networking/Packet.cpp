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

// Input status packet
InputStatusPacket::InputStatusPacket() : Packet(INPUT_STATUS_PACKET)
{

}

InputStatusPacket::InputStatusPacket(Uint32 frame, Uint8 input_flags)
{
	this->frame = frame;
	this->input_flags = input_flags;
}

// EventPacket

EventPacket::EventPacket() : Packet(EVENT_PACKET)
{

}

EventPacket::EventPacket(HandledEvent event) : EventPacket()
{
	add_event(event);
}

void EventPacket::add_event(HandledEvent event)
{
	this->handled_events.push_back(event);
	this->events_len = handled_events.size();
}

// Mouse State Packet

MouseStatePacket::MouseStatePacket() : Packet(MOUSE_STATE_PACKET)
{

}

MouseStatePacket::MouseStatePacket(Vector2<int> pos) : MouseStatePacket()
{
	this->position = pos;
}

// TimeStamp Packet

TimeStampPacket::TimeStampPacket() : Packet(TIMESTAMP_PACKET)
{

}

TimeStampPacket::TimeStampPacket(std::chrono::milliseconds ms) : TimeStampPacket()
{
	this->timestamp = ms;
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

