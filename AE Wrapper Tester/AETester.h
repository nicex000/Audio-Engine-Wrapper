#pragma once
#include <vector>

#include "System.h"

using namespace AudioEngine;
class AETester
{
	System system;
	std::vector<const char*> audioList = {
		"../media/Iona_GG.wav",
		"../media/OST.m4a",
		"../media/OST_converted_to_VBR.mp3",
		"../media/music1.mp3",
		"../media/music2.mp3",
		"---- FMOD SOUNDS ----",
		"../media/c.ogg",
		"../media/d.ogg",
		"../media/e.ogg",
		"../media/drumloop.wav",
		"../media/jaguar.wav",
		"../media/singing.wav",
		"../media/standrews.wav",
		"../media/stereo.ogg",
		"../media/swish.wav",
		"../media/wave.mp3"
	};

public:
	std::vector<Channel*> channels;

	void Init();

	void Update();

	void AddChannel();

	RESULT PlaySong(const char* path, bool loop, bool stream, Channel* channel);

};

