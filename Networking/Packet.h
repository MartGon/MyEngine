#include <string>
#include "Vector2.h"
#include <SDL.h>
#include <chrono>

class GameObject;
class Component;
class Navigator;
class Transform;

#pragma once

enum PacketType
{
	NULL_PACKET,
	COMPONENT_PACKET,
	GAMEOBJECT_UPDATE_PACKET,
	GAMEOBJECT_CREATE_PACKET,
	EVENT_PACKET,
	MOUSE_STATE_PACKET,
	TIMESTAMP_PACKET
};

enum ComponentPacketType
{
	COMPONENT_NULL,
	COMPONENT_TRANSFORM,
	COMPONENT_COLLIDER,
	COMPONENT_NAVIGATOR,
	COMPONENT_ANIMATOR,
	COMPONENT_TEXTURE_RENDERER,
	COMPONENT_AUDIO_PLAYER
};

class Packet
{
public:
	// Constructors
	Packet();
	Packet(PacketType type);
	~Packet();

	// Packet data
	PacketType packetType = NULL_PACKET;
	
	// Methods
	virtual size_t getSize() { return sizeof(Packet); };
};

class EventPacket : public Packet
{
public:
	// Constructors
	EventPacket();
	EventPacket(int gameobject_id, SDL_Event event);

	int gameobject_id;
	SDL_Event event;

	// Overrided
	size_t getSize() override { return sizeof(EventPacket); };
};

class MouseStatePacket : public Packet
{
public:
	// Constructors
	MouseStatePacket();
	MouseStatePacket(Vector2<int> pos);

	Vector2<int> position;

	// Overrided
	size_t getSize() override { return sizeof(MouseStatePacket); };
};

class TimeStampPacket : public Packet
{
public:
	// Constructors
	TimeStampPacket();
	TimeStampPacket(std::chrono::milliseconds ms);

	std::chrono::milliseconds timestamp;

	// Override
	size_t getSize() override { return sizeof(TimeStampPacket); };
};

class GameObjectUpdatePacket : public Packet
{
public:
	// Constructor
	GameObjectUpdatePacket();
	GameObjectUpdatePacket(GameObject* gameobject);

	// Members
	int gameobject_id = -1;
	bool isActive = false;
	//bool updateFromClient = false;

	// Overrided
	size_t getSize() override { return sizeof(GameObjectUpdatePacket); };
};

class GameObjectCreatePacket : public Packet
{
public:
	// Constructor
	GameObjectCreatePacket();
	GameObjectCreatePacket(GameObject* gameobject);

	// Member
	int gameobject_id = -1;
	size_t gameobject_template_id = 0;

	// Overrided
	size_t getSize() override { return sizeof(GameObjectCreatePacket); };
};

class ComponentPacket : public Packet
{
public:

	// Constructor
	ComponentPacket() {};
	ComponentPacket(ComponentPacketType sub_type, Component* component);
	ComponentPacket(Component* component);

	// Members
	ComponentPacketType sub_type = COMPONENT_NULL;
	unsigned int id = -1;
	int gameobject_id = -1;
	bool isEnabled = false;

	// Overrided
	size_t getSize() override { return sizeof(ComponentPacket); };

};

class NavigatorPacket : public ComponentPacket
{
public:
	// Constructor
	NavigatorPacket() {};
	NavigatorPacket(Navigator* nav);

	// Members
	Vector2<float> direction;
	float speed = 0;

	// Kinematics
	bool isKinematic = false;
	bool stopAtInflectionPoint = false;
	Vector2<float> acceleration;

	// Methods
	size_t getSize() override { return sizeof(NavigatorPacket); };
};

class TransformPacket : public ComponentPacket
{
public:
	// Constructor
	TransformPacket() {};
	TransformPacket(Transform* transform);

	// Members
	int parent_id = -1;
	Vector2<float> position;
	Vector2<float> scale;
	bool isRotationCenterNull = false;
	double zRotation;
	Vector2<int> rotationCenter;

	// Methods
	size_t getSize() override { return sizeof(TransformPacket); };
};