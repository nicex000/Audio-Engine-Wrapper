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
		Sound(const System& system, const char* path, MODE mode, LOOP_MODE loopMode, RESULT& outResult);

		~Sound()
		{
			sound->release();
		}


		RESULT GetLoopMode(LOOP_MODE& outLoopMode) const;
		RESULT SetLoopMode(LOOP_MODE loopMode) const;
		RESULT GetLengthInMs(unsigned int& len) const;
		RESULT GetLengthInPCM(unsigned int& len) const;



	private:
		FMOD::Sound* sound;

		friend class System;
		friend class Channel;

	};
}
