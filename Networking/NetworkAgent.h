#include <SDL_net.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Packet.h"
#include <chrono>
#pragma once

class NetworkBuffer
{
public:
	NetworkBuffer(size_t  size, size_t limit = 0);
	~NetworkBuffer();

	// Data ptr
	void* buffer = nullptr;
	Uint8* buffer_ptr = nullptr;

	// Size
	size_t virtual_size = 0;
	size_t real_size = 0;

	// Size Limit
	size_t limit = 0;

	bool append_data(void* data, size_t len);
	bool isOverLimit();
	void clear();
};

class NetworkAgent
{
public:
	// Constructors
	NetworkAgent();
	~NetworkAgent();

	// Attributes
	NetworkBuffer buffer = NetworkBuffer(1460);

	// Socket Set
	SDLNet_SocketSet socket_set;

	// Debug
	std::chrono::milliseconds ping = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());;

	// Methods
		// Initializing
	static void initNetworking();

		// Configuration
	bool readingError = false;
	virtual bool readConfigFile();
	virtual bool establishConnection();

	// Communication
	virtual bool sendPacket(Packet* packet, bool buffered = true);
	virtual Packet* recvPacket();

	// Other
	void destroy();
	virtual void beforeDestroy();

protected:
		// Communication
	bool sendPacket(TCPsocket socket, Packet* packet, bool buffered = true);
	Packet* recvPacket(TCPsocket socket);
};

