#include "Sound.h"
#include "System.h"

AudioEngine::Sound::Sound()
{
}

AudioEngine::Sound::Sound(const System& system, const char* path, Sound::MODE mode, LOOP_MODE loopMode, RESULT& outResult)
{
	outResult = system.LoadSound(path, *this, mode);
	if (outResult == RESULT::OK)
		SetLoopMode(loopMode);
}

AudioEngine::RESULT AudioEngine::Sound::GetLoopMode(LOOP_MODE& outLoopMode) const
{
	FMOD_MODE mode;
	const RESULT res = ERRCHECK(sound->getMode(&mode));

	switch (mode)
	{
	case FMOD_LOOP_OFF: outLoopMode = LOOP_MODE::OFF; break;
	case FMOD_LOOP_NORMAL: outLoopMode = LOOP_MODE::NORMAL; break;
	case FMOD_LOOP_BIDI: outLoopMode = LOOP_MODE::BIDIRECTIONAL; break;
	default: return RESULT::ERR_INVALID_LOOP_MODE;
	}

	return res;
}

AudioEngine::RESULT AudioEngine::Sound::SetLoopMode(LOOP_MODE loopMode) const
{
	switch (loopMode) { case LOOP_MODE::OFF: return ERRCHECK(sound->setMode(FMOD_LOOP_OFF));
	case LOOP_MODE::NORMAL: return ERRCHECK(sound->setMode(FMOD_LOOP_NORMAL));
	case LOOP_MODE::BIDIRECTIONAL: return ERRCHECK(sound->setMode(FMOD_LOOP_BIDI));
	default: return RESULT::ERR_INVALID_LOOP_MODE;
	}
}

AudioEngine::RESULT AudioEngine::Sound::GetLengthInMs(unsigned& len) const
{
	return ERRCHECK(sound->getLength(&len, FMOD_TIMEUNIT_MS));
}

AudioEngine::RESULT AudioEngine::Sound::GetLengthInPCM(unsigned& len) const
{
	return ERRCHECK(sound->getLength(&len, FMOD_TIMEUNIT_PCM));
}
