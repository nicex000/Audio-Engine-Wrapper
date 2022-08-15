#pragma once
#include "ErrorHandler.h"
#include "FMOD Core/inc/fmod.hpp"

namespace AudioEngine
{
	class System;

	class Sound
	{
	public:
		enum class MODE
		{
			STATIC,
			STREAM
		};

		enum class LOOP_MODE
		{
			OFF,
			NORMAL,
			BIDIRECTIONAL
		};
		Sound();
		Sound(const System& system, const char* path, MODE mode, LOOP_MODE loopMode);

		~Sound()
		{
			sound->release();
			delete sound;
		}


		RESULT GetLoopMode(LOOP_MODE& outLoopMode);
		RESULT SetLoopMode(LOOP_MODE loopMode);
		RESULT GetLengthInMs(unsigned int* len);



	private:
		FMOD::Sound* sound;

		friend class System;
		friend class Channel;

	};
}
