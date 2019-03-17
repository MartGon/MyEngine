#pragma once
#include "Component.h"

#include <string>
#include <iostream>

#include <SDL_audio.h>

namespace AudioPlayerNs 
{
	void audio_callback(void *userdata, Uint8 *stream, int len);
}

class AudioPlayer : public Component
{
public:
	// Constructor
	AudioPlayer();
	AudioPlayer(std::string audio_file);

	// Overrided methods
	void update() override;

	// Own methods
	void play();

	// Members
	SDL_AudioSpec* audio_spec = nullptr;
	Uint32 wav_length = 0;
	Uint8* wav_buffer = new Uint8;

private:
	static bool isInitialized;
	const std::string resource_path = "resources/audio/";

	// Methods
	void init();
	void loadAudioFile(std::string path);
};