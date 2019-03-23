#pragma once
#include "Component.h"

#include <vector>
#include <string>
#include <iostream>

#include <SDL_audio.h>

struct AudioData
{
	// Debug
	std::string name;

	// SDL data
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

		// Audio Data
	AudioData* loadAudioFile(std::string path);
	int addAudioToList(AudioData* audio_data);
	void setAudioToPlay(int index, bool loop = false);
		
		// Player controls
	void play();
	void pause();
	void stop();
	void reset();
	bool isPaused();

	// Members

	// Play state
	Uint8* wav_buffer;
	Uint32 wav_length;

	// Audio data
	AudioData* current_audio_data = nullptr;

	// AudioPlay Params
	bool loop = false;
	int volume = SDL_MIX_MAXVOLUME;

	// Audio List
	std::vector<AudioData*> audio_list;

private:
	const std::string resource_path = "resources/audio/";

	// Atributes
	bool paused = true;

	// Methods
	void resetAudio();
	bool isAudioIndexValid(int index);
};