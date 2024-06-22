#include "Networking/NetworkShared.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>

#include <future>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>

#include "Networking/RemoveMee/Console.hpp" //NOTE(v11.2.0): remove this later

#define LISTEN_PORT 42000

struct ClientUser
{
	std::string name = "Client";
	sockaddr_in address{};
	bool isConnected = true;
};

struct Message
{
	ClientUser* client;
	char message[NETMESSAGE_SIZE];
};

std::atomic<bool> localIsRunning = true;
std::unordered_map<int, ClientUser> localClients;
std::vector<Message> localMessageHistory;


int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int /*nCmdShow*/)
{
	Simple::Console console;

	// We'll need a socket to communicate on.
	SOCKET udpSocket{};

	// And we'll need the Winsock data object.
	WSADATA winsockData{};

	// Address information we'll use to bind.
	sockaddr_in addrServer{};

	// Address information of connecting client.
	sockaddr_in addrClient{};

	// We also need the size when receiving.
	int addrClientSize = sizeof(addrClient);

	// Data buffer that we'll receive data into.
	char socketBuffer[NETMESSAGE_SIZE];

	std::cout << "Starting Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &winsockData) != 0)
	{
		std::cout << " FAIL!" << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return -1;
	}

	std::cout << "OK!" << std::endl;

	udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udpSocket == INVALID_SOCKET)
	{
		std::cout << "Failed to create socket." << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return -1;
	}

	// Populate the server address information
	addrServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc
	addrServer.sin_addr.s_addr = INADDR_ANY;            // Don't care which IP address to use. Any available.
	// Computers usually have at least two when connected
	// to a network. One loopback and one network address.
	// 127.0.0.1 - Built-in loopback adapter.
	// e.g. 192.168.0.1 - Class-C address from a router.

	addrServer.sin_port = htons(LISTEN_PORT);   // The port we want to listen for connections on.
	// htons converts the byte order from host order to
	// network order.

	// We need to bind to a specific address and port combination. This tells the
	// operating system that we want communication on that combination to go to
	// this program.
	if (bind(udpSocket, reinterpret_cast<sockaddr*>(&addrServer), sizeof addrServer) == SOCKET_ERROR)
	{
		std::cout << "Failed to bind socket." << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return -1;
	}

	// We want to be able to shut down gracefully and right now we use blocking receive
	// so we should set up a thread to listen for keyboard input.
	std::thread inputThread([udpSocket]
		{
			while (localIsRunning)
			{
				char c;
				std::cin.get(c);
				if (c == 'q')
				{
					localIsRunning = false;
					// This will cause the socket operations to abort and release the socket.
					// It will fire a SOCKET_ERROR result from recvfrom if it's presently waiting.
					closesocket(udpSocket);
				}
			}
		});

	// If we got this far we should now have an open socket ready to receive information from the network.
	std::cout << "Press Enter to exit..." << std::endl;
	while (localIsRunning)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			localIsRunning = false;
			break;
		}

		// Clear the buffer.
		ZeroMemory(socketBuffer, NETMESSAGE_SIZE);

		// blocking receive. This function will block until a message is received.
		const int recv_len = recvfrom(udpSocket, socketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&addrClient, &addrClientSize);
		if (recv_len == SOCKET_ERROR)
		{
			std::cout << "Failed receiving data from socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			const int clientPort = ntohs(addrClient.sin_port);
			localClients.erase(clientPort);

		}

		if (recv_len > 0)
		{
			// Extract the address information from the incoming data.
			// 16 bytes is enough for an IPv4 address.
			// i.e. "xxx.xxx.xxx.xxx" + string terminator

			char clientMessage[NETMESSAGE_SIZE]{};

			strcpy_s(clientMessage, socketBuffer);

			char clientAddress[16]{ '\0'};
			inet_ntop(AF_INET, &addrClient.sin_addr, &clientAddress[0], sizeof(clientAddress));
			const int clientPort = ntohs(addrClient.sin_port);

			auto it = localClients.find(clientPort);

			ClientUser* fromClient = nullptr;
			if (it != localClients.end())
			{
				Message msg;
				strcpy_s(msg.message, socketBuffer);

				if (strstr(ExitMessage, socketBuffer))
				{
					std::cout << "Disconnect!" << std::endl;
					localClients.at(clientPort).isConnected = false;;

				}
				std::string name = localClients.at(clientPort).name;

				std::cout << "Packet from " << name << " Port:" << clientPort << std::endl;
				std::cout << "Data: " << socketBuffer << std::endl;

				fromClient = &localClients.at(clientPort);

				msg.client = &localClients.at(clientPort);

				std::string tempMessage = socketBuffer;

				ZeroMemory(socketBuffer, NETMESSAGE_SIZE);

				tempMessage = "From: " + fromClient->name + " Data: " + tempMessage;

				strcpy_s(socketBuffer, tempMessage.c_str());

				localMessageHistory.push_back(msg);

			}
			else
			{
				ClientUser clientUser;
				clientUser.name = socketBuffer;
				clientUser.address = addrClient;
				localClients.emplace(clientPort, clientUser);
				std::cout << "Welcome " << socketBuffer << "! ClientPort:" << clientPort << std::endl;

				fromClient = &localClients.at(clientPort);

				std::string tempMessage = socketBuffer;

				ZeroMemory(socketBuffer, NETMESSAGE_SIZE);

				tempMessage = "Welcome " + tempMessage + "!";

				strcpy_s(socketBuffer, tempMessage.c_str());

			}

			for (const auto& [port, client] : localClients)
			{
				// Send it back
				if (!client.isConnected)
				{
					continue;
				}
				if (sendto(udpSocket, socketBuffer, NETMESSAGE_SIZE, 0, reinterpret_cast<const sockaddr*>(&client.address), sizeof(client.address)) == SOCKET_ERROR)
				{
					std::cout << "Error: " << WSAGetLastError() << std::endl;
					localIsRunning = false;
					break;
				}
			}
		}

		if (inputThread.joinable())
		{
			inputThread.join();
		}

		WSACleanup();

	}

		return 0;
}