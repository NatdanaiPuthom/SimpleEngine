#include "fmod.hpp"
#include "fmod_studio.hpp"
#include <unordered_map>
#include <array>
#include <cassert>
#include <iostream>
#include <utility>

#include "SoundEngineStructs.h"

struct SoundEngineImpl
{
private:

	struct Event
	{
		std::string Path;
		bool IsLoaded = false;
		FMOD::Studio::EventDescription* FmodEventDesc = nullptr;
		int NextInstanceId = 0;
		int NextCallbackId = 0;
		std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> FmodEventInstances;
		std::unordered_map<unsigned int, std::shared_ptr<EventCallbackBase>> Callbacks;
	};

	typedef std::unordered_map<size_t, FMOD::Channel*> ChannelMap;
	typedef std::unordered_map<std::string, FMOD::ChannelGroup*> ChannelGroupMap;
	typedef std::unordered_map<std::string, FMOD::Sound*> SoundMap;
	typedef std::unordered_map<std::string, Event> EventMap;
	typedef std::unordered_map<std::string, FMOD::Studio::Bank*> BankMap;
	typedef std::unordered_map<std::string, FMOD::Studio::Bus*> BusMap;

	FMOD::ChannelGroup* myMasterChGrp;

	ChannelGroupMap myChannelGroups;
	ChannelMap myChannels;
	SoundMap mySounds;
	EventMap myEvents;
	BankMap myBanks;
	BusMap myBusses;

	std::string myRootDirectory;

	FMOD::Studio::System* myFModStudioPtr = nullptr;
	FMOD::System* myFModPtr = nullptr;

	size_t myNextChId = 1;

	bool isInitialized = false;

	FMOD_RESULT myLastError = FMOD_OK;

	inline static std::mutex myEventMutex{};

	static FMOD_RESULT F_CALLBACK ImplCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE aType, FMOD_STUDIO_EVENTINSTANCE* anEvent, void* /*someParams*/)
	{
		//myEventMutex.lock();

		// Convert to our own type.
		CALLBACKTYPE callbackType = (CALLBACKTYPE)aType;

		// Extract our event Id.
		void* valuePtr = nullptr;
		FMOD_Studio_EventInstance_GetUserData(anEvent, &valuePtr);
		// Some casting shenanigans.
		intptr_t ptr = reinterpret_cast<intptr_t>(valuePtr);
		const int eventId = static_cast<int>(ptr);

		FMOD::Studio::EventInstance* eventInstance = reinterpret_cast<FMOD::Studio::EventInstance*>(anEvent);
		FMOD::Studio::EventDescription* eventDesc;
		eventInstance->getDescription(&eventDesc);

		SoundEngineImpl* Impl = SoundEngine::GetImpl();

		char name[1024]{};
		int nameLength = 0;
		eventDesc->getPath(name, 1024, &nameLength);
		const std::string eventName(name);

		if (const auto event = Impl->myEvents.find(eventName); event != Impl->myEvents.end())
		{
			if (const auto callback = event->second.Callbacks.find(eventId); callback != event->second.Callbacks.end())
			{
				if (callback->second && callback->second->CallbackTypes & callbackType)
				{
					const SoundEventInstanceHandle handle(eventName, eventId);
					callback->second->Broadcast(callbackType, eventName, handle, callbackType);
				}
			}
		}

		//myEventMutex.unlock();

		return FMOD_OK;
	}

	bool GetAndEnsureLoaded(const std::string& anEventName, Event* outEvent)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		auto foundEvent = myEvents.find(anEventName);
		if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
		{
			LoadEvent(anEventName);
			foundEvent = myEvents.find(anEventName);
			if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
				return false;

			outEvent = &foundEvent->second;
			return true;
		}

		return false;
	}

