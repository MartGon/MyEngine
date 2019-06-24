#include "NetworkServer.h"
#include "Random.h"

// Constructors

NetworkServer::NetworkServer() : NetworkAgent()
{
	state = SERVER_READ_CONFIG_FILE;
}

NetworkServer::~NetworkServer()
{
	beforeDestroy();
}

bool NetworkServer::readConfigFile()
{
	// Open File
	std::string configFileName = "resources/server-config.txt";
	std::ifstream configFile(configFileName);

	if (!configFile)
	{
		std::cout << "Couldn't open " << configFileName << "\n";
		return false;
	}

	// Line for reading
	std::string line;

	// Read Server Port
	std::getline(configFile, line);
	serverPort = std::stoi(line);
	std::cout << "Server port " << serverPort << "\n";

	// Read frame buffer size
	std::getline(configFile, line);
	max_buffer_size = std::stoi(line);
	std::cout << "Frame buffer size: " << max_buffer_size << "\n";

	// Read player amount
	std::getline(configFile, line);
	player_amount = std::stoi(line);
	std::cout << "Player amount: " << player_amount<< "\n";

	// Allocat a socket set
	if(player_amount >= 2)
		socket_set = SDLNet_AllocSocketSet(getPairsAmount());

	return true;
}

bool NetworkServer::openServerSocket()
{
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, NULL, serverPort) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << "\n";
		return false;
	}

	serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}

	std::cout << "TCP Read Port " << serverPort << " Opened Successfully! \n";
	return true;
}

Uint32 NetworkServer::pairWithClient()
{
	if (!serverSocket)
	{
		std::cout << "Can't send packet, read socket is not opened \n";
		return 0;
	}

	if (client_sockets.size() == getPairsAmount())
	{
		std::cout << "Client already paired \n";
		state = SERVER_STATE_CONNECTION_ESTABLISHED;
		return 0;
	}

	TCPsocket clientSocket = nullptr;
	if (clientSocket = SDLNet_TCP_Accept(serverSocket))
	{
		std::cout << " Paired succesfully with client \n";

		// Add socket to socket set
		if (SDLNet_TCP_AddSocket(socket_set, clientSocket) == -1)
		{
			std::cout << " Add socket failded \n";
			return 0;
		}

		// Add to socket list
		int index = getFirstFreeIdentity();
		client_sockets.insert({ index, clientSocket });

		return index;
	}

	std::cout << " Could not pair with client \n";
	return 0;
}

bool NetworkServer::establishConnection()
{
	switch (state)
	{
	case SERVER_READ_CONFIG_FILE:
		if (readConfigFile())
			state = SERVER_STATE_OPENING_SOCKET;
		break;
	case SERVER_STATE_OPENING_SOCKET:
		if (openServerSocket())
			state = SERVER_STATE_PAIRING;
		break;
	case SERVER_STATE_PAIRING:
	{
		// Send identity
		int client_index = pairWithClient();

		if (client_index != 0)
		{
			SyncPacket sync(client_index, Random::seed, player_amount, max_buffer_size);
			sendPacket(client_index, &sync, false);

			// Go to next step when paired with all expected clients
			if (client_sockets.size() == getPairsAmount())
				state = SERVER_STATE_CONNECTION_ESTABLISHED;

			// Call handler
			callHandleNAEvent(EVENT_PAIR_CONNECTED);
		}
		break;
	}
	case SERVER_STATE_CONNECTION_ESTABLISHED:
		return true;
	default:
		break;
	}

	return false;
}

void NetworkServer::handleDisconnect(TCPsocket socket)
{
	// Remove socket from list
	auto to_remove = std::find_if(client_sockets.begin(), client_sockets.end(), [&](auto p) {return p.second == socket;  });

	if(to_remove != client_sockets.end())
	{
		client_sockets.erase(to_remove);
	}

	// Close socket
	SDLNet_TCP_DelSocket(socket_set, socket);
	SDLNet_TCP_Close(socket);
}

bool NetworkServer::sendPacket(int client_index, Packet *packet, bool buffered)
{
	// Don't send packet to its owner
	if (packet->owner_identity == client_index)
		return true;

	TCPsocket clientSocket = client_sockets.at(client_index);
	return NetworkAgent::sendPacket(clientSocket, packet, buffered);
}

bool NetworkServer::sendPacket(Packet* packet, bool buffered)
{
	bool result = true;
	for (auto pair : client_sockets)
		result = sendPacket(pair.first, packet, buffered) && result;
	return result;
}

// Dummy function
Packet* NetworkServer::recvPacket()
{
	return nullptr;
}

Packet* NetworkServer::recvPacket(int client_index)
{
	TCPsocket clientSocket = nullptr;

	if(client_sockets.find(client_index) != client_sockets.end())
		clientSocket = client_sockets.at(client_index);

	if (!clientSocket)
		return nullptr;

	return NetworkAgent::recvPacket(clientSocket);
}

std::vector<Packet*> NetworkServer::recvPackets()
{
	std::vector<Packet*> packets;

	int client_index = getNextAvailableIndex(0);
	while(true)
	{
		Packet* packet = recvPacket(client_index);
		packets.push_back(packet);

		// Break if last index
		if (isLastIndex(client_index))
			break;

		client_index = getNextAvailableIndex(client_index);
	}

	return packets;
}

void NetworkServer::beforeDestroy()
{
	// Destroy server socket
	SDLNet_TCP_Close(serverSocket);

	// Destroy client sockets
	for(auto clientSocket : client_sockets)
		if(clientSocket.second)
			SDLNet_TCP_Close(clientSocket.second);
}

Uint32 NetworkServer::getFirstFreeIdentity()
{
	if (client_sockets.empty())
		return 1;

	for (int i = 1; i <= (client_sockets.size() + 1); i++)
		if (client_sockets.find(i) == client_sockets.end())
			return i;

	return -1;
}

Uint32 NetworkServer::getNextAvailableIndex(int index)
{
	for (auto pair : client_sockets)
	{
		if (int next_index = pair.first; next_index > index)
			return next_index;
	}
	
	return index;
}

bool NetworkServer::isLastIndex(int index)
{
	int max_index = 0;

	for (auto pair : client_sockets)
		max_index = pair.first > max_index ? pair.first : max_index;

	return index >= max_index;
}