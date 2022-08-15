#pragma once
#include <string>
#include <vector>

#include "Channel.h"
#include "CustomDSP.h"
#include "ErrorHandler.h"
#include "Sound.h"

namespace FMOD
{
	class System;
}

namespace AudioEngine
{
	class System
	{
	public:
		System();
		~System()
		{
			ERRCHECK(system->close());
			ERRCHECK(system->release());
		}


		RESULT InitSystem(int maxChannels);

		RESULT Update() const;

		RESULT LoadSound(const char* path, Sound& sound, Sound::MODE mode) const;
		RESULT LoadDSP(Channel& channel, CustomDSP& dsp);
		RESULT PlaySound(const Sound& sound, Channel& channel, bool startPaused = false) const;

		RESULT GetPlayingChannelCount(int* count);

		void ReadFile(const char* path, int16_t** buffer, unsigned int* outLength);


	private:
		FMOD::System* system;
	};
}

