#pragma once
#include <vector>

#include "System.h"

using namespace AudioEngine;
class AETester
{
	System system;
	std::vector<const char*> audioList = {
		"../media/drumloop.wav",
		"../media/drumloop.wav"
	};

public:
	std::vector<Channel*> channels;

	void Init();

	void Update();

	void AddChannel();

	void PlaySong(const char* path, bool loop, bool stream, Channel* channel);

};

