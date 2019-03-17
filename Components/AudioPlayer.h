#pragma once
#include "Component.h"

#include <vector>
#include <string>
#include <iostream>

#include <SDL_audio.h>

namespace AudioPlayerNs 
{
	void audio_callback(void *userdata, Uint8 *stream, int len);
}

struct AudioData
{
	SDL_AudioSpec audio_spec;
	Uint32 wav_length = 0;
	Uint8* wav_buffer = new Uint8;
};

class AudioPlayer : public Component
{
public:
	// Constructor
	AudioPlayer();
	AudioPlayer(std::string audio_file);

	// Overrided methods
	void update() override;

	// Own methods
	AudioData* loadAudioFile(std::string path);
	int addAudioToList(AudioData* audio_data);
	void setAudioToPlay(AudioData* audio_data, int index = -1);
	void play(int index = -1);
	void pause();
	void stop();
	bool isPaused();

	// Members

	// Used during callbacks
	Uint32 wav_length = 0;
	Uint8* wav_buffer = new Uint8;

	// Audio data
	AudioData* current_audio_data = nullptr;

	std::vector<AudioData*> audio_list;

private:
	static SDL_AudioSpec* desired_audio_spec;
	static SDL_AudioSpec* obtained_audio_spec;

	static bool isInitialized;
	const std::string resource_path = "resources/audio/";

	// Atributes
	bool paused = false;

	// Methods
	void resetAudio();
	void init();
};