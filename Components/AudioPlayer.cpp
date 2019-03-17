#include "AudioPlayer.h"

// AudioPlayerNs
void AudioPlayerNs::audio_callback(void* userdata, Uint8* stream, int len)
{
	AudioPlayer* audioPlayer = static_cast<AudioPlayer*>(userdata);

	if (!audioPlayer)
	{
		std::cout << "audio_callback: AudioPlayer could not be created from userdata static_cast" << std::endl;
		return;
	}

	Uint8* audio_pos = audioPlayer->wav_buffer;
	Uint32 audio_len = audioPlayer->wav_length;
	
	if (audio_len == 0)
	{
		SDL_PauseAudio(1);
		return;
	}

	len = (len > audio_len ? audio_len : len);
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_memset(stream, 0, len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
	//SDL_MixAudioFormat(dst, src, format, len, volume);


	std::cout << "Memory address is " << (Uint32)audio_pos << " Next address should be " << Uint32(audio_pos + len) <<std::endl;

	audioPlayer->wav_buffer += len;
	audioPlayer->wav_length -= len;
}

// Attributes
bool AudioPlayer::isInitialized = false;

// Constructor
AudioPlayer::AudioPlayer()
{
	//if (!isInitialized)
		//init();

	audio_spec = new SDL_AudioSpec();
}

AudioPlayer::AudioPlayer(std::string audio_file) : AudioPlayer()
{
	std::string rel_path = resource_path + audio_file;
	loadAudioFile(rel_path);
	init();
}

// Own methods
void AudioPlayer::play() 
{
	SDL_PauseAudio(0);
}

// Overrided Methods

void AudioPlayer::update() 
{

}

// Private methods

void AudioPlayer::init()
{
	// Creating desired spec
	SDL_AudioSpec* wav_spec = this->audio_spec;
	/*wav_spec->freq = 48000;
	wav_spec->channels = 2;
	wav_spec->samples = 4096;*/
	wav_spec->callback = AudioPlayerNs::audio_callback;
	wav_spec->userdata = this;

	if (SDL_OpenAudio(wav_spec, NULL) < 0) {
		std::cout << "SDL_OpenAudio: Could not open audio" << std::endl;
		exit(-1);
	}

	isInitialized = true;
}

void AudioPlayer::loadAudioFile(std::string path)
{
	// Load the audio file
	if (!SDL_LoadWAV(path.c_str(), audio_spec, &wav_buffer, &wav_length))
	{
		std::cout << "SDL_LoadWAV: Could not load the audio file " << path << std::string(SDL_GetError()) <<  std::endl;
		return;
	}
	
	// Fill Spec
	/*
	audio_spec->callback = AudioPlayerNs::audio_callback;
	audio_spec->userdata = this;
	*/
}


