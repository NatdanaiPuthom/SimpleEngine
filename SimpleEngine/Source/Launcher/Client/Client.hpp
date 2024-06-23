#pragma once
#include "Networking/NetworkShared.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <array>

namespace Simple
{
	class Client final
	{
	public:
		Client();
		~Client();

		bool Init();
		bool Update(const bool aIsRunning);

	private:
		void CheckInput();
		bool CheckMessage();
		void PrintCommands();
		void CreateCommands();
	private:
		char mySocketBuffer[NETMESSAGE_SIZE]; // Data buffer that we'll receive data into.
		char myMessage[NETMESSAGE_SIZE];

		WSADATA myWinsockData; // And we'll need the Winsock data object.		
		SOCKET myUDPSocket; // We'll need a udpSocket to communicate on.
		sockaddr_in myAddressServer; // Address information where we'll send our data. i.e. the container for the server address.
		
		std::array<const char*, 5> myCommandTexts;
		std::thread myInputThread;
		std::atomic<bool> myHasMessage;
		std::atomic<bool> myIsRunning;
	};
}