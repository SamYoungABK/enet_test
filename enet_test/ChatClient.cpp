#include "ChatClient.h"
#include <vector>
#include <string>
#include <enet/enet.h>
#include <iostream>
#include <thread>

using std::cout;   using std::endl;
using std::string; using std::vector;
using std::thread;

void ChatClient::DrawScreen()
{
	int chatLogSize = chatLog.size();
	bool drawing = true;
	while (drawing)
	{
		if (chatLogSize != chatLog.size())
		{
			system("cls");
			cout << "Connected to chat server on localhost:1234" << endl;
			cout << "You are " << clientName << endl;
			for (int i = 0; i < 80; i++) cout << char(219);
			cout << endl;
			for (string msg : chatLog)
			{
				cout << '\t' << msg << endl;
			}

			cout << endl << endl << "Enter Message: ";
			chatLogSize = chatLog.size();
		}
	}
}

void ChatClient::KbListen()
{
	bool listening = true;
	while (listening)
	{
		string message;
		getline(cin, message);

		SendMessagePacket(message);
	}
}

void ChatClient::InputClientName()
{
	std::cout << "Enter client name: ";
	getline(cin, clientName);
}

void ChatClient::CreateClient()
{
	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (!client) exit(EXIT_FAILURE);
}

void ChatClient::CreateServerConnection()
{
	server = enet_host_connect(client, &address, 2, 0);
	if (!server) exit(EXIT_FAILURE);
}

void ChatClient::Connect()
{
	CreateClient();
	CreateServerConnection();

	ENetEvent event;
	if ((enet_host_service(client, &event, 5000) > 0) &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		cout << "Connected!" << endl;
		SendJoinPacket();
	}
	else {
		cout << "Unable to connect to server!" << endl;
		exit(EXIT_FAILURE);
	}
}

void ChatClient::StartThreads()
{
	if (t_kbListen == nullptr) t_kbListen = new thread(&ChatClient::KbListen, this);
	if (t_draw == nullptr) t_draw = new thread(&ChatClient::DrawScreen, this);
}

void ChatClient::SendJoinPacket()
{
	string dataToSend = "j" + clientName;

	ENetPacket* packet = enet_packet_create(dataToSend.c_str(),
		strlen(dataToSend.c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(server, 0, packet);
}

void ChatClient::SendMessagePacket(string message)
{
	string dataToSend = "m" + clientName + ": " + message;

	ENetPacket* packet = enet_packet_create(dataToSend.c_str(),
		strlen(dataToSend.c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(server, 0, packet);
}

void ChatClient::ParsePacket(ENetPacket* p)
{
	switch (p->data[0])
	{
	case 'm':
		HandleMessagePacket(p);
		break;
	case 'j':
		HandleJoinPacket(p);
		break;
	}
}

void ChatClient::HandleMessagePacket(ENetPacket* p)
{
	chatLog.push_back((char*)p->data + 1);
}

void ChatClient::HandleJoinPacket(ENetPacket* p)
{
	chatLog.push_back("User joined: " + string((char*)p->data + 1));
}

void ChatClient::ClientLoop()
{
	ENetEvent event;
	bool connected = true;
	while (connected)
	{
		enet_host_service(client, &event, 1000);
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			ParsePacket(event.packet);
		}
	}
}


void ChatClient::RunClient()
{
	InputClientName();

	ENetAddress address;

	enet_address_set_host(&address, "localhost");
	address.port = 1234;

	Connect();

	StartThreads();

	ClientLoop();
}