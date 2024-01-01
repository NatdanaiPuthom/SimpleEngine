#pragma once
#include <External/FMOD/SoundEngine.h>

class AudioManager
{
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
	void Destroy();

	void PlayMusic(const std::string aFileName);
	void StopMusic(const std::string aFileName);

	void PlaySFX(const std::string aFileName);
	void StopSFX(const std::string aFileName);

	void StopAllMusic();

	//Range: 0 - 1f.
	void SetMusicVolume(float aVolume);
	void SetSFXVolume(float aVolume);

	const std::string& GetCurrentMusicName();

	void ToggleMuteMusic();
	void ToggleMuteSFX();

public:
	bool IsMusicMuted() const;
	bool IsSFXMuted() const;
private:
	AudioManager();

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

	friend class OptionsState;
};