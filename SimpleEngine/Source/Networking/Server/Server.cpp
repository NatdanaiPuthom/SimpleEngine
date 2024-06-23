#include "Networking/Server/Server.hpp"
#include <iostream>
#include <cassert>

#define WIN32_LEAN_AND_MEAN //NOTE(v11.2.2): Fuck you Microsoft
#include <Windows.h>

#define LISTEN_PORT 42000

namespace Simple
{
	Server::Server()
		: myUDPSocket{}
		, myWinsockData{}
		, myAddressServer{}
		, myAddressClient{}
		, mySocketBuffer{}
		, myAddressClientSize(sizeof(myAddressClient))
		, myIsRunning(true)
	{
		std::memset(mySocketBuffer, '\0', sizeof(mySocketBuffer));
		std::memset(myMessage, '\0', sizeof(myMessage));
	}

	Server::~Server()
	{
		ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

		if (myInputThread.joinable())
		{
			myInputThread.join();
		}

		WSACleanup();
	}

	bool Server::Init()
	{
		myAddressClientSize = sizeof(myAddressClient);

		std::cout << "Starting Winsock...";

		if (WSAStartup(MAKEWORD(2, 2), &myWinsockData) != 0)
		{
			std::cout << "\nFAIL!" << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			assert(false && "Failed to start winsock");
			return false;
		}

		std::cout << "OK!" << std::endl;

		myUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (myUDPSocket == INVALID_SOCKET)
		{
			std::cout << "\nFailed to create socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			assert(false && "Failed to create socket");
			return false;
		}

		// Populate the server address information
		myAddressServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc
		myAddressServer.sin_addr.s_addr = INADDR_ANY;            // Don't care which IP address to use. Any available.
		// Computers usually have at least two when connected
		// to a network. One loopback and one network address.
		// 127.0.0.1 - Built-in loopback adapter.
		// e.g. 192.168.0.1 - Class-C address from a router.

		myAddressServer.sin_port = htons(LISTEN_PORT);   // The port we want to listen for connections on.
		// htons converts the byte order from host order to
		// network order.

		// We need to bind to a specific address and port combination. This tells the
		// operating system that we want communication on that combination to go to
		// this program.
		if (bind(myUDPSocket, reinterpret_cast<sockaddr*>(&myAddressServer), sizeof(myAddressServer)) == SOCKET_ERROR)
		{
			std::cout << "\nFailed to bind socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			assert(false && "Failed to bind socket");
			return false;
		}

		// We want to be able to shut down gracefully and right now we use blocking receive
		// so we should set up a thread to listen for keyboard input.
		myInputThread = std::thread(&Server::CheckInput, this);	

		// If we got this far we should now have an open socket ready to receive information from the network.
		std::cout << "\nType \"quit\" to exit..." << std::endl;

		return true;
	}

	bool Server::Update()
	{
		if (myIsRunning == false)
		{
			return false;
		}

		// Clear the buffer.
		ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

		// blocking receive. This function will block until a message is received.
		const int recv_len = recvfrom(myUDPSocket, mySocketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&myAddressClient, &myAddressClientSize);

		if (recv_len == SOCKET_ERROR)
		{
			std::cout << "\nFailed receiving data from socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;

			const int clientPort = ntohs(myAddressClient.sin_port);
			myClients.erase(clientPort);
		}

		if (recv_len > 0)
		{
			// Extract the address information from the incoming data.
			// 16 bytes is enough for an IPv4 address.
			// i.e. "xxx.xxx.xxx.xxx" + string terminator

			char clientMessage[NETMESSAGE_SIZE]{};

			strcpy_s(clientMessage, mySocketBuffer);

			char clientAddress[16]{ '\0' };
			inet_ntop(AF_INET, &myAddressClient.sin_addr, &clientAddress[0], sizeof(clientAddress));
			const int clientPort = ntohs(myAddressClient.sin_port);

			auto it = myClients.find(clientPort);

			ClientUser* fromClient = nullptr;
			if (it != myClients.end())
			{
				Message msg;
				strcpy_s(msg.message, mySocketBuffer);

				if (strstr(ExitMessage, mySocketBuffer))
				{
					std::cout << "Disconnect!" << std::endl;
					myClients.at(clientPort).isConnected = false;;
				}

				std::string name = myClients.at(clientPort).name;

				std::cout << "Packet from " << name << " Port:" << clientPort << std::endl;
				std::cout << "Data: " << mySocketBuffer << std::endl;

				fromClient = &myClients.at(clientPort);

				msg.client = &myClients.at(clientPort);

				std::string tempMessage = mySocketBuffer;

				ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

				tempMessage = "From: " + fromClient->name + " Data: " + tempMessage;

				strcpy_s(mySocketBuffer, tempMessage.c_str());

				myMessageHistory.push_back(msg);
			}
			else
			{
				ClientUser clientUser;
				clientUser.name = mySocketBuffer;
				clientUser.address = myAddressClient;
				myClients.emplace(clientPort, clientUser);
				std::cout << "Welcome " << mySocketBuffer << "! ClientPort:" << clientPort << std::endl;

				fromClient = &myClients.at(clientPort);

				std::string tempMessage = mySocketBuffer;

				ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

				tempMessage = "Welcome " + tempMessage + "!";

				strcpy_s(mySocketBuffer, tempMessage.c_str());
			}

			for (const auto& [port, client] : myClients)
			{
				// Send it back
				if (client.isConnected == false)
				{
					continue;
				}

				if (sendto(myUDPSocket, mySocketBuffer, NETMESSAGE_SIZE, 0, reinterpret_cast<const sockaddr*>(&client.address), sizeof(client.address)) == SOCKET_ERROR)
				{
					std::cout << "Error: " << WSAGetLastError() << std::endl;
					myIsRunning = false;
					return false;
				}
			}
		}

		return true;
	}

	void Server::CheckInput()
	{
		while (myIsRunning)
		{
			std::cin.getline(myMessage, NETMESSAGE_SIZE);

			if (strcmp(myMessage, "quit") == 0)
			{
				myIsRunning = false;
				// This will cause the socket operations to abort and release the socket.
				// It will fire a SOCKET_ERROR result from recvfrom if it's presently waiting.
				closesocket(myUDPSocket);
			}
		}
	}
}