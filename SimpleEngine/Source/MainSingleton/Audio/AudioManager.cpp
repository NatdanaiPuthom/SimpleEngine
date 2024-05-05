#include "MainSingleton/Audio/AudioManager.h"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <External/TheGameAssembly/SoundEngine/SoundEngine.h>
#include <External/TheGameAssembly/SoundEngine/SoundEngineImpl.hpp>

namespace Simpleton
{
	AudioManager::AudioManager()
	{
		mySFXVolume = 1.0f;
		myMusicVolume = 1.0f;
		mySFXVolMute = 0.0f;
		myMusicVolMute = 0.0f;

		myMusicMuted = false;
		mySFXMuted = false;
	}

	AudioManager::~AudioManager()
	{
		SoundEngine::Release();
		mySFXChannels.clear();
		myMusicChannels.clear();
	}

	void AudioManager::Init()
	{
		ChangeMusicVolume(0.1f); //NOTE(v9.35.1): "StardewValley.mp3" test were too loud 

		//Dont touch
		SoundEngine::Init(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_AUDIO));

		//Music
		SoundEngine::LoadSoundFile("StardewValley.mp3", false, false, true);

		//SFX
		//SoundEngine::LoadSoundFile("");
	}

	void AudioManager::PlayMusic(std::string aFileName)
	{
		myMusicChannels.push_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, myMusicVolume)));
	}

	void AudioManager::ToggleMuteMusic()
	{
		if (!myMusicMuted)
		{
			ChangeMusicVolume(myMusicVolMute);
			myMusicMuted = true;
		}
		else
		{
			ChangeMusicVolume(myMusicVolume);
			myMusicMuted = false;
		}
	}

	void AudioManager::ToggleMuteSFX()
	{
		if (!mySFXMuted)
		{
			ChangeSFXVolume(mySFXVolMute);
			mySFXMuted = true;
		}
		else
		{
			ChangeSFXVolume(mySFXVolume);
			mySFXMuted = false;
		}
	}

	void AudioManager::PlaySFX(std::string aFileName)
	{
		mySFXChannels.push_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, mySFXVolume)));
	}

	void AudioManager::StopSFX(const std::string aFileName)
	{
		for (size_t i = 0; i < mySFXChannels.size(); i++)
		{
			if (mySFXChannels[i].myChannelName == aFileName)
			{
				SoundEngine::StopSound(mySFXChannels[i].myChannelIndex);
			}
		}
	}

	void AudioManager::StopMusic(const std::string aFileName)
	{
		for (size_t i = 0; i < myMusicChannels.size(); i++)
		{
			if (myMusicChannels[i].myChannelName == aFileName)
			{
				SoundEngine::StopSound(myMusicChannels[i].myChannelIndex);
				myMusicChannels.erase(myMusicChannels.begin() + i);
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

	float AudioManager::GetSFXVolume() const
	{
		return mySFXVolume;
	}

	void AudioManager::ChangeSFXVolume(const float aVolume)
	{
		mySFXVolume = aVolume;

		for (size_t i = 0; i < mySFXChannels.size(); i++)
		{
			SoundEngine::SetVolume(static_cast<int>(mySFXChannels[i].myChannelIndex), mySFXVolume);
		}
	}

	float AudioManager::GetMusicVolume() const
	{
		return myMusicVolume;
	}

	void AudioManager::ChangeMusicVolume(const float aVolume)
	{
		myMusicVolume = aVolume;

		for (size_t i = 0; i < myMusicChannels.size(); i++)
		{
			SoundEngine::SetVolume(static_cast<int>(myMusicChannels[i].myChannelIndex), myMusicVolume);
		}
	}

	void AudioManager::Update()
	{
		SoundEngine::Update();

		const auto soundEngine = SoundEngine::GetImpl();

		for (size_t i = 0; i < myMusicChannels.size(); ++i)
		{
			if (soundEngine->IsPlaying(myMusicChannels[i].myChannelIndex) == false)
			{
				myMusicChannels.erase(myMusicChannels.begin() + i);
			}
		}
	}
}