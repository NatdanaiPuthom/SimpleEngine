#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Managers/DataManager.hpp"

namespace Graphics
{
	GenericDataManager::GenericDataManager()
		: myClearColor{ 0.0f, 0.0f, 0.0f, 1.0f }
		, myVSync(false)
		, myFPSLevelCap(0)
	{
	}

	GenericDataManager::~GenericDataManager()
	{
	}

	void GenericDataManager::Init()
	{
	}

	bool GenericDataManager::IsVSyncOn() const
	{
		return myVSync;
	}

	void GenericDataManager::SetVSync(const bool aShouldTurnOn)
	{
		myVSync = aShouldTurnOn;

		if (myVSync)
		{
			myFPSLevelCap = 1;
		}
		else
		{
			myFPSLevelCap = 0;
		}
	}

	void GenericDataManager::SetFPSLevelCap(const unsigned int aCapLevel)
	{
		myFPSLevelCap = aCapLevel;

		if (aCapLevel > 4)
		{
			myFPSLevelCap = 4;
		}

		if (myVSync)
		{
			myFPSLevelCap = 1;
		}
	}

	unsigned int GenericDataManager::GetFPSLevelCap() const
	{
		return myFPSLevelCap;
	}

	const std::array<float, 4>& GenericDataManager::GetClearColor() const
	{
		return myClearColor;
	}
}
