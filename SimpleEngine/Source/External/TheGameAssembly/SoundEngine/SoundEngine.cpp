#include "SoundEngine.pch.h"
#include "SoundEngine.h"

#include <utility>
#include "SoundEngineImpl.hpp"

int SoundEngine::ListenerHandle::GetId() const
{
	return myId;
}

bool SoundEngine::ListenerHandle::IsValid() const
{
	return myId != -1;
}

SoundEngine::ListenerHandle SoundEngine::Listener::Handle() const
{
	ListenerHandle handle;
	handle.myId = ListenerId;
	return handle;
}

bool SoundEngine::Init(const std::string& aRootPath)
{
	myImpl = new SoundEngineImpl();

	// FMOD supports 8 listeners.
	myListeners.reserve(8);

	return myImpl->Init(aRootPath);
}

void SoundEngine::Release()
{
	if (myImpl != nullptr)
	{
		delete myImpl;
		myImpl = nullptr;
	}
}

void SoundEngine::Update()
{
	myImpl->Update();
}

bool SoundEngine::LoadSoundFile(const std::string& aFileName, bool enable3d, bool shouldStream, bool shouldLoop)
{
	return myImpl->LoadSoundFile(aFileName, enable3d, shouldStream, shouldLoop);
}

size_t SoundEngine::PlaySound(const std::string& aFileName, const float aVolume)
{
	return myImpl->PlaySound(aFileName, aVolume);
}

bool SoundEngine::StopSound(size_t aChannelNumber)
{
	return myImpl->StopSound(aChannelNumber);
}

bool SoundEngine::RestartSound(size_t aChannelNumber)
{
	return myImpl->RestartSound(aChannelNumber);
}

size_t SoundEngine::PlaySoundAtLocation(const std::string& aFileName, const float aVolume,
	std::array<float, 3> aPosition)
{
	return myImpl->PlaySoundAtLocation(aFileName, aVolume, aPosition);
}

bool SoundEngine::LoadBank(const std::string& aFileName, FMOD_STUDIO_LOAD_BANK_FLAGS someFlags)
{
	return myImpl->LoadBank(aFileName, someFlags);
}

bool SoundEngine::LoadBus(const std::string& aFileName, const std::string& anID)
{
	return myImpl->LoadBus(aFileName, anID);
}

bool SoundEngine::LoadEvent(const std::string& anEventName)
{
	return myImpl->LoadEvent(anEventName);
}

unsigned int SoundEngine::GetEvents(std::vector<std::string>& outEventList)
{
	return myImpl->GetEvents(outEventList);
}

SoundEventInstanceHandle SoundEngine::CreateEventInstance(unsigned anEventId)
{
	if (myEventAccelMap.find(anEventId) != myEventAccelMap.end())
	{
		return myImpl->CreateEventInstance(myEventAccelMap[anEventId]);
	}

	return SoundEventInstanceHandle::InvalidHandle;
}

SoundEventInstanceHandle SoundEngine::CreateEventInstance(const FMOD_GUID& anEventGUID)
{
	return myImpl->CreateEventInstance(anEventGUID);
}

bool SoundEngine::PlayEvent(const SoundEventInstanceHandle& anEventInstanceHandle)
{
	return myImpl->PlayEvent(anEventInstanceHandle);
}

bool SoundEngine::PlayEventAtLocation(const SoundEventInstanceHandle& anEventInstanceHandle, std::array<float, 3> aPosition)
{
	return myImpl->PlayEventAtLocation(anEventInstanceHandle, aPosition);
}

bool SoundEngine::IsEventPlaying(const SoundEventInstanceHandle& anEventInstanceHandle)
{
	return myImpl->IsEventPlaying(anEventInstanceHandle);
}

ChannelGroupHandle SoundEngine::GetEventInstanceChannelGroup(const SoundEventInstanceHandle& anEventInstanceHandle)
{
	return myImpl->GetEventInstanceChannelGroup(anEventInstanceHandle);
}

bool SoundEngine::PlayEventOneShot(const FMOD_GUID& anEventGUID)
{
	return myImpl->PlayEventOneShot(anEventGUID);
}

bool SoundEngine::PlayEventOneShot(const std::string& anEventName)
{
	return myImpl->PlayEventOneShot(anEventName);
}

bool SoundEngine::RegisterEvent(const std::string& anEventName, unsigned anEventId)
{
	if (myEventAccelMap.find(anEventId) == myEventAccelMap.end())
	{
		myEventAccelMap[anEventId] = anEventName;
		return true;
	}

	return false;
}

SoundEventInstanceHandle SoundEngine::CreateEventInstance(const std::string& anEventName)
{
	return myImpl->CreateEventInstance(anEventName);
}

bool SoundEngine::PlayEventOneShot(unsigned anEventId)
{
	if (myEventAccelMap.find(anEventId) != myEventAccelMap.end())
	{
		return myImpl->PlayEventOneShot(myEventAccelMap[anEventId]);
	}

	return false;
}

