#define _CRT_SECURE_NO_WARNINGS
#include "System.h"

#include <cstdio>

#include "ErrorHandler.h"
#include "FMOD Core/inc/fmod.hpp"

namespace AudioEngine
{
	System::System()
	{
	}

	RESULT System::InitSystem(int maxChannels)
	{
		FMOD_RESULT result = FMOD::System_Create(&system);
		if (ERRCHECK(result) != RESULT::OK) return RESULT::ERR_SYSTEM_CREATION_FAILED;

		result = system->init(maxChannels, FMOD_INIT_NORMAL, nullptr);
		return ERRCHECK(result);
	}

	RESULT System::Update() const
	{
		const FMOD_RESULT result = system->update();
		return ERRCHECK(result);
	}

	RESULT System::LoadSound(const char* path, Sound& sound, Sound::MODE mode) const
	{
		int fmodMode;
		switch (mode) {
		case Sound::MODE::STATIC: fmodMode = FMOD_DEFAULT;  break;
		case Sound::MODE::STREAM: fmodMode = FMOD_CREATESTREAM; break;
		default:
			printf("Invalid Sound Mode");
			return RESULT::ERR_INVALID_SOUND_MODE;
		}

		return ERRCHECK(system->createSound(path, fmodMode, nullptr, &sound.sound));

	}

	RESULT System::LoadDSP(Channel& channel, CustomDSP& dsp) const
	{
		FMOD::ChannelGroup* mastergroup;
		FMOD_RESULT         result;
		dsp.CreateDSPDesc(system);


		//result = system->getMasterChannelGroup(&mastergroup);
		//ERRCHECK(result);

		//result = mastergroup->addDSP(0, dsp.dsp);

		result = system->createChannelGroup("", &mastergroup);
		ERRCHECK(result);
		result = channel.channel->setChannelGroup(mastergroup);
		ERRCHECK(result);
		result = mastergroup->addDSP(0, dsp.dsp);
		ERRCHECK(result);



		channel.AttachDSP(&dsp);
		
		return ERRCHECK(result);
	}

	RESULT System::PlaySound(const Sound& sound, Channel& channel, bool startPaused) const
	{
		RESULT result = RESULT::OK;
		if (!channel.ended)
		{
			result = channel.Stop();
			delete channel.channel;
			channel.channel = nullptr;
		}
		if (result != RESULT::OK) return result;

		result = ERRCHECK(system->playSound(
			sound.sound, nullptr, startPaused, &channel.channel));
		if (result == RESULT::OK)
		{
			channel.ended = false;
			result = ERRCHECK(channel.channel->setUserData(&channel));
			if (result == RESULT::OK)
			{
				result = ERRCHECK(channel.channel->setCallback(Channel::ChannelCallback));
			}
		}
		return result;
	}

	RESULT System::GetPlayingChannelCount(int& count) const
	{
		return ERRCHECK(system->getChannelsPlaying(&count));
	}

	RESULT System::PauseMasterChannel(bool pause) const
	{
		FMOD::ChannelGroup* masterChannel;
		ERRCHECK(system->getMasterChannelGroup(&masterChannel));
		return ERRCHECK(masterChannel->setPaused(pause));
	}

	RESULT System::IsMasterChannelPaused(bool& outPause) const
	{
		FMOD::ChannelGroup* masterChannel;
		ERRCHECK(system->getMasterChannelGroup(&masterChannel));
		return ERRCHECK(masterChannel->getPaused(&outPause));
	}

	RESULT System::StopAll() const
	{
		FMOD::ChannelGroup* masterChannel;
		ERRCHECK(system->getMasterChannelGroup(&masterChannel));
		return ERRCHECK(masterChannel->stop());
	}
}

