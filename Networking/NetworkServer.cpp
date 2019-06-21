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

bool NetworkServer::pairWithClient()
{
	if (!serverSocket)
	{
		std::cout << "Can't send packet, read socket is not opened \n";
		return false;
	}

	if (client_sockets.size() == getPairsAmount())
	{
		std::cout << "Client already paired \n";
		return true;
	}

	TCPsocket clientSocket = nullptr;
	if (clientSocket = SDLNet_TCP_Accept(serverSocket))
	{
		std::cout << " Paired succesfully with client \n";

		// Add socket to socket set
		if (SDLNet_TCP_AddSocket(socket_set, clientSocket) == -1)
		{
			std::cout << " Add socket failded \n";
			return false;
		}

		// Add to socket list
		client_sockets.push_back(clientSocket);

		return true;
	}

	std::cout << " Could not pair with client \n";
	return false;
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
		if (pairWithClient())
		{
			// Send identity
			int client_index = client_sockets.size();
			SyncPacket sync(client_index, Random::seed);
			sync.frame_buffer = max_buffer_size;
			sync.player_amount = player_amount;
			sendPacket(client_index - 1, &sync, false);

			// Go to next step when paired with all expected clients
			if (client_sockets.size() == getPairsAmount())
				state = SERVER_STATE_CONNECTION_ESTABLISHED;

			// Call handler
			callHandleNAEvent(EVENT_PAIR_CONNECTED);
		}
		break;
	case SERVER_STATE_SENDING:
		// Give each pair each identity
		for (int client_index = 1; client_index < client_sockets.size() + 1; client_index++)
			if (sendPacket(client_index - 1, new SyncPacket(client_index, Random::seed), false))
				state = SERVER_STATE_CONNECTION_ESTABLISHED;
		break;
	case SERVER_STATE_RECEIVING:
			state = SERVER_STATE_CONNECTION_ESTABLISHED;
		break;
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
	auto to_remove = std::find(client_sockets.begin(), client_sockets.end(), socket);

	if(to_remove != client_sockets.end())
	{

		client_sockets.erase(to_remove);
	}
}

bool NetworkServer::sendPacket(int client_index, Packet *packet, bool buffered)
{
	// Don't send packet to its owner
	if (packet->owner_identity == client_index + 1)
		return true;

	TCPsocket clientSocket = client_sockets.at(client_index);
	return NetworkAgent::sendPacket(clientSocket, packet, buffered);
}

bool NetworkServer::sendPacket(Packet* packet, bool buffered)
{
	bool result = true;
	for (int client_index = 0; client_index < client_sockets.size(); client_index++)
		result = sendPacket(client_index, packet, buffered) && result;
	return result;
}

// Dummy function
Packet* NetworkServer::recvPacket()
{
	return nullptr;
}

Packet* NetworkServer::recvPacket(int client_index)
{
	TCPsocket clientSocket = client_sockets.at(client_index);

	return NetworkAgent::recvPacket(clientSocket);
}

std::vector<Packet*> NetworkServer::recvPackets()
{
	std::vector<Packet*> packets;

	int client_index = 0;
	while(client_index < client_sockets.size())
	{
		Packet* packet = recvPacket(client_index);

		// If packet is not null
		if (!packet)
		{
			if(last_event == EVENT_NO_ACTIVITY)
				client_index++;
		}

		packets.push_back(packet);
	}

	return packets;
}

void NetworkServer::beforeDestroy()
{
	// Destroy server socket
	SDLNet_TCP_Close(serverSocket);

	// Destroy client sockets
	for(auto clientSocket : client_sockets)
		if(clientSocket)
			SDLNet_TCP_Close(clientSocket);
}