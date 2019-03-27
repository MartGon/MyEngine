#include "AudioManager.h"
#include "GameObject.h"

// AudioManagerNS
void AudioManagerNs::audio_callback(void* userdata, Uint8* stream, int len)
{
	AudioManager* audioManager = static_cast<AudioManager*>(userdata);

	if (!audioManager)
	{
		std::cout << "audio_callback: AudioPlayer could not be created from userdata static_cast" << std::endl;
		return;
	}

	// Set to silence first, removing previous data
	SDL_memset(stream, 0, len);

	std::vector<AudioPlayer*> audioPlayers = audioManager->getComponents();
	for (auto audioPlayer : audioPlayers)
	{
		// Check if gameObject exists
		if (!audioPlayer->gameObject)
			continue;

		// Check if gameObject is active
		if (!audioPlayer->gameObject->shouldBeLoaded())
			continue;

		// Check if audio exists
		if (!audioPlayer->current_audio_data)
			continue;

		// Return if the audio player is paused
		if (audioPlayer->isPaused())
			continue;

		Uint8* audio_pos = audioPlayer->wav_buffer;
		Uint32 audio_len = audioPlayer->wav_length;

		if (audio_len == 0)
		{
			if (audioPlayer->loop)
				audioPlayer->reset();
			else
				audioPlayer->stop();

			continue;
		}

		len = (len > audio_len ? audio_len : len);
		//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
		//SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
		SDL_MixAudioFormat(stream, audio_pos, audioPlayer->current_audio_data->audio_spec.format, len, audioPlayer->volume);

		//std::cout << "Memory address is " << (Uint32)audio_pos << " Next address should be " << Uint32(audio_pos + len) <<std::endl;

		audioPlayer->wav_buffer += len;
		audioPlayer->wav_length -= len;
	}
	
}

// Attributes
SDL_AudioSpec* AudioManager::desired_audio_spec = new SDL_AudioSpec();
SDL_AudioSpec* AudioManager::obtained_audio_spec = new SDL_AudioSpec();

bool AudioManager::isInitialized = false;

// Constructors
AudioManager::AudioManager() 
{
	if (!isInitialized)
		init();
}

// Own methods
void AudioManager::init() 
{
	// Creating desired spec
	desired_audio_spec->freq = 44100;
	desired_audio_spec->format = AUDIO_S16LSB;
	desired_audio_spec->channels = 2;
	desired_audio_spec->samples = 4096;
	desired_audio_spec->callback = AudioManagerNs::audio_callback;
	desired_audio_spec->userdata = this;

	if (SDL_OpenAudio(desired_audio_spec, NULL) < 0) {
		std::cout << "SDL_OpenAudio: Could not open audio" << std::endl;
		exit(-1);
	}

	isInitialized = true;
}

void AudioManager::enable()
{
	SDL_PauseAudio(0);
}

void AudioManager::silence() 
{
	SDL_PauseAudio(1);
}

// Overrided methods
void AudioManager::manage() 
{

}

void AudioManager::onAddComponent(AudioPlayer * aPlayer)
{
	//std::cout << "AduioPlayer added" << std::endl;
}