bool SoundEngine::StopEvent(const SoundEventInstanceHandle& anEventInstanceHandle, bool immediately)
{
	return myImpl->StopEvent(anEventInstanceHandle, immediately);
}

bool SoundEngine::GetEventParameter(const SoundEventInstanceHandle& anEventInstanceHandle, const std::string& aParameterName, float* outValue)
{
	return myImpl->GetEventParameter(anEventInstanceHandle, aParameterName, outValue);
}

bool SoundEngine::SetEventParameter(const SoundEventInstanceHandle& anEventInstanceHandle, const std::string& aParameterName, float aValue)
{
	return myImpl->SetEventParameter(anEventInstanceHandle, aParameterName, aValue);
}

bool SoundEngine::SetGlobalParameter(const std::string& aParameterName, float aValue, bool ignoreSeekSpeed)
{
	return myImpl->SetGlobalParameter(aParameterName, aValue, ignoreSeekSpeed);
}

bool SoundEngine::GetGlobalParameter(const std::string& aParameterName, float* outValue)
{
	return myImpl->GetGlobalParameter(aParameterName, outValue);
}

bool SoundEngine::SetMasterVolume(float aVolumePct)
{
	return myImpl->SetMasterVolume(aVolumePct);
}

bool SoundEngine::SetMasterVolumeDB(float aVolumeDB)
{
	const float volumePct = powf(10.0f, 0.05f * aVolumeDB);
	return myImpl->SetMasterVolume(volumePct);
}

bool SoundEngine::SetBusVolume(const std::string& anID, float aVolumePct)
{
	return myImpl->SetBusVolume(anID, aVolumePct);
}

bool SoundEngine::SetBusVolumeDB(const std::string& anID, float aVolumeDB)
{
	const float volumePct = powf(10.0f, 0.05f * aVolumeDB);
	return myImpl->SetBusVolume(anID, volumePct);
}

bool SoundEngine::SetVolume(unsigned aChannel, float aVolumePct)
{
	return myImpl->SetVolume(aChannel, aVolumePct);
}

bool SoundEngine::SetVolumeDB(unsigned aChannel, float aVolumeDB)
{
	const float volumePct = powf(10.0f, 0.05f * aVolumeDB);
	return myImpl->SetVolume(aChannel, volumePct);
}

bool SoundEngine::SetEventVolume(const SoundEventInstanceHandle& anEventInstanceHandle, float aVolumePct)
{
	return myImpl->SetVolume(anEventInstanceHandle, aVolumePct);
}

bool SoundEngine::SetEventVolumeDB(const SoundEventInstanceHandle& anEventInstanceHandle, float aVolumeDB)
{
	const float volumePct = powf(10.0f, 0.05f * aVolumeDB);
	return myImpl->SetVolume(anEventInstanceHandle, volumePct);
}

bool SoundEngine::SetChannelGroupVolume(const ChannelGroupHandle& aGroupHandle, float aVolumePct)
{
	return myImpl->SetVolume(aGroupHandle, aVolumePct);
}

bool SoundEngine::SetChannelGroupVolumeDB(const ChannelGroupHandle& aGroupHandle, float aVolumeDB)
{
	const float volumePct = powf(10.0f, 0.05f * aVolumeDB);
	return myImpl->SetVolume(aGroupHandle, volumePct);
}

bool SoundEngine::SetEventListenerMask(const SoundEventInstanceHandle& anEventInstanceHandle, unsigned aMask)
{
	return myImpl->SetEventListenerMask(anEventInstanceHandle, aMask);
}

bool SoundEngine::SetEvent3DParameters(const SoundEventInstanceHandle& anEventInstanceHandle,
	std::array<float, 3> aPosition, std::array<float, 3> aVelocity, std::array<float, 3> aForwardVector, std::array<float,
	3> aUpVector)
{
	return myImpl->SetEvent3DParameters(anEventInstanceHandle, aPosition, aVelocity, aForwardVector, aUpVector);
}

bool SoundEngine::SetEventPitch(const SoundEventInstanceHandle& anEventInstanceHandle, float aPitch)
{
	return myImpl->SetEventPitch(anEventInstanceHandle, aPitch);
}

bool SoundEngine::SetEventReverbLevel(const SoundEventInstanceHandle& anEventInstanceHandle, int anIndex, float aLevel)
{
	return myImpl->SetEventReverbLevel(anEventInstanceHandle, anIndex, aLevel);
}

SoundEngine::ListenerHandle SoundEngine::GetNextFreeListener()
{
	const int numListeners = myImpl->GetNumListeners(); // We always have at least one listener.
	if (numListeners < 8)
	{
		Listener newListener;
		newListener.FmodId = numListeners;
		newListener.ListenerId = ++nextListenerId;

		myImpl->AddListener(newListener.Location, newListener.Velocity, newListener.Forward, newListener.Up);
		myListeners[newListener.ListenerId] = newListener;

		return newListener.Handle();
	}

	return ListenerHandle();
}

bool SoundEngine::OverrideOtherListeners(ListenerHandle aHandle)
{
	return myImpl->OverrideOtherListeners(aHandle.GetId());
}

