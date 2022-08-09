#include <iostream>
#include <enet/enet.h>
#include <string>
#include "ChatClient.h"
#include "ChatServer.h"

using namespace std;

void InitializeEnet();

int main()
{
	string userInput;
	ChatClient* c = nullptr;
	ChatServer* s = nullptr;

	cout << "Server or client?" << endl <<
		"1. server " << endl <<
		"2. client" << endl <<
		"Selection (1,2,3): ";
	getline(cin, userInput);
	int num = int(userInput[0]-'0');

	ENetAddress addr;

	InitializeEnet();
	switch (num)
	{
	case 1:
		addr.host = ENET_HOST_ANY;
		addr.port = 1234;
		s = new ChatServer;
		s->SetAddress(addr);
		s->RunServer();
		break;
	case 2:
		enet_address_set_host(&addr, "localhost");
		addr.port = 1234;
		c = new ChatClient;
		c->SetAddress(addr);
		c->RunClient();
		break;
	default:
		cout << "Invalid selection." << endl;
		exit(EXIT_FAILURE);
	}
}

void InitializeEnet()
{
	if (enet_initialize() != 0)
	{
		cout << "Error initializing enet." << endl;
		exit(EXIT_FAILURE);
	}
	atexit(enet_deinitialize);
}