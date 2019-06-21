#include "NetworkClient.h"
#include "Random.h"

// Constructors

NetworkClient::NetworkClient() : NetworkAgent()
{
	state = CLIENT_READ_CONFIG_FILE;

	// Allocat a socket set
	socket_set = SDLNet_AllocSocketSet(1);
}

NetworkClient::~NetworkClient()
{
	beforeDestroy();
}

bool NetworkClient::readConfigFile()
{
	// Open File
	std::string configFileName = "resources/client-config.txt";
	std::ifstream configFile(configFileName);

	if (!configFile)
	{
		std::cout << "Couldn't open " << configFileName << "\n";
		return false;
	}

	// Line for reading
	std::string line;

	// Read Server IP
	std::getline(configFile, serverIP);
	std::cout << "Read IP " << serverIP << "\n";

	// Read Server Port
	std::getline(configFile, line);
	serverPort = std::stoi(line);
	std::cout << "Server port " << serverPort << "\n";

	// Read frame window size
	std::getline(configFile, line);
	max_buffer_size = std::stoi(line);
	std::cout << "Frame buffer size: " << max_buffer_size << "\n";

	// Read player amount
	std::getline(configFile, line);
	player_amount = std::stoi(line);
	std::cout << "Player amount: " << player_amount << "\n";

	// Allocat a socket set
	socket_set = SDLNet_AllocSocketSet(1);

	return true;
}

bool NetworkClient::openClientSocket()
{
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, serverIP.c_str(), serverPort) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << "\n";
		return false;
	}

	clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}

	if (SDLNet_TCP_AddSocket(socket_set, clientSocket) == -1)
	{
		std::cout << " Add socket failded \n";
		return false;
	}

	std::cout << "TCP Write Port " << serverPort << " Opened Successfully! \n";
	return true;
}

bool NetworkClient::establishConnection()
{
	switch (state)
	{
	case CLIENT_READ_CONFIG_FILE:
		if (readConfigFile())
			state = CLIENT_OPENING_SOCKET;
		break;
	case CLIENT_OPENING_SOCKET:
		if (openClientSocket())
			state = CLIENT_RECEIVING;
		break;
	case CLIENT_SENDING:
		if (sendPacket(new Packet(), false))
			state = CLIENT_CONNECTION_ESTABLISHED;
		break;
	case CLIENT_RECEIVING:
		if (Packet* packt = recvPacket())
		{
			if (packt->packetType == PacketType::SYNC_PACKET)
			{
				if (SyncPacket* syn = static_cast<SyncPacket*>(packt))
				{
					// Set identity
					pair_identity = syn->identity_to_set;
					std::cout << "My pair identity is: " << (int)pair_identity << "\n";

					// Set RNG Seed
					Random::setSeed(syn->seed);

					// Set buffer size
					max_buffer_size = syn->frame_buffer;

					// Set player amount
					player_amount = syn->player_amount;

					state = CLIENT_CONNECTION_ESTABLISHED;
				}
			}
		}
		break;
	case CLIENT_CONNECTION_ESTABLISHED:
		return true;
	default:
		break;
	}

	return false;
}

void NetworkClient::handleDisconnect(TCPsocket socket)
{
	state = CLIENT_OPENING_SOCKET;

	SDLNet_TCP_Close(socket);
	socket = nullptr;
}

bool NetworkClient::sendPacket(Packet* packet, bool buffered)
{
	return NetworkAgent::sendPacket(clientSocket, packet, buffered);
}

Packet* NetworkClient::recvPacket()
{
	return NetworkAgent::recvPacket(clientSocket);
}

std::vector<Packet*> NetworkClient::recvPackets()
{
	std::vector<Packet*> packets;

	for (int i = 0; i < getPairsAmount(); i++)
	{
		Packet* packet = recvPacket();
		packets.push_back(packet);
	}

	return packets;
}

void NetworkClient::beforeDestroy()
{
	SDLNet_TCP_Close(clientSocket);
}