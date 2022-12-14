#pragma once
#include <vector>
#include <string>
#include <enet/enet.h>
#include <thread>

using namespace std;

class ChatClient
{
	thread* t_kbListen = nullptr;
	thread* t_draw = nullptr;

	ENetAddress address;

	vector<string> chatLog;
	string clientName;
	ENetPeer* server;
	ENetHost* client;

	void DrawScreen();
	void KbListen();

	void InputClientName();
	void Connect();
	void CreateClient();
	void CreateServerConnection();
	void StartThreads();
	
	void SendJoinPacket();
	void SendMessagePacket(string message);
	void SendWhoPacket();
	void SendWhisperPacket(const char* receiver,const char* whisperMsg);

	void ParsePacket(ENetPacket* p);
	void HandleMessagePacket(ENetPacket* p);
	void HandleJoinPacket(ENetPacket* p);

	void ParseCommand(string message);

	void ClientLoop();
public:
	void RunClient();

	void SetAddress(ENetAddress addr) { address = addr; }
};

