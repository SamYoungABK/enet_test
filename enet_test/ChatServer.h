#pragma once
#include <enet/enet.h>
#include <string>
#include <vector>

using std::vector; using std::string;

class ChatServer
{

	ENetAddress address;
	ENetHost* server;
	vector<string> connectedUsers;

	void CreateServer();
	void HandleConnection(ENetEvent e);
	void ServerLoop();

	void ParsePacket(ENetEvent* e);
	void HandleMessagePacket(ENetEvent* e);
	void HandleJoinPacket(ENetEvent* e);
	void HandleWhoPacket(ENetEvent* e);

	void HandleDisconnect(ENetEvent* e);

	bool IsNameTaken(string name);
public:
	void RunServer();
	void SetAddress(ENetAddress addr) { address = addr; }
};

