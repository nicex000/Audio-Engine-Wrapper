#include "AETester.h"

#include "imgui.h"
#include "implot.h"

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }
};




void AETester::Init()
{
    system.InitSystem(32);
   
}

void AETester::Update()
{
    system.Update();

    for (auto channel : channels)
    {
        channel->UpdateDSP();
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin("Audio Engine Wrapper Test", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);


    bool pressed = ImGui::Button("Add Channel");
    ImGui::SameLine();
    ImGui::Text("Active Channels:");
    ImGui::SameLine();
    int channelsPlaying = 0;
    system.GetPlayingChannelCount(&channelsPlaying);
    ImGui::Text(std::to_string(channelsPlaying).c_str());
    if (pressed)
    {
        AddChannel();
    }
    if (ImGui::BeginTabBar("Channels", ImGuiTabBarFlags_FittingPolicyScroll))
    {
        std::vector<bool*> openedTabs;
        for (int i = 0; i < channels.size(); ++i)
        {
            openedTabs.push_back(new bool(true));
            if (ImGui::BeginTabItem((std::to_string(i)+ "  ").c_str(), openedTabs.back(), ImGuiTabItemFlags_None))
            {
	            unsigned int currentPos = 0;
	            unsigned int trackLength = 0;
	            bool         playing = 0;
	            bool         paused = 0;
	            Sound* currentSound = new Sound;

                bool hasEnded;
                channels[i]->HasEnded(hasEnded);
                if (!hasEnded) {
                    channels[i]->IsPlaying(playing);
                    channels[i]->IsPaused(paused);
                    channels[i]->GetPositionInMs(currentPos);

                    channels[i]->GetCurrentSound(*currentSound);

                    currentSound->GetLengthInMs(&trackLength);
                }

                ImGui::PushID(0);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                static int audioIndex = 0;
                const char* combo_preview_value = audioList[audioIndex];
                if (ImGui::BeginCombo("Select Track", combo_preview_value))
                {
                    for (int n = 0; n < audioList.size(); n++)
                    {
                        const bool is_selected = (audioIndex == n);
                        if (ImGui::Selectable(audioList[n], is_selected))
                            audioIndex = n;

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                static bool loop, stream = false;
                ImGui::SameLine();
                ImGui::Checkbox("Loop", &loop);
                ImGui::SameLine();
                ImGui::Checkbox("Stream", &stream);
                ImGui::SameLine();
                const bool playAudioFromCombo = ImGui::Button("Play!");
                if (playAudioFromCombo) 
                    PlaySong(audioList[audioIndex], loop, stream, channels[i]);
                ImGui::PopID();



                ImGui::PushID(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                static bool loop2, stream2 = false;
                static char* path = new char[2000]{ 0 };
                ImGui::InputTextWithHint("Custom Track", "File Path", path, 2000);
                ImGui::SameLine();
                ImGui::Checkbox("Loop", &loop2);
                ImGui::SameLine();
                ImGui::Checkbox("Stream", &stream2);
                ImGui::SameLine();
                const bool playAudioFromTextBox = ImGui::Button("Play!");
                if (playAudioFromTextBox)
                    PlaySong(path, loop2, stream2, channels[i]);
                ImGui::PopID();



                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::NewLine();






	            
                if (!hasEnded)
                {
                    static const int size = 1000;

                    unsigned int pcmPos = 0;
                    channels[i]->GetPositionInPCM(pcmPos);
                    auto vis = channels[i]->GetDsp()->GetVisualizationData();
                    if (vis.size() > 0)
                    {
                        ScrollingBuffer leftCh, rightCh;
                        auto visIt = vis.begin();
                        for (int j = 0; j < size && j < vis.size(); ++j)
                        {
                            leftCh.AddPoint(j, visIt->first);
                            rightCh.AddPoint(j, -visIt->second);
                            ++visIt;
                        }

                        if (ImPlot::BeginPlot("WaveForm", ImVec2(ImGui::GetContentRegionAvail().x, 180.0f),
                            ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame | ImPlotFlags_NoLegend))
                        {
                            ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoTickLabels);
                            ImPlot::SetupAxisLimits(ImAxis_X1, 0, size, ImGuiCond_Always);
                            ImPlot::SetupAxisLimits(ImAxis_Y1, -30, 30);
                            ImPlot::PlotLine("Left Ch", &leftCh.Data[0].x, &leftCh.Data[0].y, leftCh.Data.size(), 
                                0, leftCh.Offset, 2 * sizeof(float));
                            ImPlot::PlotLine("Right Ch", &rightCh.Data[0].x, &rightCh.Data[0].y, rightCh.Data.size(), 
                                0, rightCh.Offset, 2 * sizeof(float));
                            ImPlot::EndPlot();
                        }
                    }
#pragma region time_text

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                    ImGui::ProgressBar(static_cast<float>(currentPos) / static_cast<float>(trackLength),
                        ImVec2(0, 20));
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

		            ImGui::Text("Time %02d:%02d:%02d/%02d:%02d:%02d\n",
	                    currentPos / 1000 / 60,
	                    currentPos / 1000 % 60,
	                    currentPos / 10 % 100,
	                    trackLength / 1000 / 60,
	                    trackLength / 1000 % 60,
	                    trackLength / 10 % 100);
#pragma endregion time_text

               
                    bool pauseBtn, stopBtn;

                    ImGui::PushID(2);
                    float hue = paused ? 0.3f : 0.15f;
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                    pauseBtn = ImGui::Button(paused ? "Resume" : "Pause");
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    ImGui::PushID(3);
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
                    stopBtn = ImGui::Button("Stop");
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                	if (pauseBtn) channels[i]->Pause(!paused);
                    if (stopBtn) channels[i]->Stop();
                    ImGui::SameLine();
                    ImGui::Text("Status: ");
                    ImVec4 statusColor;
                    statusColor.x = playing ? 0 : 1;
                    statusColor.y = playing ? 1 : 0;
                    statusColor.z = 0;
                    statusColor.w = 1;
                    ImGui::SameLine();
                    ImGui::TextColored(statusColor, "%s", hasEnded ? "Ended" :
                        paused ? "Paused " :
                        playing ? "Playing" :
                        "Stopped");
                    ImGui::NewLine();

                    int pos = static_cast<int>(currentPos);
                    ImGui::PushID(5);
                    ImGui::SliderInt("Position", &pos, 0, static_cast<int>(trackLength), "%dms");
                    ImGui::PopID();
                    if (pos != static_cast<int>(currentPos))
                        channels[i]->SetPositionInMs(pos);

                    float volume;
                    channels[i]->GetVolume(volume);
                    ImGui::PushID(5);
                    hue = lerp(0.3f, 0.f, volume > 1.f ? 1.f : volume);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(hue, 0.5f, 0.5f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(hue, 0.6f, 0.5f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(hue, 0.7f, 0.5f));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(hue, 0.9f, 0.9f));
                    ImGui::SliderFloat("Volume", &volume, 0.f, 2.f);
                    ImGui::PopStyleColor(4);
                    ImGui::PopID();
                	channels[i]->SetVolume(volume);




                    float panVal;
                    Channel::PAN_DIRECTION panDir;

                    channels[i]->GetPan(panVal, panDir);
                    if (panDir == Channel::PAN_DIRECTION::LEFT) panVal *= -1.f;
                    ImGui::SliderFloat("Pan", &panVal, -1.f, 1.f);
                    if (panVal < 0)
                    {
                        panVal *= -1.f;
                        panDir = Channel::PAN_DIRECTION::LEFT;
                    }
                    else
                        panDir = Channel::PAN_DIRECTION::RIGHT;

                    channels[i]->SetPan(panVal, panDir);


                }





	            ImGui::EndTabItem();
            }
        }
        for (int i = 0; i < openedTabs.size(); ++i)
        {
            if (!*openedTabs[i])
            {
                delete channels[i];
                channels.erase(channels.begin() + i);
            }
            delete openedTabs[i];
        }
        ImGui::EndTabBar();

    }

    ImGui::End();
}

void AETester::AddChannel()
{
    channels.push_back(new Channel);
}

void AETester::PlaySong(const char* path, bool loop, bool stream, Channel* channel)
{
    Sound* mySound = new Sound(system, path,
        stream ? Sound::MODE::STREAM : Sound::MODE::STATIC,
        loop ? Sound::LOOP_MODE::NORMAL : Sound::LOOP_MODE::OFF);


    CustomDSP* dsp = new CustomDSP(1000);
    system.PlaySound(*mySound, *channel);

    system.LoadDSP(*channel, *dsp);

}
