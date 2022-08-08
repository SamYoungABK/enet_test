#include "ChatServer.h"
#include <enet/enet.h>
#include <iostream>
#include <string>
#include <vector>

using std::cout;   using std::endl;
using std::string; using std::vector;

void ChatServer::CreateServer()
{
	server = enet_host_create(&address, 2, 2, 0, 0);

	if (server == NULL)
	{
		cout << "Server creation error :(" << endl;
		exit(EXIT_FAILURE);
	}
}

void ChatServer::HandleConnection(ENetEvent e)
{
	cout << "A new client connected from " <<
		e.peer->address.host << ':' << e.peer->address.port << endl << endl;
}

void ChatServer::ServerLoop()
{
	bool hosting = true;
	while (hosting)
	{
		ENetEvent event;
		while (enet_host_service(server, &event, 100000))
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				HandleConnection(event);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				ParsePacket(&event);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				HandleDisconnect(&event);
				break;
			}
		}
	}
}

void ChatServer::ParsePacket(ENetEvent* e)
{
	char opCode = e->packet->data[0];
	switch (opCode)
	{
	case 'j': HandleJoinPacket(e); break;
	case 'm': HandleMessagePacket(e); break;
	case 'w': HandleWhoPacket(e); break;
	}
}

void ChatServer::HandleMessagePacket(ENetEvent* e)
{
	cout << "Received message: " << e->packet->data + 1 << endl;
	enet_host_broadcast(server, 0, e->packet);
}

void ChatServer::HandleJoinPacket(ENetEvent* e)
{
	cout << "User joined: " << e->packet->data + 1 << endl;
	connectedUsers.push_back((char*)(e->packet->data + 1));
	enet_host_broadcast(server, 0, e->packet);
}

void ChatServer::HandleWhoPacket(ENetEvent* e)
{
	cout << "User list requested" << endl;
	string response = "mConnected users: ";
	
	for (string user : connectedUsers)
	{
		response += (user);
		
		if (user != connectedUsers.back())
			response += ", ";
	}

	ENetPacket* packet = enet_packet_create(response.c_str(),
		strlen(response.c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send((e->peer), 0, packet);
}

void ChatServer::HandleDisconnect(ENetEvent* e)
{
	cout << (char*)(e->peer->data) << " disconnected. " << endl;
	e->peer->data = NULL;
}

void ChatServer::RunServer()
{
	CreateServer();
	ServerLoop();

	enet_host_destroy(server);
}