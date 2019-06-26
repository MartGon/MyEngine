#include "NetworkAgent.h"
#pragma once

class NetworkClient : public NetworkAgent
{
public:

	enum ClientState
	{
		CLIENT_NULL,
		CLIENT_READ_CONFIG_FILE,
		CLIENT_OPENING_SOCKET,
		CLIENT_SENDING,
		CLIENT_RECEIVING,
		CLIENT_CONNECTION_ESTABLISHED
	};

	// Constructors

	NetworkClient();
	~NetworkClient() override;

	// Attributes

		// State
	ClientState state = CLIENT_OPENING_SOCKET;

		// Client side
	TCPsocket clientSocket = nullptr;

		// Server side
	std::string serverIP;
	Uint16 serverPort;

	// Methods
		// Upper
	virtual bool readConfigFile();
	virtual bool establishConnection();
	void handleDisconnect(TCPsocket socket) override;

			// Communication
	virtual bool sendPacket(Packet* packet, bool buffered = false) override;
	virtual Packet* recvPacket();
	std::vector<Packet*> recvPackets() override;

		// Own
	bool openClientSocket();
		
	// Other
	virtual void beforeDestroy();
};

