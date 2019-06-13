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

	// Allocat a socket set
	socket_set = SDLNet_AllocSocketSet(1);
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

	// Should not do this
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
		size_t data_size = sizeof(size_t) + len;
		NetworkBuffer buffer = NetworkBuffer(data_size);

		buffer.append_data(&len, sizeof(size_t));
		buffer.append_data(packet, len);

		result = SDLNet_TCP_Send(socket, buffer.buffer, buffer.real_size);
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

	// If not blocking, we do a SocketSet check for activity
	if (!isBlocking)
	{
		// Check for activity
		Uint32 result = SDLNet_CheckSockets(socket_set, 0);
		if (result == -1)
		{
			std::cout << "SDLNet_CheckSockets: failed\n";
			return nullptr;
		}
		if (result == 0)
		{
			//std::cout << "SDLNet_CheckSockets: No activiy ";
			return nullptr;
		}
		if (!SDLNet_SocketReady(socket))
		{
			std::cout << "SDLNet_SocketReady: Socket was not ready";
			return nullptr;
		}
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

	// Calculate time diff
	std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::milliseconds diff = now - this->ping;
	ping = now;

	//std::cout << "Ping rate is " << diff.count() << " ms\n";

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