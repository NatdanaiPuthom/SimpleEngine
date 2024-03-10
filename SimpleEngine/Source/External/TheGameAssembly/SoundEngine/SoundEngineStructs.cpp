#include "SoundEngine.pch.h"
#include "SoundEngineStructs.h"
#include "SoundEngine.h"
#include "SoundEngineImpl.hpp"

SoundEventInstanceHandle SoundEventInstanceHandle::InvalidHandle = {"", -1};

SoundEventInstanceHandle::SoundEventInstanceHandle(const std::string& anEvent, int aHandleId)
{
	myInstance = aHandleId;
	myEventName = anEvent;
}

SoundEventInstanceHandle::SoundEventInstanceHandle()
	: myEventName(), myInstance(-1)
{

}

int SoundEventInstanceHandle::GetId() const
{
	return myInstance;
}

const std::string& SoundEventInstanceHandle::GetEvent() const
{
	return myEventName;
}

bool SoundEventInstanceHandle::IsValid() const
{
	return !myEventName.empty() && myInstance >= 0;
}

ChannelGroupHandle ChannelGroupHandle::InvalidHandle = {""};

ChannelGroupHandle::ChannelGroupHandle(const std::string& aChannelName)
	: myChannelName(aChannelName)
{
	
}

ChannelGroupHandle::ChannelGroupHandle()
	: myChannelName()
{

}

const std::string& ChannelGroupHandle::GetName() const
{
	return myChannelName;
}

bool ChannelGroupHandle::IsValid() const
{
	return !myChannelName.empty();
}
