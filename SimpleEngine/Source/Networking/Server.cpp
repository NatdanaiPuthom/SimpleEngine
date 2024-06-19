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
	sockaddr_in address;
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
	SOCKET udpSocket;

	// And we'll need the Winsock data object.
	WSADATA winsockData;

	// Address information we'll use to bind.
	sockaddr_in addrServer{};

	// Address information of connecting client.
	sockaddr_in addrClient{};

	// We also need the size when receiving.
	int addrCLientSize = sizeof(addrClient); addrCLientSize;

	// Data buffer that we'll receive data into.
	char socketBuffer[NETMESSAGE_SIZE]; socketBuffer;

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
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			localIsRunning = false;
			break;
		}
	}

	if (inputThread.joinable())
	{
		inputThread.join();
	}

	WSACleanup();

	return 0;
}