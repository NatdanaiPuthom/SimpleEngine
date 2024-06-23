#include "Launcher/Client/Client.hpp"
#include <future>
#include <iostream>
#include <Windows.h>
#include <cassert>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 42000

//static int ReceiveFrom(
//	_In_ SOCKET s,
//	_Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR* buf,
//	_In_ int len,
//	_In_ int flags,
//	_Out_writes_bytes_to_opt_(*fromlen, *fromlen) struct sockaddr FAR* from,
//	_Inout_opt_ int FAR* fromlen
//)
//{
//	return recvfrom(s, buf, len, flags, from, fromlen);
//}

namespace Simple
{
	Client::Client()
		: myUDPSocket{}
		, myWinsockData{}
		, myAddressServer{}
		, myCommandTexts{}
	{
		myHasMessage = false;
		myIsRunning = true;
		myHasLoggedIn = false;

		std::memset(mySocketBuffer, '\0', sizeof(mySocketBuffer));
		std::memset(myMessage, '\0', sizeof(myMessage));
	}

	Client::~Client()
	{
		// Clear the buffer.
		ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

		if (myInputThread.joinable())
		{
			myInputThread.join();
		}

		if (sendto(myUDPSocket, Global_ExitMessage, sizeof(Global_ExitMessage), 0, reinterpret_cast<sockaddr*>(&myAddressServer), sizeof(myAddressServer)) == SOCKET_ERROR)
		{
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			assert(false && "something went wrong in client destructor");
			return;
		}

		closesocket(myUDPSocket);
		WSACleanup();
	}

	bool Client::Init()
	{
		std::cout << "Starting Winsock...";

		if (WSAStartup(MAKEWORD(2, 2), &myWinsockData) != 0)
		{
			std::cout << " FAIL!" << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;

			assert(false && "Failed to start up WinSock");
			return false;
		}

		std::cout << "OK!" << std::endl;

		myUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (myUDPSocket == INVALID_SOCKET)
		{
			std::cout << "Failed to create udpSocket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;

			assert(false && "Invalid socket");
			return false;
		}

		// Set udpSocket to non blocking
		// This way recvfrom will not wait until it gets a message
		u_long iMode = true;
		ioctlsocket(myUDPSocket, FIONBIO, &iMode);

		// Tell the system where the server is that we want to talk with.
		myAddressServer.sin_family = AF_INET; // Internetwork. Required for TCP/UDP/etc

		// Unlike on the Server we cannot specify INADDR_ANY since we need to know
		// which destination IP address to use.
		InetPton(AF_INET, TEXT(SERVER_ADDRESS), &myAddressServer.sin_addr.s_addr);

		myAddressServer.sin_port = htons(SERVER_PORT);   // The port that the server is listening on
		// htons converts the byte order from host order to
		// network order.

		// Why no bind on client? We leave that to the operating system!
		// By calling sendto we cause Windows to implicitly bind the udpSocket
		// by filling in all the information and giving us a random local port.
		// This then allows us to use recvfrom as we please because the udpSocket
		// is already bound for us in an appropriate way.

		// send a first message to bind the udpSocket
		// after this we will be able to receive messages
		std::cout << "Enter username: ";
		myInputThread = std::thread(&Client::CheckInput, this);

		return true;
	}

	bool Client::Update(const bool aIsRunning)
	{
		if (myHasLoggedIn == false)
		{
			return true;
		}

		myIsRunning = aIsRunning;

		if (CheckMessage() == false)
		{
			return false;
		}

		ZeroMemory(mySocketBuffer, NETMESSAGE_SIZE);

		int addrServerSize = sizeof(myAddressServer);
		const int recv_len = recvfrom(myUDPSocket, mySocketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&myAddressServer, &addrServerSize);

		if (recv_len == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			std::cout << "\nFailed receiving data from udpSocket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
		}

		if (recv_len > 0)
		{
			std::cout << "\nPacket from server!: " << mySocketBuffer << std::endl;
		}

		return true;
	}

	void Client::CheckInput()
	{
		while (myHasLoggedIn == false)
		{
			std::cin.getline(myMessage, NETMESSAGE_SIZE);

			if (sendto(myUDPSocket, myMessage, NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myAddressServer), sizeof(myAddressServer)) == SOCKET_ERROR)
			{
				std::cout << "\nError: " << WSAGetLastError() << std::endl;
				assert(false && "Something went wrong");
				myIsRunning = false;
				return;
			}

			CreateCommands();
			PrintCommands();

			myHasLoggedIn = true;
		}

		// very basic async input setup... we read input on a different thread
		while (myIsRunning)
		{
			while (myHasMessage) // wait until message is processed before reading the next one;
			{
				std::this_thread::yield();
			}

			if (myIsRunning == false)
			{
				break;
			}

			std::cin.getline(myMessage, NETMESSAGE_SIZE);
			myHasMessage = true;
		};
	}

	bool Client::CheckMessage()
	{
		if (myIsRunning == false)
		{
			myHasMessage = false;
			return false;
		}

		if (myHasMessage)
		{
			if (strcmp(myMessage, "quit") == 0)
			{
				myIsRunning = false;
				myHasMessage = false;
				return false;
			}
			else if (strcmp(myMessage, "clear") == 0)
			{
				system("cls");
				std::cout << "----------------------------------------" << std::endl;
				std::cout << "help - show commands" << std::endl;
				std::cout << "----------------------------------------" << std::endl;
			}
			else if (strcmp(myMessage, "help") == 0)
			{
				PrintCommands();
			}

			if (sendto(myUDPSocket, myMessage, NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myAddressServer), sizeof(myAddressServer)) == SOCKET_ERROR)
			{
				std::cout << "\nError: " << WSAGetLastError() << std::endl;
				myIsRunning = false;
				return false;
			}

			myHasMessage = false;
		}

		return true;
	}

	void Client::PrintCommands()
	{
		for (size_t i = 0; i < myCommandTexts.size(); ++i)
		{
			std::cout << myCommandTexts[i] << std::endl;
		}
	}

	void Client::CreateCommands()
	{
		myCommandTexts[0] = "\n----------------------------------------";
		myCommandTexts[1] = "\nCommands";
		myCommandTexts[2] = "\n quit -  exit the application";
		myCommandTexts[3] = "\n clear - clear console";
		myCommandTexts[4] = "\n----------------------------------------";
	}
}