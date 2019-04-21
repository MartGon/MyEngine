#include "NetworkAgent.h"

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

bool NetworkAgent::sendPacket(TCPsocket socket, Packet* packet)
{
	if (!socket)
	{
		std::cout << "Can't send packet, socket is not opened \n";
		return false;
	}

	int result;
	size_t len = packet->getSize();
	// Send packet size
	result = SDLNet_TCP_Send(socket, &len, sizeof(size_t));

	// Send packet
	result = SDLNet_TCP_Send(socket, packet, len);

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

	if (data_read <= 0)
	{
		std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << "\n";
		return nullptr;
	}

	//std::cout << "Pair packet: " << len << " recv succesfully \n";
	return packet;
}

bool NetworkAgent::sendPacket(Packet * packet)
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