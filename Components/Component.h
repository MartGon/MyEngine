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
	bool isEnabled = true;
	GameObject *gameObject = nullptr;

	// Methods
	virtual void start() {};
	virtual void update() override {};
	virtual void destroy() {};

	// Network Methods
	virtual ComponentPacket* toComponentPacket() { return new ComponentPacket(); };
	virtual void updateFromComponentPacket(ComponentPacket* component_packet) {};
};

