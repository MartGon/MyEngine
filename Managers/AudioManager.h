#pragma once

#include "Manager.h"
#include "AudioPlayer.h"

namespace AudioManagerNs
{
	void audio_callback(void *userdata, Uint8 *stream, int len);
}

class AudioManager : public Manager<AudioPlayer*>
{
public:
	// Constructors
	AudioManager();

	// Member
	int device = -1;

	// Own methods
	void init();
	void enable();
	void silence();

	SDL_AudioFormat getFormat();

	// Overrided methods
	void manage() override;
	void onAddComponent(AudioPlayer* aPlayer) override;
	void beforeRemoveComponent(AudioPlayer* aPlayer) override;
	void onRemoveComponent(AudioPlayer* aPlayer) override;
	void destroy() override;

	// Concurrency lock
	SDL_mutex* mutex = nullptr;

	static bool sound_enabled;
	static Uint8 volume;

private:
	// Audio Hardware specs
	static SDL_AudioSpec* desired_audio_spec;
	static SDL_AudioSpec* obtained_audio_spec;

	// Init Flag
	static bool isInitialized;
};