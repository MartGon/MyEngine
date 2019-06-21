#include "NetworkAgent.h"
#pragma once

class NetworkServer : public NetworkAgent
{
public:

	enum ServerState
	{
		SERVER_STATE_NULL,
		SERVER_READ_CONFIG_FILE,
		SERVER_STATE_OPENING_SOCKET,
		SERVER_STATE_PAIRING,
		SERVER_STATE_SENDING,
		SERVER_STATE_RECEIVING,
		SERVER_STATE_CONNECTION_ESTABLISHED
	};

	// Constructors
	NetworkServer();
	~NetworkServer();

	// Attributes

		// State
	ServerState state;

		// Server side
	TCPsocket serverSocket = nullptr;
	Uint16 serverPort;

		// Client side
	std::vector<TCPsocket> client_sockets;

	// Methods
		// Uppper
	virtual bool readConfigFile();
	virtual bool establishConnection();
	void handleDisconnect(TCPsocket socket) override;

		// Own
	bool openServerSocket();
	bool pairWithClient();

		// Communication
	bool sendPacket(Packet* packet, bool buffered = true) override;
	bool sendPacket(int client_index, Packet* packet, bool buffered = true);
	Packet* recvPacket() override;
	Packet* recvPacket(int client_index);
	std::vector<Packet*> recvPackets() override;

	// Other
	virtual void beforeDestroy();
};

