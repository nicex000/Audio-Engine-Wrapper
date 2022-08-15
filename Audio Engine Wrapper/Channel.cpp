#include "Channel.h"

#include "Sound.h"

namespace AudioEngine
{
	Channel::Channel(): channel(nullptr)
	{
		ended = true;
	}

	RESULT Channel::Play(const System& system, const Sound& sound, bool startPaused)
	{
		return system.PlaySound(sound, *this, startPaused);
	}

	RESULT Channel::Stop()
	{
		return ERRCHECK(channel->stop());
	}

	RESULT Channel::HasEnded(bool& outIsEnded)
	{
		outIsEnded = ended;
		return RESULT::OK;
	}

	RESULT Channel::SetVolume(float volume)
	{
		return ERRCHECK(channel->setVolume(volume));
	}

	RESULT Channel::GetVolume(float& outVolume)
	{
		return ERRCHECK(channel->getVolume(&outVolume));
	}

	RESULT Channel::SetVolumeMode(VOLUME_MODE mode)
	{
		switch (mode) { case VOLUME_MODE::INSTANT: return ERRCHECK(channel->setVolumeRamp(false));
		case VOLUME_MODE::GRADUAL: return ERRCHECK(channel->setVolumeRamp(true));
		default: return RESULT::ERR_INVALID_VOLUME_MODE;
		}
	}

	RESULT Channel::GetVolumeMode(VOLUME_MODE& outMode)
	{
		bool ramp;
		const RESULT res = ERRCHECK(channel->getVolumeRamp(&ramp));

		outMode = ramp ? VOLUME_MODE::GRADUAL : VOLUME_MODE::INSTANT;

		return res;
	}

	RESULT Channel::SetPan(float amount, PAN_DIRECTION direction)
	{
		switch (direction) {
		case PAN_DIRECTION::LEFT: 
			panAmount = amount;
			panDirection = direction;
			return ERRCHECK(channel->setPan(amount * -1.f));
		case PAN_DIRECTION::RIGHT: 
			panAmount = amount;
			panDirection = direction;
			return ERRCHECK(channel->setPan(amount));
		default: return RESULT::ERR_INVALID_PAN_DIRECTION;
		}
	}

	RESULT Channel::GetPan(float& outAmount, PAN_DIRECTION& outDirection)
	{
		outAmount = panAmount;
		outDirection = panDirection;
		return RESULT::OK;
	}

	RESULT Channel::IsPlaying(bool& outIsPlaying)
	{
		if (ended)
		{
			outIsPlaying = false;
			return RESULT::OK;
		}
		return ERRCHECK(channel->isPlaying(&outIsPlaying));
	}

	RESULT Channel::IsPaused(bool& outIsPaused)
	{
		return ERRCHECK(channel->getPaused(&outIsPaused));
	}

	RESULT Channel::Pause(bool pause)
	{
		return ERRCHECK(channel->setPaused(pause));
	}

	RESULT Channel::SetPositionInMs(unsigned Pos)
	{
		return ERRCHECK(channel->setPosition(Pos, FMOD_TIMEUNIT_MS));
	}

	RESULT Channel::GetPositionInMs(unsigned int& outPos)
	{
		return ERRCHECK(channel->getPosition(&outPos, FMOD_TIMEUNIT_MS));
	}

	RESULT Channel::GetPositionInPCM(unsigned& outPos)
	{
		return ERRCHECK(channel->getPosition(&outPos, FMOD_TIMEUNIT_PCM));
	}

	RESULT Channel::GetCurrentSound(Sound& outCurrentSound)
	{
		return ERRCHECK(channel->getCurrentSound(&outCurrentSound.sound));
	}

	void Channel::AttachDSP(CustomDSP* dsp)
	{
		attachedDSP = dsp;
	}

	CustomDSP* Channel::GetDsp()
	{
		return attachedDSP;
	}

	void Channel::UpdateDSP()
	{
		if (!ended && channel)
		{
			bool paused;
			IsPaused(paused);
			if (!paused)
				attachedDSP->Update();
		}
	}

	FMOD_RESULT Channel::ChannelCallback(FMOD_CHANNELCONTROL* channelcontrol,
	                                     FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1,
	                                     void* commanddata2)
	{
		if (controltype == FMOD_CHANNELCONTROL_CHANNEL && callbacktype == FMOD_CHANNELCONTROL_CALLBACK_END)
		{
			void* channel;
			const RESULT res = ERRCHECK(reinterpret_cast<FMOD::Channel*>(channelcontrol)->getUserData(&channel));

			if (res == RESULT::OK)
			{
				static_cast<Channel*>(channel)->ended = true;
				static_cast<Channel*>(channel)->channel = nullptr;
			}
		}
		return FMOD_OK;
	}
}
