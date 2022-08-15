#pragma once


#include <list>
#include <utility>
#include "FMOD Core/inc/fmod.hpp"

#include "ErrorHandler.h"

namespace FMOD
{
	class System;
	class DSP;
}

namespace AudioEngine
{
	class CustomDSP
	{

	public:
		typedef struct
		{
			float* buffer;
			int   length_samples;
			int   channels;
		} CustomDspData;

		CustomDSP(int visualizationSize): visSize(visualizationSize){}
		~CustomDSP()
		{
			dsp->release();
		}

		const std::list<std::pair<float, float>>& GetVisualizationData();

	private:

		FMOD::DSP* dsp;

		std::list<std::pair<float, float>> visualization;
		int visSize;

		RESULT CreateDSPDesc(FMOD::System* system);
		void Update();

		static FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels);
		static FMOD_RESULT F_CALLBACK DSPCreateCallback(FMOD_DSP_STATE* dsp_state);
		static FMOD_RESULT F_CALLBACK DSPReleaseCallback(FMOD_DSP_STATE* dsp_state);
		static FMOD_RESULT F_CALLBACK DSPGetParameterDataCallback(FMOD_DSP_STATE* dsp_state, int index, void** data, unsigned int* length, char*);

		friend class System;
		friend class Channel;


	};

	
}
