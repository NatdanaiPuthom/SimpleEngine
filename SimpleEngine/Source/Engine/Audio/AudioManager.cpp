#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Audio/AudioManager.h"
//#include <External/TheGameAssembly/SoundEngine/SoundEngine.h>
//
//AudioManager::AudioManager() 
//{
//	mySFXVolume = 1.0f;
//	myMusicVolume = 1.0f;
//	mySFXVolMute = 0.0f;
//	myMusicVolMute = 0.0f;
//
//	myMusicMuted = false;
//	mySFXMuted = false;
//}
//
//AudioManager::~AudioManager()
//{
//}
//
//void AudioManager::Destroy()
//{
//	SoundEngine::Release();
//	mySFXChannels.clear();
//	myMusicChannels.clear();
//}
//
//void AudioManager::Init()
//{
//	//Dont touch
//	SoundEngine::Init(SIMPLE_AUDIO_DIR);
//
//	//Music
//	//SoundEngine::LoadSoundFile("Music/Tavern_music_menu.ogg", false, false, true);
//
//	//SFX
//	//SoundEngine::LoadSoundFile("Player/climb.ogg");
//}
//
//void AudioManager::PlayMusic(std::string aFileName)
//{
//	myMusicChannels.push_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, myMusicVolume)));
//}
//
//void AudioManager::ToggleMuteMusic()
//{
//	if (!myMusicMuted)
//	{
//		ChangeMusicVolume(myMusicVolMute);
//		myMusicMuted = true;
//	}
//	else
//	{
//		ChangeMusicVolume(myMusicVolume);
//		myMusicMuted = false;
//	}
//}
//
//void AudioManager::ToggleMuteSFX()
//{
//	if (!mySFXMuted)
//	{
//		ChangeSFXVolume(mySFXVolMute);
//		mySFXMuted = true;
//	}
//	else
//	{
//		ChangeSFXVolume(mySFXVolume);
//		mySFXMuted = false;
//	}
//}
//
//void AudioManager::PlayASound(std::string aFileName)
//{
//	mySFXChannels.push_back(Channel(aFileName, SoundEngine::PlaySound(aFileName, mySFXVolume)));
//}
//
//void AudioManager::StopSound(const std::string aFileName)
//{
//	for (size_t i = 0; i < mySFXChannels.size(); i++)
//	{
//		if (mySFXChannels[i].myChannelName == aFileName)
//		{
//			SoundEngine::StopSound(mySFXChannels[i].myChannelIndex);
//		}
//	}
//}
//
//void AudioManager::StopMusic(const std::string aFileName)
//{
//	for (size_t i = 0; i < myMusicChannels.size(); i++)
//	{
//		if (myMusicChannels[i].myChannelName == aFileName)
//		{
//			SoundEngine::StopSound(myMusicChannels[i].myChannelIndex);
//		}
//	}
//}
//
//void AudioManager::StopAllMusic()
//{
//	for (size_t i = 0; i < myMusicChannels.size(); i++)
//	{
//		SoundEngine::StopSound(myMusicChannels[i].myChannelIndex);
//	}
//}
//
//float AudioManager::GetSFXVolume()
//{
//	return mySFXVolume;
//}
//
//void AudioManager::ChangeSFXVolume(const float aVolume)
//{
//	mySFXVolume = aVolume;
//	for (size_t i = 0; i < mySFXChannels.size(); i++)
//	{
//		SoundEngine::SetVolume(static_cast<int>(mySFXChannels[i].myChannelIndex), mySFXVolume);
//	}
//}
//
//float AudioManager::GetMusicVolume()
//{
//	return myMusicVolume;
//}
//
//void AudioManager::ChangeMusicVolume(const float aVolume)
//{
//	myMusicVolume = aVolume;
//	for (size_t i = 0; i < myMusicChannels.size(); i++)
//	{
//		SoundEngine::SetVolume(static_cast<int>(myMusicChannels[i].myChannelIndex), myMusicVolume);
//	}
//}
//
//void AudioManager::Update()
//{
//	SoundEngine::Update();
//}