#pragma once
#include <enet/enet.h>
#include <string>
#include <vector>
#include "User.h"


using std::vector; using std::string;

class ChatServer
{

	ENetAddress address;
	ENetHost* server;
	vector<User> connectedUsers;

	void CreateServer();
	void HandleConnection(ENetEvent e);
	void ServerLoop();

	void ParsePacket(ENetEvent* e);
	void HandleMessagePacket(ENetEvent* e);
	void HandleJoinPacket(ENetEvent* e);
	void HandleWhoPacket(ENetEvent* e);
	void HandleWhisperPacket(ENetEvent* e);

	void HandleDisconnect(ENetEvent* e);

	vector<User>::iterator GetUserFromName(string name);
	vector<User>::iterator GetUserFromPeer(ENetPeer* p);
	bool IsNameTaken(string name);
public:
	void RunServer();
	void SetAddress(ENetAddress addr) { address = addr; }
};

