#pragma once
#include "FMOD Core/inc/fmod_common.h"

namespace AudioEngine
{
	enum class RESULT
	{
		OK,
		ERR_CHANNEL_STOLEN,
		ERR_INVALID_HANDLE,
		ERR_INVALID_SOUND_MODE,
		ERR_INVALID_LOOP_MODE,
		ERR_INVALID_VOLUME_MODE,
		ERR_INVALID_PAN_DIRECTION,
		ERR_SYSTEM_CREATION_FAILED,
		ERR_UNKNOWN
	};

	class ErrorHandler
	{
		//adopting the errcheck in the FMOD examples, just so i can display the error line
		static RESULT ErrCheck_Implementation(FMOD_RESULT result, const char* file, int line);

		friend class System;
		friend class Channel;
		friend class Sound;
		friend class CustomDSP;
	};
}

#define ERRCHECK(_result) ErrorHandler::ErrCheck_Implementation(_result, __FILE__, __LINE__)

