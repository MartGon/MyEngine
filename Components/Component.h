#pragma once
#include "Updateable.h"
#include "Packet.h"

// Class to wrap al the different kinds of components
class GameObject;

// TODO - Virtual destructor

class Component : public Updateable
{
public:
	Component();
	~Component();

	// Attributes
	unsigned int id = -1;
	bool isEnabled = true;
	GameObject *gameObject = nullptr;

	// Methods
	virtual void start() {};
	virtual void update() override {};
	virtual void destroy() {};

	// Network Methods
	virtual ComponentPacket* toComponentPacket() { return new ComponentPacket(this); };
	virtual void updateFromComponentPacket(ComponentPacket* component_packet) { this->isEnabled = component_packet->isEnabled; };
	virtual ComponentPacketType getComponentPacketType() { return ComponentPacketType::COMPONENT_NULL; };
};

