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

	if (!audioManager->sound_enabled)
		return;

	// Lock
	SDL_LockMutex(audioManager->mutex);

	std::vector<AudioPlayer*> audioPlayers = audioManager->getComponents();
	for (auto audioPlayer : audioPlayers)
	{
		if (!audioPlayer)
			continue;

		// Check if audioPlayer is enabled
		if (!audioPlayer->isEnabled)
			continue;

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

		float mod = (float)audioManager->volume / 100.f;

		len = (len > audio_len ? audio_len : len);
		SDL_MixAudioFormat(stream, audio_pos, audioManager->getFormat(), len, audioPlayer->volume * mod);

		audioPlayer->wav_buffer += len;
		audioPlayer->wav_length -= len;
	}
	
	// Unlock
	SDL_UnlockMutex(audioManager->mutex);
}

// Attributes
SDL_AudioSpec* AudioManager::desired_audio_spec = new SDL_AudioSpec();
SDL_AudioSpec* AudioManager::obtained_audio_spec = new SDL_AudioSpec();

// Settings

bool AudioManager::sound_enabled = true;
Uint8 AudioManager::volume = 100;

bool AudioManager::isInitialized = false;

// Constructors
AudioManager::AudioManager() 
{
	if (!isInitialized)
		init();
	else
	{
		SDL_CloseAudio();
		init();
	}	
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

	device = SDL_OpenAudioDevice(NULL, 0, desired_audio_spec, obtained_audio_spec, 0);

	if (device == 0) {
		std::cout << "SDL_OpenAudio: Could not open audio: " << SDL_GetError() << std::endl;
		exit(-1);
	}

	isInitialized = true;
	SDL_PauseAudioDevice(device, 0);

	// Crete mutex
	mutex = SDL_CreateMutex();
}

void AudioManager::enable()
{
	SDL_PauseAudioDevice(device, 0);
}

void AudioManager::silence() 
{
	SDL_PauseAudioDevice(device, 1);
}

// Overrided methods
void AudioManager::manage() 
{

}

void AudioManager::onAddComponent(AudioPlayer * aPlayer)
{
	//std::cout << "AduioPlayer added" << std::endl;
}

void AudioManager::beforeRemoveComponent(AudioPlayer* aPlayer)
{
	// Lock mutex
	SDL_LockMutex(mutex);
}

void AudioManager::onRemoveComponent(AudioPlayer* aPlayer)
{
	// Unock mutex
	SDL_UnlockMutex(mutex);
}

SDL_AudioFormat AudioManager::getFormat()
{
	return obtained_audio_spec->format;
}

void AudioManager::destroy()
{
	silence();
	if (mutex)
	{
		SDL_DestroyMutex(mutex);
		mutex = nullptr;
	}

	//SDL_CloseAudioDevice(device);
}