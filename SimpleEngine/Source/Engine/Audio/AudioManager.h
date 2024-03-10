#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Channel
{
	Channel(std::string aFileName, size_t aChannelIndex) : myChannelName(aFileName), myChannelIndex(aChannelIndex) {}
	std::string myChannelName;
	size_t myChannelIndex;
};

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

	//Init has already been called in main, dont touch!
	void Init();
	//Update is called in main, dont touch!
	void Update();

	//Plays sound from filename
	void PlayASound(std::string aFileName);
	void StopSound(std::string aFileName);

	//Range: 0 - 1f.
	float GetMusicVolume();
	void ChangeMusicVolume(float aVolume);

	//Range: 0 - 1f.
	float GetSFXVolume();
	void ChangeSFXVolume(float aVolume);

	void StopMusic(std::string aFileName);
	void StopAllMusic();
	void PlayMusic(std::string aFilename);

	void ToggleMuteMusic();
	void ToggleMuteSFX();

private:
	AudioManager();
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