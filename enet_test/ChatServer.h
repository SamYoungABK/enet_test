#include <enet/enet.h>
#include <string>

#pragma once
class ChatServer
{

	ENetAddress address;
	ENetHost* server;

	void CreateServer();
	void HandleConnection(ENetEvent e);
	void ServerLoop();

	void ParsePacket(ENetEvent* e);
	void HandleMessagePacket(ENetEvent* e);
	void HandleJoinPacket(ENetEvent* e);

	void HandleDisconnect(ENetEvent* e);

public:
	void RunServer();
	void SetAddress(ENetAddress addr) { address = addr; }
};

