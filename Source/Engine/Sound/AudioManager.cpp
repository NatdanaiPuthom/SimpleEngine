#include "Engine/Precomplier/stdafx.h"
#include "Engine/Sound/AudioManager.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

AudioManager::AudioManager()
	: myMusicVolume(1.0f)
	, mySFXVolume(1.0f)
	, mySFXVolumeMute(0)
	, myMusicMuted(false)
	, mySFXMuted(false)
{
	SimpleGlobalAudioManagerImpl::SetAudioManager(this);
}

AudioManager::~AudioManager()
{
	SoundEngine::Release();
}

void AudioManager::Init()
{
	const std::string root = SimpleUtilities::GetPath(SIMPLE_SOUND_DIR);

	if (!SoundEngine::Init(root))
		assert(false && "Failed to init SoundEngine");

	if (!SoundEngine::LoadSoundFile("StardewValley.mp3", false, false, true))
		assert(false && "Failed to load music file");

	PlayMusic("StardewValley.mp3");
	SetMusicVolume(0.1f);	
}

void AudioManager::Update()
{
	SoundEngine::Update();
}

void AudioManager::PlayMusic(const std::string aFileName)
{
	myCurrentMusic = aFileName;
	myMusicChannels.emplace_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, myMusicVolume)));
}

void AudioManager::StopMusic(const std::string aFileName)
{
	for (auto it = myMusicChannels.begin(); it != myMusicChannels.end(); ++it)
	{
		if (it->myChannelName == aFileName)
		{
			SoundEngine::StopSound(it->myChannelIndex);
			mySFXChannels.erase(it);
			break;
		}
	}
}

void AudioManager::PlaySFX(const std::string aFileName)
{

	mySFXChannels.emplace_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, mySFXVolume)));
}

void AudioManager::StopSFX(const std::string aFileName)
{
	for (auto it = mySFXChannels.begin(); it != mySFXChannels.end(); ++it)
	{
		if (it->myChannelName == aFileName)
		{
			SoundEngine::StopSound(it->myChannelIndex);
			mySFXChannels.erase(it);
			break;
		}
	}
}

void AudioManager::StopAllMusic()
{
	for (size_t i = 0; i < myMusicChannels.size(); i++)
	{
		SoundEngine::StopSound(myMusicChannels[i].myChannelIndex);
	}
}

void AudioManager::SetMusicVolume(const float aVolume)
{
	myMusicVolume = SU::Clamp(aVolume, 0.0f, 1.0f);

	for (size_t i = 0; i < myMusicChannels.size(); i++)
	{
		SoundEngine::SetVolume(static_cast<int>(myMusicChannels[i].myChannelIndex), myMusicVolume);
	}
}

void AudioManager::SetSFXVolume(const float aVolume)
{
	mySFXVolume = SU::Clamp(aVolume, 0.0f, 1.0f);

	for (size_t i = 0; i < mySFXChannels.size(); i++)
	{
		SoundEngine::SetVolume(static_cast<int>(mySFXChannels[i].myChannelIndex), mySFXVolume);
	}
}

const std::string& AudioManager::GetCurrentMusicName()
{
	return myCurrentMusic;
}

void AudioManager::ToggleMuteMusic()
{
	float volume = 0;

	if (myMusicMuted == true)
	{
		volume = myMusicVolume;
	}

	for (size_t i = 0; i < myMusicChannels.size(); i++)
	{
		SoundEngine::SetVolume(static_cast<int>(myMusicChannels[i].myChannelIndex), volume);
	}

	myMusicMuted = !myMusicMuted;
}

void AudioManager::ToggleMuteSFX()
{
	if (mySFXMuted == false)
	{
		mySFXVolumeMute = mySFXVolume;
		mySFXVolume = 0;
	}
	else
	{
		mySFXVolume = mySFXVolumeMute;
	}

	for (size_t i = 0; i < mySFXChannels.size(); i++)
	{
		SoundEngine::SetVolume(static_cast<int>(mySFXChannels[i].myChannelIndex), mySFXVolume);
	}

	mySFXMuted = !mySFXMuted;
}

bool AudioManager::IsMusicMuted() const
{
	return myMusicMuted;
}

bool AudioManager::IsSFXMuted() const
{
	return mySFXMuted;
}