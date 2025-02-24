#pragma once
#include <climits>

constexpr unsigned int NETMESSAGE_SIZE = 512;
constexpr char Global_ExitMessage[11] = "Disconnect";

int MessageID = 0;

int GiveIDMessage()
{
	int id = MessageID;
	MessageID++;
	if (MessageID == INT_MAX)
	{
		MessageID = 0;
	}
	return id;
}

enum class eTypeOfMessage : unsigned int
{
	MessageEnd,
	MessageSucced,
	ConnectionMessage,
	DisconnectMessage,
	AcceptedNewPlayer,
	ChatMessage,
	InformationMessage,
};

template<typename T>
struct MessageBlock
{
	int messageID;
	eTypeOfMessage messageType;
	T data;
};

//struct Message
//{
//	int clientID;
//	std::vector<MessageBlock> Message;
//};
