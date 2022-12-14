#pragma once
#include <enet/enet.h>
#include <string>

using std::string;

static constexpr int cycleTimeout = 10;

class User
{
	ENetPeer* m_peer;
	string m_userName;
	bool m_isAdmin = false;

public:
	User(ENetPeer* peer, string userName) :
		m_peer(peer),
		m_userName(userName)
	{};

	User() :
		m_peer(nullptr),
		m_userName("")
	{};

	void SetPeer(ENetPeer* peer) { m_peer = peer; }
	ENetPeer* GetPeer() { return m_peer; }
	void SetName(string name) { m_userName = name; }
	string GetName() { return m_userName; }
	void makeAdmin() { m_isAdmin = true; }
	bool isAdmin() { return m_isAdmin; }
};

