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

	RESULT System::LoadDSP(Channel& channel, CustomDSP& dsp)
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

	RESULT System::GetPlayingChannelCount(int* count)
	{
		return ERRCHECK(system->getChannelsPlaying(count));
	}

	void System::ReadFile(const char* path, int16_t** buffer, unsigned int* outLength)
	{
		FMOD::Sound* sound;

		system->createSound(path, FMOD_DEFAULT, nullptr, &sound);
		FMOD_SOUND_FORMAT format;
		FMOD_SOUND_TYPE type;
		int a, b;

		sound->getFormat(&type, &format, &a, &b);



		unsigned int length, read = 0;
		void* buf = nullptr;
		ERRCHECK(sound->getLength(&length, FMOD_TIMEUNIT_PCMBYTES));
		//ERRCHECK(sound->readData(buf, 10, &read));
		*outLength = read / sizeof(int16_t);

		void* buf1 = nullptr, *buf2 = nullptr;
		unsigned int l1, l2;
		ERRCHECK(sound->lock(0, length, &buf1, &buf2, &l1, &l2));
		*outLength = l1/sizeof(int16_t);
		*buffer = static_cast<int16_t*>(malloc(l1));
		memcpy(*buffer, buf1, l1);
		sound->unlock(buf1, buf2, l1, l2);
		sound->release();
	}
}

