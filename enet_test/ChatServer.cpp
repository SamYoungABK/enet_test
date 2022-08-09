#include "ChatServer.h"
#include <enet/enet.h>
#include <iostream>
#include <string>
#include <vector>

using std::cout;   using std::endl;
using std::string; using std::vector;

void ChatServer::CreateServer()
{
	server = enet_host_create(&address, 10, 2, 0, 0);

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
	
	enet_peer_timeout(e.peer, 1000, 1000, 1500);
}

void ChatServer::ServerLoop()
{
	bool hosting = true;
	while (hosting)
	{
		ENetEvent event;
		while (enet_host_service(server, &event, 1000))
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
	case 'W': HandleWhisperPacket(e); break;
	}
}

void ChatServer::HandleMessagePacket(ENetEvent* e)
{
	cout << "Received message: " << e->packet->data + 1 << endl;
	enet_host_broadcast(server, 0, e->packet);
}

// TODO: Needs Refactoring
void ChatServer::HandleJoinPacket(ENetEvent* e)
{
	string userName = (char*)(e->packet->data + 1);
	if (IsNameTaken(userName))
	{
		cout << "Duplicate name tried to join: " << userName;

		ENetPacket* nameTakenPacket = enet_packet_create("jNameTaken",
			strlen("jNameTaken") + 1,
			ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send((e->peer), 0, nameTakenPacket);
	}
	else
	{
		ENetPacket* nameAcceptedPacket = enet_packet_create("jNameAccepted",
			strlen("jNameAccepted") + 1,
			ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send((e->peer), 0, nameAcceptedPacket);

		cout << "User joined: " << userName << endl;
		User newUser(e->peer, userName);
		connectedUsers.push_back(newUser);

		string userJoinMessage = "mUser joined: " + userName;

		ENetPacket* joinMessagePacket = enet_packet_create(userJoinMessage.c_str(),
			strlen(userJoinMessage.c_str()) + 1,
			ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, joinMessagePacket);
	}
}

void ChatServer::HandleWhoPacket(ENetEvent* e)
{
	cout << "User list requested" << endl;
	string response = "mConnected users: ";
	
	for (User u: connectedUsers)
	{
		response += (u.GetName());
		
		if (u.GetName() != connectedUsers.back().GetName())
			response += ", ";
	}

	ENetPacket* packet = enet_packet_create(response.c_str(),
		strlen(response.c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send((e->peer), 0, packet);
}

// TODO: Needs refactoring
void ChatServer::HandleWhisperPacket(ENetEvent* e)
{
	User sender = *(GetUserFromPeer(e->peer));
	char receiverNameSize;
	char* tmp;
	string receiverName;
	string message;
	User receiver;
	string response;
	
	receiverNameSize = *((char*)(e->packet->data) + 1);
	tmp = new char[receiverNameSize+1];
	strncpy_s(tmp, receiverNameSize + 1, (char*)(e->packet->data + 2), receiverNameSize);
	tmp[receiverNameSize] = '\0';
	receiverName = tmp;
	message = (char*)(e->packet->data + 2 + receiverNameSize);
	if (!IsNameTaken(receiverName))
	{
		response = "mUser not found: " + receiverName;
		ENetPacket* packet = enet_packet_create(response.c_str(),
			strlen(response.c_str()) + 1,
			ENET_PACKET_FLAG_RELIABLE);

		enet_peer_send((e->peer), 0, packet);
		return;
	}
	receiver = *(GetUserFromName(receiverName));
	response = "m[" + sender.GetName() + "->" + receiver.GetName() + "]: " + message;
	
	ENetPacket* packet = enet_packet_create(response.c_str(),
		strlen(response.c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send((e->peer), 0, packet);
	enet_peer_send((receiver.GetPeer()), 0, packet);
}

void ChatServer::HandleDisconnect(ENetEvent* e)
{
	vector<User>::iterator disconnectingUser = GetUserFromPeer(e->peer);
	cout << disconnectingUser->GetName() << " disconnected. " << endl;
	connectedUsers.erase(disconnectingUser);
}

vector<User>::iterator ChatServer::GetUserFromName(string name)
{
	for (vector<User>::iterator it = connectedUsers.begin(); it < connectedUsers.end(); it++)
		if (it->GetName() == name)
			return it;
	return connectedUsers.end();
}

vector<User>::iterator ChatServer::GetUserFromPeer(ENetPeer* p)
{
	for (vector<User>::iterator it=connectedUsers.begin(); it < connectedUsers.end(); it++)
		if (it->GetPeer() == p)
			return it;
	return connectedUsers.end();
}

bool ChatServer::IsNameTaken(string name)
{
	for (User u : connectedUsers)
	{
		if (u.GetName() == name)
			return true;
	}
	return false;
}

void ChatServer::RunServer()
{
	CreateServer();
	ServerLoop();

	enet_host_destroy(server);
}