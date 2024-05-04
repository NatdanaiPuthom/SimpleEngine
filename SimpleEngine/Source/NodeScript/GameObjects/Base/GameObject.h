#pragma once
#include "NodeScript/Game/GameUpdateContext.h"
#include <vector>
#include <string>

class GameObject final
{
public:
	std::string myName;
	const unsigned int myID;
	bool myCanPush;
	bool myCanStandOn;
	Math::Vector2i myPosition;
	bool myIsActive ;
	int myKeyCount;

public:
	GameObject(unsigned int aID)
		: myCanPush(false)
		, myCanStandOn(false)
		, myID(aID)
		, myName(std::string("GameObject") + std::to_string(aID))
		, myIsActive(true)
		, myKeyCount(0)
	{
	};

	~GameObject()
	{
	};

	void Update(const GameUpdateContext& /*aContext*/)
	{
		if (!myIsActive)
		{
			return;
		}
	}
};