bool SoundEngine::RemoveListener(ListenerHandle aHandle)
{
	if (aHandle.IsValid())
	{
		const auto listener = myListeners.find(aHandle.GetId());
		if (listener != myListeners.end())
		{
			return myImpl->RemoveListener(listener->second.FmodId);
		}
	}

	return false;
}

bool SoundEngine::SetListenerPosition(ListenerHandle aHandle, std::array<float, 3> aPosition)
{
	std::cout << "Is handle valid: " << aHandle.IsValid() << std::endl;
	if (aHandle.IsValid())
	{
		Listener& listener = myListeners[aHandle.GetId()];
		std::cout << "Listener FMODID " << listener.FmodId << " Number of listeners: " << myImpl->GetNumListeners() - 1 << std::endl;
		if (listener.FmodId > myImpl->GetNumListeners() - 1)
			return false;

		std::array<float, 3> pos, vel, fwd, up;
		if (myImpl->GetListenerTransform(listener.FmodId, pos, vel, fwd, up))
		{
			myImpl->SetListenerTransform(listener.FmodId, aPosition, vel, fwd, up);
			std::cout << "Listener " << listener.FmodId << " Pos " << aPosition[0] << std::endl;
			return true;
		}
	}

	return false;
}

bool SoundEngine::SetListenerTransform(ListenerHandle aHandle, const std::array<float, 3> aPosition,
	const std::array<float, 3> aVelocity, const std::array<float, 3> aForwardVector,
	const std::array<float, 3> aUpVector)
{
	if (aHandle.IsValid())
	{
		const Listener& listener = myListeners[aHandle.GetId()];
		if (listener.FmodId > myImpl->GetNumListeners() - 1)
			return false;

		return myImpl->SetListenerTransform(listener.FmodId, aPosition, aVelocity, aForwardVector, aUpVector);
	}

	return false;
}

bool SoundEngine::SetListenerTransform(ListenerHandle aHandle, const std::array<float, 16> aTransform, const std::array<float, 3> aVelocity)
{
	if (aHandle.IsValid())
	{
		const Listener& listener = myListeners[aHandle.GetId()];
		if (listener.FmodId > myImpl->GetNumListeners() - 1)
			return false;

		// We need to extract position, up and forward.
		const std::array<float, 3> position = { aTransform[12], aTransform[13], aTransform[14] };

		std::array<float, 16> rot = aTransform;

		// Zero out location row from rotation.
		rot[12] = 0;
		rot[13] = 0;
		rot[14] = 0;

		// Extract Up and Forward vectors.
		std::array<float, 4> forward = { 0, 0, 1, 1 };
		std::array<float, 4> up = { 0, 1, 0, 1 };

		forward[0] = (rot[0] * forward[0]) + (rot[4] * forward[1]) + (rot[8] * forward[2]) + (rot[12] * forward[3]);
		forward[1] = (rot[1] * forward[0]) + (rot[5] * forward[1]) + (rot[9] * forward[2]) + (rot[13] * forward[3]);
		forward[2] = (rot[2] * forward[0]) + (rot[6] * forward[1]) + (rot[10] * forward[2]) + (rot[14] * forward[3]);
		forward[3] = (rot[3] * forward[0]) + (rot[7] * forward[1]) + (rot[11] * forward[2]) + (rot[15] * forward[3]);

		up[0] = (rot[0] * up[0]) + (rot[4] * up[1]) + (rot[8] * up[2]) + (rot[12] * up[3]);
		up[1] = (rot[1] * up[0]) + (rot[5] * up[1]) + (rot[9] * up[2]) + (rot[13] * up[3]);
		up[2] = (rot[2] * up[0]) + (rot[6] * up[1]) + (rot[10] * up[2]) + (rot[14] * up[3]);
		up[3] = (rot[3] * up[0]) + (rot[7] * up[1]) + (rot[11] * up[2]) + (rot[15] * up[3]);

		const std::array<float, 3> forward3Axis = { forward[0], forward[1], forward[2] };
		const std::array<float, 3> up3Axis = { up[0], up[1], up[2] };

		return myImpl->SetListenerTransform(listener.FmodId, position, aVelocity, forward3Axis, up3Axis);
	}

	return false;
}

bool SoundEngine::RegisterCallbackLambda(const SoundEventInstanceHandle& anEventInstanceHandle, int aCallbackType,
	EventCallbackLambda::Lambda aLambda)
{
	std::shared_ptr<EventCallbackLambda> callbackPtr(new EventCallbackLambda(aCallbackType, std::move(aLambda)));
	return myImpl->RegisterCallback(anEventInstanceHandle, callbackPtr);
}

bool SoundEngine::RegisterCallbackInternal(const SoundEventInstanceHandle& anEventInstanceHandle, std::shared_ptr<EventCallbackBase> aPtr)
{
	myImpl->RegisterCallback(anEventInstanceHandle, std::move(aPtr));
	return true;
}

SERESULT SoundEngine::GetLastError()
{
#pragma warning(disable: 26812)
	return static_cast<SERESULT>(myImpl->GetLastError());
#pragma warning(default: 26812)
}
