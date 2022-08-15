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
		RESULT LoadDSP(Channel& channel, CustomDSP& dsp) const;
		RESULT PlaySound(const Sound& sound, Channel& channel, bool startPaused = false) const;

		RESULT GetPlayingChannelCount(int& count) const;

		RESULT PauseMasterChannel(bool pause) const;
		RESULT IsMasterChannelPaused(bool& outPause) const;
		RESULT StopAll() const;

	private:
		FMOD::System* system;
	};
}

