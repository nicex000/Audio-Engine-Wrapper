
#include "CustomDSP.h"

#include <cstdio>
#include <cstdlib>

#include "ErrorHandler.h"
#include "System.h"
#pragma warning(disable : 4996)
namespace AudioEngine
{
    FMOD_RESULT F_CALLBACK CustomDSP::DSPCallback(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels)
    {
        CustomDspData* data = (CustomDspData*)dsp_state->plugindata;

        /*
            This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0'
            as the number of channels in FMOD_DSP_DESCRIPTION.
            Specifying an actual channel count will mean you have to take care of any number of channels coming in,
            but outputting the number of channels specified. Generally it is best to keep the channel
            count at 0 for maximum compatibility.
        */
        for (unsigned int sample = 0; sample < length; sample++)
        {
            for (int chan = 0; chan < *outchannels; chan++)
            {
                data->buffer[(sample * *outchannels) + chan] = outbuffer[(sample * inchannels) + chan] = inbuffer[(sample * inchannels) + chan];
            }
        }

        data->channels = inchannels;

        return FMOD_OK;
    }

    /*
        Callback called when DSP is created.   This implementation creates a structure which is attached to the dsp state's 'plugindata' member.
    */
    FMOD_RESULT F_CALLBACK CustomDSP::DSPCreateCallback(FMOD_DSP_STATE* dsp_state)
    {
        unsigned int blocksize;
        FMOD_RESULT result;

        result = dsp_state->functions->getblocksize(dsp_state, &blocksize);

        CustomDspData* data = (CustomDspData*)calloc(sizeof(CustomDspData), 1);
        if (!data)
        {
            return FMOD_ERR_MEMORY;
        }
        dsp_state->plugindata = data;
        data->length_samples = blocksize;

        data->buffer = (float*)malloc(blocksize * 8 * sizeof(float));      // *8 = maximum size allowing room for 7.1.   Could ask dsp_state->functions->getspeakermode for the right speakermode to get real speaker count.
        if (!data->buffer)
        {
            return FMOD_ERR_MEMORY;
        }

        return FMOD_OK;
    }

    /*
        Callback called when DSP is destroyed.   The memory allocated in the create callback can be freed here.
    */
    FMOD_RESULT F_CALLBACK CustomDSP::DSPReleaseCallback(FMOD_DSP_STATE* dsp_state)
    {
        if (dsp_state->plugindata)
        {
            CustomDspData* data = (CustomDspData*)dsp_state->plugindata;

            if (data->buffer)
            {
                free(data->buffer);
            }

            free(data);
        }

        return FMOD_OK;
    }

    /*
        Callback called when DSP::getParameterData is called.   This returns a pointer to the raw floating point PCM data.
        We have set up 'parameter 0' to be the data parameter, so it checks to make sure the passed in index is 0, and nothing else.
    */
    FMOD_RESULT F_CALLBACK CustomDSP::DSPGetParameterDataCallback(FMOD_DSP_STATE* dsp_state, int index, void** data, unsigned int* length, char*)
    {
        if (index == 0)
        {
            unsigned int blocksize;
            FMOD_RESULT result;
            CustomDspData* mydata = (CustomDspData*)dsp_state->plugindata;

            result = dsp_state->functions->getblocksize(dsp_state, &blocksize);

            *data = (void*)mydata;
            *length = blocksize * 2 * sizeof(float);

            return FMOD_OK;
        }

        return FMOD_ERR_INVALID_PARAM;
    }
    

    void CustomDSP::Update()
    {
        CustomDspData* data;
        FMOD_RESULT result = dsp->getParameterData(0, (void**)&data, 0, 0, 0);
        ERRCHECK(result);


        if (data->channels)
        {
            int channel;
            float leftLevel, rightLevel;
            for (channel = 0; channel < data->channels; channel++)
            {
                if (channel >= 2) break;
                int count;
            	float max = 0;

                for (count = 0; count < data->length_samples; count++)
                {
                    if (fabs(data->buffer[(count * data->channels) + channel]) > max)
                    {
                        max = fabs(data->buffer[(count * data->channels) + channel]);
                    }
                }
                if (channel == 0) leftLevel = max * 40.f;
                else if (channel == 1) rightLevel = max * 40.f;
            }

            while (visualization.size() >= visSize) visualization.pop_front();

            visualization.emplace_back(leftLevel, rightLevel);
        }
    }
    

    RESULT CustomDSP::CreateDSPDesc(FMOD::System* system)
    {
        FMOD_DSP_DESCRIPTION dspDesc;
        memset(&dspDesc, 0, sizeof(FMOD_DSP_DESCRIPTION));
        FMOD_DSP_PARAMETER_DESC wavedata_desc;
        FMOD_DSP_PARAMETER_DESC* paramdesc[1] =
        {
            &wavedata_desc,
        };

        FMOD_DSP_INIT_PARAMDESC_DATA(wavedata_desc, "wave data", "", "wave data", FMOD_DSP_PARAMETER_DATA_TYPE_USER);

        strncpy(dspDesc.name, "DSP unit", sizeof(dspDesc.name));
        dspDesc.version = 0x00010000;
        dspDesc.numinputbuffers = 1;
        dspDesc.numoutputbuffers = 1;
        dspDesc.read = DSPCallback;
        dspDesc.create = DSPCreateCallback;
        dspDesc.release = DSPReleaseCallback;
        dspDesc.getparameterdata = DSPGetParameterDataCallback;
        dspDesc.numparameters = 1;
        dspDesc.paramdesc = paramdesc;


        return ERRCHECK(system->createDSP(&dspDesc, &dsp));;
    }

    const std::list<std::pair<float, float>>& CustomDSP::GetVisualizationData()
    {
        return visualization;
    }
}
