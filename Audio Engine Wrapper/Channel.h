#pragma once
#include "FMOD Core/inc/fmod.hpp"

#include "ErrorHandler.h"
#include "System.h"
#include "CustomDSP.h"
#include "Sound.h"

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

			if (ended) return;
			Sound* sound = new Sound();
			GetCurrentSound(*sound);
			delete sound;
		}


		RESULT Play(const System& system, const Sound& sound, bool startPaused = false);
		RESULT IsPlaying(bool& outIsPlaying) const;
		RESULT Pause(bool pause) const;
		RESULT IsPaused(bool& outIsPaused) const;
		RESULT Stop() const;
		RESULT HasEnded(bool& outIsEnded) const;

		RESULT SetVolume(float volume) const;
		RESULT GetVolume(float& outVolume) const;
		RESULT SetVolumeMode(VOLUME_MODE mode) const;
		RESULT GetVolumeMode(VOLUME_MODE& outMode) const;

		RESULT SetPan(float amount, PAN_DIRECTION direction);
		RESULT GetPan(float& outAmount, PAN_DIRECTION& outDirection) const;

		RESULT SetPositionInMs(unsigned int Pos) const;
		RESULT GetPositionInMs(unsigned int& outPos) const;
		RESULT SetPositionInPCM(unsigned int Pos) const;
		RESULT GetPositionInPCM(unsigned int& outPos) const;
		RESULT GetCurrentSound(Sound& outCurrentSound) const;

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
