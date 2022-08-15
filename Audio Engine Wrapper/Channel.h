#pragma once
#include "FMOD Core/inc/fmod.hpp"

#include "ErrorHandler.h"
#include "System.h"
#include "CustomDSP.h"


namespace AudioEngine
{
	class Channel
	{
	public:

		enum class VOLUME_MODE
		{
			INSTANT,
			GRADUAL
		};

		enum class PAN_DIRECTION
		{
			LEFT,
			RIGHT
		};
		Channel();
		~Channel()
		{
			FMOD::ChannelGroup* chGroup;
			channel->getChannelGroup(&chGroup);
			if (chGroup)
			{
				chGroup->removeDSP(attachedDSP->dsp);
				int chCount;
				if (chGroup->getNumChannels(&chCount) == 1)
				{
					FMOD::ChannelGroup* parentGroup;
					chGroup->getParentGroup(&parentGroup);
					if (parentGroup)
						chGroup->release();
				}
			}

			delete attachedDSP;
			delete channel;
		}


		RESULT Play(const System& system, const Sound& sound, bool startPaused = false);
		RESULT IsPlaying(bool& outIsPlaying);
		RESULT Pause(bool pause);
		RESULT IsPaused(bool& outIsPaused);
		RESULT Stop();
		RESULT HasEnded(bool& outIsEnded);

		RESULT SetVolume(float volume);
		RESULT GetVolume(float& outVolume);
		RESULT SetVolumeMode(VOLUME_MODE mode);
		RESULT GetVolumeMode(VOLUME_MODE& outMode);

		RESULT SetPan(float amount, PAN_DIRECTION direction);
		RESULT GetPan(float& outAmount, PAN_DIRECTION& outDirection);

		RESULT SetPositionInMs(unsigned int Pos);
		RESULT GetPositionInMs(unsigned int& outPos);
		RESULT GetPositionInPCM(unsigned int& outPos);
		RESULT GetCurrentSound(Sound& outCurrentSound);

		CustomDSP* GetDsp();
		void UpdateDSP();
	private:
		FMOD::Channel* channel = nullptr;
		bool ended;
		float panAmount;
		PAN_DIRECTION panDirection;

		CustomDSP* attachedDSP = nullptr;

		void AttachDSP(CustomDSP* dsp);

		static FMOD_RESULT ChannelCallback(FMOD_CHANNELCONTROL* channelcontrol,
			FMOD_CHANNELCONTROL_TYPE controltype,
			FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
			void* commanddata1,
			void* commanddata2);

		friend class System;
	};
}