public:

	bool Init(const std::string& aRootPath, unsigned int maxChannels = 64)
	{
		myRootDirectory = aRootPath;
		if (!myRootDirectory.empty() && myRootDirectory.back() != '\\')
		{
			myRootDirectory.append("\\");
		}

		FMOD::Studio::System::create(&myFModStudioPtr);
		myLastError = myFModStudioPtr->initialize(static_cast<int>(maxChannels), FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr);

		if (myLastError != FMOD_OK)
			return false;

		myLastError = myFModStudioPtr->getCoreSystem(&myFModPtr);
		if (myLastError != FMOD_OK)
			return false;

		myChannels.reserve(maxChannels);

		myFModPtr->getMasterChannelGroup(&myMasterChGrp);

		isInitialized = true;
		return true;
	}

	FMOD_RESULT GetLastError()
	{
		return myLastError;
	}

	unsigned GetEvents(std::vector<std::string>& outEventList)
	{
		unsigned int count = 0;
		for (auto It = myEvents.begin(); It != myEvents.end(); ++It)
		{
			outEventList.push_back(It->second.Path);
			count++;
		}

		return count;
	}

	SoundEngineImpl() = default;
	~SoundEngineImpl()
	{
		myFModStudioPtr->unloadAll();
		myFModStudioPtr->release();
	}

	bool IsPlaying(const size_t aChannelIndex)
	{
		bool isActive = false;
		myChannels.at(aChannelIndex)->isPlaying(&isActive);
		return isActive;
	}

	void Update()
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		std::vector<ChannelMap::iterator> stopped;
		for (ChannelMap::iterator It = myChannels.begin(); It != myChannels.end(); ++It)
		{
			bool isActive = false;
			It->second->isPlaying(&isActive);
			if (!isActive)
			{
				stopped.push_back(It);
			}
		}

		for (const auto& ch : stopped)
		{
			myChannels.erase(ch);
		}

		myFModStudioPtr->update();
	}

	bool LoadSoundFile(const std::string& aFileName, bool enable3d = false, bool shouldStream = false, bool shouldLoop = false)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (mySounds.find(aFileName) != mySounds.end())
		{
			// Already loaded
			return true;
		}

		// We always load looping to avoid click at the end of the file
		// if it's toggled later.
		FMOD_MODE loadMode = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
		loadMode |= enable3d ? FMOD_3D : FMOD_2D;
		loadMode |= shouldStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		FMOD::Sound* ptr = nullptr;
		myLastError = myFModPtr->createSound((myRootDirectory + aFileName).c_str(), loadMode, nullptr, &ptr);
		if (ptr)
		{
			// Reset looping
			if (!shouldLoop)
			{
				myLastError = ptr->setMode(FMOD_LOOP_OFF);
			}

			mySounds[aFileName] = ptr;
			return true;
		}

		return false;
	}

	size_t PlaySound(const std::string& aFileName, const float aVolume = 1.0f)
	{
		return PlaySoundAtLocation(aFileName, aVolume);
	}

	bool RestartSound(size_t aChannelId)
	{
		if (aChannelId == 0)
			return false;

		if (const auto It = myChannels.find(aChannelId); It != myChannels.end())
		{
			It->second->setPosition(0, FMOD_TIMEUNIT_MS);
			myLastError = It->second->setPaused(false);
			return myLastError == FMOD_OK;
		}

		return false;
	}

	bool StopSound(size_t aChannelId)
	{
		if (aChannelId == 0)
			return false;

		if (const auto It = myChannels.find(aChannelId); It != myChannels.end())
		{
			myLastError = It->second->setPaused(true);
			if (myLastError == FMOD_OK)
			{
				myChannels.erase(It);
				return true;
			}
		}

		return false;
	}

	size_t PlaySoundAtLocation(const std::string& aFileName, const float aVolume = 1.0f, std::array<float, 3> aPosition = { 0, 0, 0 })
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		auto existingSound = mySounds.find(aFileName);
		if (existingSound == mySounds.end())
		{
			if (LoadSoundFile(aFileName, false, false))
			{
				existingSound = mySounds.find(aFileName);
			}
			else
			{
				return 0;
			}
		}

		FMOD::Channel* soundChannel = nullptr;
		myLastError = myFModPtr->playSound(existingSound->second, nullptr, true, &soundChannel);
		if (soundChannel)
		{
			FMOD_MODE currentMode;
			existingSound->second->getMode(&currentMode);
			if (currentMode & FMOD_3D)
			{
				FMOD_VECTOR relativePos;
				relativePos.x = aPosition[0];
				relativePos.y = aPosition[1];
				relativePos.z = aPosition[2];

				soundChannel->set3DAttributes(&relativePos, nullptr);
			}

			soundChannel->setVolume(aVolume);
			soundChannel->setPaused(false);

			const size_t chId = myNextChId++;
			myChannels[chId] = soundChannel;
			return chId;
		}

		return 0;
	}

	bool LoadBank(const std::string& aFileName, FMOD_STUDIO_LOAD_BANK_FLAGS someFlags)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (myBanks.find(aFileName) != myBanks.end())
			return true;  // Already loaded.

		FMOD::Studio::Bank* soundBank = nullptr;
		myLastError = myFModStudioPtr->loadBankFile((myRootDirectory + aFileName).c_str(), someFlags, &soundBank);
		if (soundBank)
		{
			myBanks[aFileName] = soundBank;
			FMOD::Studio::EventDescription* eventList[512]{};

			int eventCount = 0;
			soundBank->getEventCount(&eventCount);
			if (eventCount > 0)
			{
				FMOD_RESULT result = soundBank->getEventList(&*eventList, 512, &eventCount);
				for (auto ptr : eventList)
				{
					char path[512];
					int size = 0;
					result = ptr->getPath(path, 512, &size);
					std::string p(path);
					myEvents[path] = { path, false, nullptr };
				}
			}

			return true;
		}

		return false;
	}

	bool LoadBus(const std::string& aFileName, const std::string& anID)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (myBusses.find(aFileName) != myBusses.end())
			return true;  // Already loaded.
		FMOD::Studio::Bus* soundBus = nullptr;
		myLastError = myFModStudioPtr->getBus((myRootDirectory + aFileName).c_str(), &soundBus);
		if (soundBus)
		{
			myBusses[anID] = soundBus;
			/*FMOD::Studio::EventDescription* eventList[512]{};

			int eventCount = 0;
			soundBank->getEventCount(&eventCount);
			if (eventCount > 0)
			{
				FMOD_RESULT result = soundBank->getEventList(&*eventList, 512, &eventCount);
				for (auto ptr : eventList)
				{
					char path[512];
					int size = 0;
					result = ptr->getPath(path, 512, &size);
					std::string p(path);
					myEvents[path] = { path, false, nullptr };
				}
			}*/
			return true;
		}
		return false;
	}

	bool LoadEvent(const std::string& anEventName)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		//myEventMutex.lock();
		if (auto It = myEvents.find(anEventName); It != myEvents.end())
		{
			// We're aware of this event already.
			// Check if it's loaded.
			if (!It->second.IsLoaded)
			{
				FMOD::Studio::EventDescription* eventDesc = nullptr;
				myLastError = myFModStudioPtr->getEvent(anEventName.c_str(), &eventDesc);
				if (eventDesc)
				{
					It->second.FmodEventDesc = eventDesc;
					It->second.IsLoaded = true;
					return true;
				}
			}
		}
		//myEventMutex.unlock();
		return false;
	}

	bool LoadEvent(const FMOD_GUID& anEventGUID)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		FMOD::Studio::EventDescription* eventDesc = nullptr;
		myFModStudioPtr->getEventByID(&anEventGUID, &eventDesc);
		return false;
	}

	SoundEventInstanceHandle CreateEventInstance(const FMOD_GUID& anEventGUID)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		FMOD::Studio::EventDescription* eventDesc = nullptr;
		myLastError = myFModStudioPtr->getEventByID(&anEventGUID, &eventDesc);

		if (myLastError == FMOD_OK)
		{
			char name[1024]{};
			int nameLength = 0;
			eventDesc->getPath(name, 1024, &nameLength);
			const std::string eventName(name);
			return CreateEventInstance(eventName);
		}

		return SoundEventInstanceHandle::InvalidHandle;
	}

	SoundEventInstanceHandle CreateEventInstance(const std::string& anEventName)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		auto foundEvent = myEvents.find(anEventName);
		if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
		{
			LoadEvent(anEventName);
			foundEvent = myEvents.find(anEventName);
			if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
				return SoundEventInstanceHandle::InvalidHandle;
		}

		Event& theEvent = foundEvent->second;

		FMOD::Studio::EventInstance* eventInstance;
		myLastError = theEvent.FmodEventDesc->createInstance(&eventInstance);
		if (eventInstance)
		{
			SoundEventInstanceHandle result(anEventName, theEvent.NextInstanceId++);
			theEvent.FmodEventInstances.insert({ result.GetId(),eventInstance });
			myLastError = FMOD_OK;
			return result;
		}

		return SoundEventInstanceHandle::InvalidHandle;
	}

	ChannelGroupHandle GetEventInstanceChannelGroup(const SoundEventInstanceHandle& anEventInstanceHandle)
	{
		if (anEventInstanceHandle.IsValid())
		{
			assert(isInitialized && "SoundEngine::Init has not been called!");
			const auto foundEvent = myEvents.find(anEventInstanceHandle.GetEvent());
			if (foundEvent != myEvents.end())
			{
				if (const auto foundInstance = foundEvent->second.FmodEventInstances.find(anEventInstanceHandle.GetId()); foundInstance != foundEvent->second.FmodEventInstances.end())
				{
					FMOD::ChannelGroup* temp;
					myLastError = foundInstance->second->getChannelGroup(&temp);
					std::string name;
					name.reserve(512);
					temp->getName(name.data(), static_cast<int>(name.capacity()));

					if (myLastError == FMOD_OK)
					{
						ChannelGroupHandle result(name);
						if (myChannelGroups.find(name) == myChannelGroups.end())
						{
							myChannelGroups.insert({ name, temp });
						}
						return result;
					}

					return ChannelGroupHandle::InvalidHandle;
				}
			}
		}

		myLastError = FMOD_ERR_EVENT_NOTFOUND;
		return ChannelGroupHandle::InvalidHandle;
	}

	bool PlayEvent(const SoundEventInstanceHandle& anEventInstanceHandle)
	{
		if (anEventInstanceHandle.IsValid())
		{
			assert(isInitialized && "SoundEngine::Init has not been called!");
			const auto foundEvent = myEvents.find(anEventInstanceHandle.GetEvent());
			if (foundEvent != myEvents.end())
			{
				if (const auto foundInstance = foundEvent->second.FmodEventInstances.find(anEventInstanceHandle.GetId()); foundInstance != foundEvent->second.FmodEventInstances.end())
				{
					myLastError = foundInstance->second->start();
					return myLastError == FMOD_OK;
				}
			}
		}

		myLastError = FMOD_ERR_EVENT_NOTFOUND;
		return false;
	}

	bool PlayEventAtLocation(const SoundEventInstanceHandle& anEventInstanceHandle, std::array<float, 3> aPosition = { 0, 0, 0 })
	{
		if (anEventInstanceHandle.IsValid())
		{
			assert(isInitialized && "SoundEngine::Init has not been called!");
			const auto foundEvent = myEvents.find(anEventInstanceHandle.GetEvent());
			if (foundEvent != myEvents.end())
			{
				if (const auto foundInstance = foundEvent->second.FmodEventInstances.find(anEventInstanceHandle.GetId()); foundInstance != foundEvent->second.FmodEventInstances.end())
				{
					/*FMOD_3D_ATTRIBUTES attributes;

					attributes.position = { aPosition[0], aPosition[1], aPosition[2] };
					attributes.velocity = { 0,0,0 };
					attributes.forward = { 0,0,0 };
					attributes.up = { 0,0,0 };

					myLastError = foundInstance->second->set3DAttributes(&attributes);*/
					if (SetEvent3DParameters(anEventInstanceHandle, aPosition))
					{
						std::cout << "Updated event with new 3D parameters" << std::endl;
					}
					myLastError = foundInstance->second->start();
					return myLastError == FMOD_OK;
				}
			}
		}

		myLastError = FMOD_ERR_EVENT_NOTFOUND;
		return false;
	}

	bool IsEventPlaying(const SoundEventInstanceHandle& anEventInstanceHandle)
	{
		FMOD_STUDIO_PLAYBACK_STATE state;
		if (anEventInstanceHandle.IsValid())
		{
			assert(isInitialized && "SoundEngine::Init has not been called!");
			const auto foundEvent = myEvents.find(anEventInstanceHandle.GetEvent());
			if (foundEvent != myEvents.end())
			{
				if (const auto foundInstance = foundEvent->second.FmodEventInstances.find(anEventInstanceHandle.GetId()); foundInstance != foundEvent->second.FmodEventInstances.end())
				{
					myLastError = foundInstance->second->getPlaybackState(&state);

					if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
					{
						return false;
					}
					else
					{
						return true;
					}
				}
			}
		}

		myLastError = FMOD_ERR_EVENT_NOTFOUND;
		return false;
	}

	bool PlayEventOneShot(const std::string& anEventName)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		auto foundEvent = myEvents.find(anEventName);
		if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
		{
			LoadEvent(anEventName);
			foundEvent = myEvents.find(anEventName);
			if (foundEvent == myEvents.end() || !foundEvent->second.IsLoaded)
				return false;
		}

		FMOD::Studio::EventInstance* eventInstance;
		myLastError = foundEvent->second.FmodEventDesc->createInstance(&eventInstance);
		if (eventInstance)
		{
			myLastError = eventInstance->start();
			myLastError = eventInstance->release();
		}

		return myLastError == FMOD_OK;
	}

	bool PlayEventOneShot(const FMOD_GUID& anEventGUID)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		FMOD::Studio::EventDescription* eventDesc = nullptr;
		myLastError = myFModStudioPtr->getEventByID(&anEventGUID, &eventDesc);

		if (myLastError == FMOD_OK)
		{
			char name[1024]{};
			int nameLength = 0;
			eventDesc->getPath(name, 1024, &nameLength);
			const std::string eventName(name);
			return PlayEventOneShot(eventName);
		}

		return myLastError == FMOD_OK;
	}

	bool RegisterCallback(const SoundEventInstanceHandle& aHandle, std::shared_ptr<EventCallbackBase> aPtr)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		Event& theEvent = foundEvent->second;

		if (const auto eventInstance = theEvent.FmodEventInstances.find(aHandle.GetId()); eventInstance != theEvent.FmodEventInstances.end())
		{
			if (const auto existingCallback = theEvent.Callbacks.find(aHandle.GetId()); existingCallback != theEvent.Callbacks.end())
			{
				theEvent.Callbacks.erase(existingCallback);
			}
			else
			{
				intptr_t val = (intptr_t)aHandle.GetId();
				eventInstance->second->setUserData(reinterpret_cast<void*>(val));
				myLastError = eventInstance->second->setCallback(ImplCallback, FMOD_STUDIO_EVENT_CALLBACK_ALL);
			}

			theEvent.Callbacks.insert({ aHandle.GetId(), aPtr });
			return true;
		}

		return false;
	}

	bool RemoveCallback(const SoundEventInstanceHandle& aHandle)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		Event& theEvent = foundEvent->second;
		if (const auto eventInstance = theEvent.FmodEventInstances.find(aHandle.GetId()); eventInstance != theEvent.FmodEventInstances.end())
		{
			if (const auto existingCallback = theEvent.Callbacks.find(aHandle.GetId()); existingCallback != theEvent.Callbacks.end())
			{
				theEvent.Callbacks.erase(existingCallback);
				eventInstance->second->setCallback(nullptr, FMOD_STUDIO_EVENT_CALLBACK_ALL);
				return true;
			}
		}

		return false;
	}

	bool SetEvent3DParameters(const SoundEventInstanceHandle& aHandle, std::array<float, 3> aPosition = { 0, 0, 0 }, std::array<float, 3> aVelocity = { 0, 0, 0 }, std::array<float, 3> aForwardVector = { 0, 0, 1 }, std::array<float, 3> aUpVector = { 0, 1, 0 })
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		FMOD_VECTOR pos, vel, fwd, up;
		pos		=	{ aPosition[0], aPosition[1], aPosition[2] };
		vel		=	{ aVelocity[0], aVelocity[1], aVelocity[2] };
		fwd		=	{ aForwardVector[0], aForwardVector[1], aForwardVector[2] };
		up		=	{ aUpVector[0], aUpVector[1], aUpVector[2] };
		const FMOD_3D_ATTRIBUTES attributes = { pos, vel, fwd, up };

		myLastError = foundEvent->second.FmodEventInstances[aHandle.GetId()]->set3DAttributes(&attributes);
		return myLastError == FMOD_OK;
	}

	bool SetEventListenerMask(const SoundEventInstanceHandle& aHandle, unsigned int aMask)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->setListenerMask(aMask);
		}

		return myLastError == FMOD_OK;
	}

	bool SetEventPitch(const SoundEventInstanceHandle& aHandle, float aPitch)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->setPitch(aPitch);
		}

		return myLastError == FMOD_OK;
	}

	bool SetEventReverbLevel(const SoundEventInstanceHandle& aHandle, int anIndex, float aLevel)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->setReverbLevel(anIndex, aLevel);
		}

		return myLastError == FMOD_OK;
	}

	bool StopEvent(const SoundEventInstanceHandle& aHandle, bool immediately)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			const FMOD_STUDIO_STOP_MODE stopMode = immediately ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
			myLastError = instance->second->stop(stopMode);
		}

		return myLastError == FMOD_OK;
	}

	bool GetEventParameter(const SoundEventInstanceHandle& aHandle, const std::string& aParameterName, float* outValue)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->getParameterByName(aParameterName.c_str(), outValue);
		}

		return myLastError == FMOD_OK;
	}

	bool SetEventParameter(const SoundEventInstanceHandle& aHandle, const std::string& aParameterName, float value)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->setParameterByName(aParameterName.c_str(), value);
		}

		return myLastError == FMOD_OK;
	}

	bool SetGlobalParameter(const std::string& aParameterName, float aValue, bool ignoreSeekSpeed = false)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		myLastError = myFModStudioPtr->setParameterByName(aParameterName.c_str(), aValue, ignoreSeekSpeed);
		return myLastError == FMOD_OK;
	}

	bool GetGlobalParameter(const std::string& aParameterName, float* outValue)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		myLastError = myFModStudioPtr->getParameterByName(aParameterName.c_str(), outValue);
		return myLastError == FMOD_OK;
	}

	bool SetVolume(unsigned int aChannel, float aVolumePct)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (auto ch = myChannels.find(aChannel); ch == myChannels.end())
		{
			myLastError = FMOD_ERR_INVALID_HANDLE;
		}
		else
		{
			myLastError = ch->second->setVolume(aVolumePct);
		}

		return myLastError == FMOD_OK;
	}

	bool SetVolume(const SoundEventInstanceHandle& aHandle, float aVolumePct)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!aHandle.IsValid())
			return false;

		const auto foundEvent = myEvents.find(aHandle.myEventName);
		if (foundEvent == myEvents.end())
		{
			return false;
		}

		myLastError = FMOD_ERR_INVALID_HANDLE;

		if (const auto instance = foundEvent->second.FmodEventInstances.find(aHandle.GetId());
			instance != foundEvent->second.FmodEventInstances.end())
		{
			myLastError = instance->second->setVolume(aVolumePct);
		}

		return myLastError == FMOD_OK;
	}

	bool SetVolume(const ChannelGroupHandle& aGroupHandle, float aVolumePct)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (const auto ch = myChannelGroups.find(aGroupHandle.GetName()); ch == myChannelGroups.end())
		{
			myLastError = FMOD_ERR_INVALID_HANDLE;
		}
		else
		{
			if (ch->second)
			{
				myLastError = ch->second->setVolume(aVolumePct);
			}
			else
			{
				myChannelGroups.erase(ch);
				myLastError = FMOD_ERR_INVALID_HANDLE;
			}
		}

		return myLastError == FMOD_OK;
	}

	bool SetBusVolume(const std::string& anID, float aVolumePct)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (const auto bus = myBusses.find(anID); bus == myBusses.end())
		{
			return myLastError = FMOD_ERR_INVALID_STRING;
		}
		else
		{
			myLastError = bus->second->setVolume(aVolumePct);
		}
		return myLastError == FMOD_OK;
	}

	bool SetMasterVolume(float aVolumePct)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (!myMasterChGrp)
		{
			myLastError = FMOD_ERR_BADCOMMAND;
			return false;
		}

		myLastError = myMasterChGrp->setVolume(aVolumePct);

		return myLastError == FMOD_OK;
	}

	int GetNumListeners() const
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		int numListeners;
		myFModPtr->get3DNumListeners(&numListeners);
		myFModStudioPtr->getNumListeners(&numListeners);
		return numListeners;
	}

	bool AddListener(const std::array<float, 3> aPosition = { 0, 0, 0 }, const std::array<float, 3> aVelocity = { 0, 0, 0 }, const std::array<float, 3> aForwardVector = { 0, 0, 1 }, const std::array<float, 3> aUpVector = { 0, 1, 0 })
	{
		std::cout << "Entered the addlistener" << std::endl;
		assert(isInitialized && "SoundEngine::Init has not been called!");
		std::cout << "Past assert" << std::endl;
		const int numListeners = GetNumListeners();
		if (numListeners < 8)
		{
			int num; 
			myFModStudioPtr->getNumListeners(&num);
				std::cout << "number of listeners before add: " << num << std::endl;
			
			myFModStudioPtr->setNumListeners(numListeners + 1);
			myFModStudioPtr->getNumListeners(&num);
			std::cout << "number of listeners after add: " << num << std::endl;
			//FMOD_VECTOR pos, vel, fwd, up;
			FMOD_3D_ATTRIBUTES attributes;

			attributes.position		=	{ aPosition[0], aPosition[1], aPosition[2] };
			attributes.velocity		=	{ aVelocity[0], aVelocity[1], aVelocity[2] };
			attributes.forward		=	{ aForwardVector[0], aForwardVector[1], aForwardVector[2] };
			attributes.up			=	{ aUpVector[0], aUpVector[1], aUpVector[2] };

			myLastError = myFModStudioPtr->setListenerAttributes(numListeners, &attributes);
		}
		else
		{
			return myLastError = FMOD_ERR_TOOMANYCHANNELS;
		}
		EqualizeListenerWeights();
		return myLastError == FMOD_OK;
	}

	bool EqualizeListenerWeights()
	{
		const int numListeners = GetNumListeners();
		float quotientWeight = 1.0f / numListeners;
		for (size_t i = 0; i < numListeners; i++)
		{
			myLastError = myFModStudioPtr->setListenerWeight(static_cast<int>(i), quotientWeight);
		}
		return myLastError;
	}

	bool OverrideOtherListeners(int anIndex)
	{
		const int numListeners = GetNumListeners();
		if (numListeners > 0)
		{
			for (size_t i = 0; i < numListeners; i++)
			{
				if (i == anIndex)
				{
					myLastError = myFModStudioPtr->setListenerWeight(static_cast<int>(i), 1.0f);
				}
				else
				{
					myLastError = myFModStudioPtr->setListenerWeight(static_cast<int>(i), 0.0f);
				}
			}
			return myLastError;
		}
		else
		{
			myLastError = FMOD_ERR_BADCOMMAND;
		}

		return myLastError;
	}

	bool RemoveListener(int anIndex)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		const int numListeners = GetNumListeners();
		if (anIndex >= numListeners || numListeners == 1)
			return false;

		if (anIndex != numListeners)
		{
			// And here we go, move everything after this index forward.
			for (int i = anIndex; i < numListeners - 1; i++)
			{
				FMOD_3D_ATTRIBUTES attributes;
				/*FMOD_VECTOR pos, vel, fwd, up;
				myFModPtr->get3DListenerAttributes(i + 1, &pos, &vel, &fwd, &up);
				myFModPtr->set3DListenerAttributes(i, &pos, &vel, &fwd, &up);*/
				myFModStudioPtr->getListenerAttributes(i + 1, &attributes);
				myFModStudioPtr->setListenerAttributes(i, &attributes);
			}
		}
		myLastError = myFModStudioPtr->setNumListeners(numListeners - 1);
		//myLastError = myFModPtr->set3DNumListeners(numListeners - 1);

		return myLastError == FMOD_OK;
	}

	bool SetListenerTransform(int aListenerIndex, const std::array<float, 3> aPosition = { 0, 0, 0 }, const std::array<float, 3> aVelocity = { 0, 0, 0 }, const std::array<float, 3> aForwardVector = { 0, 0, 0 }, const std::array<float, 3> aUpVector = { 0, 0, 0 })
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		if (aListenerIndex >= GetNumListeners())
		{
			return false;
		}

		FMOD_3D_ATTRIBUTES attributes;

		attributes.position		=	{ aPosition[0], aPosition[1], aPosition[2] };
		attributes.velocity		=	{ aVelocity[0], aVelocity[1], aVelocity[2] };
		attributes.forward		=	{ aForwardVector[0], aForwardVector[1], aForwardVector[2] };
		attributes.up			=	{ aUpVector[0], aUpVector[1], aUpVector[2] };
		std::cout << "Attributes pos " << attributes.position.x << std::endl;
		myLastError = myFModStudioPtr->setListenerAttributes(aListenerIndex, &attributes);

		return myLastError == FMOD_OK;
	}


	bool GetListenerTransform(int aListenerIndex, std::array<float, 3>& aPosition, std::array<float, 3>& aVelocity, std::array<float, 3>& aForwardVector, std::array<float, 3>& aUpVector)
	{
		assert(isInitialized && "SoundEngine::Init has not been called!");
		FMOD_3D_ATTRIBUTES attributes;

		myLastError = myFModStudioPtr->getListenerAttributes(aListenerIndex, &attributes);
		std::cout << "Get Listener Transform returns : " << myLastError << std::endl;
		if (myLastError == FMOD_OK)
		{
			aPosition		=	{ attributes.position.x, attributes.position.y, attributes.position.z };
			aVelocity		=	{ attributes.velocity.x, attributes.velocity.y, attributes.velocity.z };
			aForwardVector	=	{ attributes.forward.x, attributes.forward.y, attributes.forward.z };
			aUpVector		=	{ attributes.up.x, attributes.up.y, attributes.up.z };
			return true;
		}

		return false;
	}
};
