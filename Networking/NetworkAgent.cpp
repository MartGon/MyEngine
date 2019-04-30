#include "NetworkAgent.h"

// Network buffer

NetworkBuffer::NetworkBuffer(size_t size, size_t limit) : virtual_size{ size }
{
	// Allocate buffer
	buffer = std::malloc(virtual_size);
	buffer_ptr = (Uint8*)buffer;

	// Set limit to 90% of virtual_size
	if (!limit)
		this->limit = virtual_size - 2 * virtual_size / 10;
}

NetworkBuffer::~NetworkBuffer()
{
	// Deallocate buffer
	std::free(buffer);
}

bool NetworkBuffer::append_data(void* data, size_t len)
{
	// Check for overflow
	size_t next_size = real_size + len;
	if (next_size > virtual_size)
		return false;

	// Copy data to buffer
	std::memcpy(buffer_ptr, data, len);

	// Update pointer location
	buffer_ptr += len;

	// Update real_size
	real_size += len;
	
	return true;
}

bool NetworkBuffer::isOverLimit()
{
	return real_size > limit;
}

void NetworkBuffer::clear()
{
	// Reset to start
	buffer_ptr = (Uint8*)buffer;

	// Reset real_size
	real_size = 0;
}

// Attributes


// Constructors

NetworkAgent::NetworkAgent()
{
		std::cout << "Inicializando networking\n";
		initNetworking();
}

NetworkAgent::~NetworkAgent()
{
}

// Methods

void NetworkAgent::initNetworking()
{
	if (SDLNet_Init() == -1)
	{
		std::cout << "SDLNet_Init: " << SDLNet_GetError() << "\n";
		return;
	}

}

bool NetworkAgent::readConfigFile()
{
	std::cout << "Parent read mehtod\n";
	return false;
}

bool NetworkAgent::establishConnection()
{
	return false;
}

bool NetworkAgent::sendPacket(TCPsocket socket, Packet* packet, bool buffered)
{
	if (!socket)
	{
		std::cout << "Can't send packet, socket is not opened \n";
		return false;
	}

	size_t len = packet->getSize();
	int result = 0;

	// Add to the data buffer
	if (buffered)
	{
		// Add to buffer
			// Add packet Length
		if (!buffer.append_data(&len, sizeof(size_t)))
			std::cout << "Overflow detected, buffer copy aborted\n";
		// Add packet data
		if (!buffer.append_data(packet, len))
			std::cout << "Overflow detected, buffer copy aborted\n";

		// Return if buffer is not ready
		if (!buffer.isOverLimit())
			return true;

		// Send buffer
		result = SDLNet_TCP_Send(socket, buffer.buffer, buffer.real_size);

		// Clear buffer
		buffer.clear();
	}
	// Send directly
	else
	{
		result = SDLNet_TCP_Send(socket, &len, sizeof(size_t));
		result = SDLNet_TCP_Send(socket, packet, len);
	}

	//std::cout << "Sent " << result << " bytes of data\n";

	if (result < len)
	{
		std::cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << "\n";
		return false;
	}

	//std::cout << "Pair packet sent succesfully \n";
	return true;
}

Packet* NetworkAgent::recvPacket(TCPsocket socket)
{
	if (!socket)
	{
		std::cout << "Can't receive packet, socket is not opened \n";
		return nullptr;
	}

	// Read packet length
	size_t packet_len = 0;
	int data_read = SDLNet_TCP_Recv(socket, &packet_len, sizeof(size_t));

	// Read data
	Packet* packet = (Packet*)std::malloc(packet_len);
	data_read = SDLNet_TCP_Recv(socket, packet, packet_len);

	//std::cout << "Received " << data_read << " bytes of data\n";

	if (data_read <= 0)
	{
		std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << "\n";
		return nullptr;
	}

	//std::cout << "Pair packet: " << len << " recv succesfully \n";
	return packet;
}

bool NetworkAgent::sendPacket(Packet * packet, bool buffered)
{
	return false;
}

Packet* NetworkAgent::recvPacket()
{
	return nullptr;
}

void NetworkAgent::destroy()
{
	// Virtual hook
	beforeDestroy();

	// Reset flag
	printf("Destroying network agent\n");

	// Calling SDL
	SDLNet_Quit();

	// Destroying pointer
	this->~NetworkAgent();
}

void NetworkAgent::beforeDestroy()
{
	printf("Parent method\n");
}