#pragma once
#include <External/FMOD/SoundEngine.h>

namespace Simple
{
	class AudioManager
	{
	public:
		AudioManager();
		~AudioManager();

		void Init();
		void Update();

		void PlayMusic(const std::string aFileName);
		void StopMusic(const std::string aFileName);

		void PlaySFX(const std::string aFileName);
		void StopSFX(const std::string aFileName);

		void StopAllMusic();

		void SetMusicVolume(const float aVolume);
		void SetSFXVolume(const float aVolume);

		const std::string& GetCurrentMusicName();

		void ToggleMuteMusic();
		void ToggleMuteSFX();

		bool IsMusicMuted() const;
		bool IsSFXMuted() const;
	private:
		struct Channel
		{
			Channel(std::string aFileName, size_t aChannelIndex) : myChannelName(aFileName), myChannelIndex(aChannelIndex) {}
			std::string myChannelName;
			size_t myChannelIndex;
		};
	private:
		std::vector<Channel> myMusicChannels;
		std::vector<Channel> mySFXChannels;
		std::string myCurrentMusic;

		float myMusicVolume;
		float mySFXVolume;
		float mySFXVolumeMute;

		bool myMusicMuted;
		bool mySFXMuted;
	};
}