#include "ErrorHandler.h"

#include <cstdio>
#include <inc/fmod_errors.h>

namespace AudioEngine
{
	RESULT ErrorHandler::ErrCheck_Implementation(FMOD_RESULT result, const char* file, int line)
	{
		RESULT res = RESULT::ERR_UNKNOWN;

		switch (result)
		{
		case FMOD_RESULT::FMOD_OK:
			res = RESULT::OK;
			break;


		case FMOD_RESULT::FMOD_ERR_INVALID_HANDLE:
			res = RESULT::ERR_INVALID_HANDLE;
			break;
		case FMOD_RESULT::FMOD_ERR_CHANNEL_STOLEN:
			res = RESULT::ERR_CHANNEL_STOLEN;
			break;
		}

		if (res != RESULT::OK) 
			printf("%s(%d): FMOD error %d - %s\n", file, line, result, FMOD_ErrorString(result));

		return res;
	}
}
