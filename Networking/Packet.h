#include <string>
#include "Vector2.h"
#include "GameObject.h"

#include "Navigator.h"

#pragma once

enum PacketType
{
	NULL_PACKET,
	COMPONENT_PACKET
};

enum ComponentPacketType
{
	COMPONENT_NULL,
	COMPONENT_TRANSFORM,
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

class ComponentPacket : public Packet
{
public:

	// Constructor
	ComponentPacket() {};
	ComponentPacket(ComponentPacketType sub_type, Component* component);

	// Members
	ComponentPacketType sub_type = COMPONENT_NULL;
	int gameobject_id = -1;
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
	size_t getSize() override;
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
	double zRotation;
	Vector2<int> rotationCenter;

	// Methods
	size_t getSize() override;
};