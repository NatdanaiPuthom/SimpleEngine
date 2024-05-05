#pragma once
#include <string>
#include <vector>

namespace Simpleton
{
	class AudioManager
	{
		struct Channel
		{
			Channel(std::string aFileName, size_t aChannelIndex) : myChannelName(aFileName), myChannelIndex(aChannelIndex) {}
			std::string myChannelName;
			size_t myChannelIndex;
		};

	public:
		//Remember to call destructor for all singletons before main calling return 0
		static AudioManager& GetInstance()
		{
			static AudioManager myInstance;
			return myInstance;
		}

		~AudioManager();

		// Delete copy constructor and copy assignment operator
		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;

		// Default move constructor and move assignment operator
		AudioManager(AudioManager&&) = default;
		AudioManager& operator=(AudioManager&&) = default;

		void Init();
		void Update();

		void ToggleMuteMusic();
		void ToggleMuteSFX();

		void PlaySFX(std::string aFileName);
		void PlayMusic(std::string aFilename);

		void StopSFX(std::string aFileName);
		void StopMusic(std::string aFileName);
		void StopAllMusic();

		//Range: 0 - 1f.
		void ChangeSFXVolume(float aVolume);

		//Range: 0 - 1f.
		void ChangeMusicVolume(float aVolume);

		float GetMusicVolume() const;
		float GetSFXVolume() const;

	private:
		AudioManager();
	private:
		std::string myFilePathRoot;

		float mySFXVolume;
		float myMusicVolume;
		float mySFXVolMute;
		float myMusicVolMute;

		bool myMusicMuted;
		bool mySFXMuted;

		std::vector<Channel> myMusicChannels;
		std::vector<Channel> mySFXChannels;
	};
}