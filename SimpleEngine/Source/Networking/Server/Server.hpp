#pragma once
#include "Networking/NetworkShared.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>

namespace Simple
{
	class Server final
	{
		struct ClientUser
		{
			std::string name = "Client";
			sockaddr_in address{};
			bool isConnected = false;
		};

	public:
		Server();
		~Server();

		bool Init();
		bool Update();

	private:
		void CheckInput();
		void ConnectUser(const int aClientPort);
		void DisconnectUser(const int aClientPort);
		void CheckMessageFromClient(const int aClientPort);
		void SendMessageToAllClients();
	private:
		char mySocketBuffer[NETMESSAGE_SIZE];
		char myMessage[NETMESSAGE_SIZE];

		// We'll need a socket to communicate on.
		SOCKET myUDPSocket;

		// And we'll need the Winsock data object.
		WSADATA myWinsockData;

		// Address information we'll use to bind.
		sockaddr_in myAddressServer;

		// Address information of connecting client.
		sockaddr_in myAddressClient;

		// We also need the size when receiving.
		int myAddressClientSize;

		std::unordered_map<int, ClientUser> myClients;
		std::thread myInputThread;
		std::atomic<bool> myIsRunning;
	};
}