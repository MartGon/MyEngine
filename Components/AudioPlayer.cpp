#include "AudioPlayer.h"

// AudioPlayerNs
// TODO - Es necesario hacer la clase AudioPlayerManager que se use como userdata de este m'etodo
// En ella se pasara por todos los audio players que no esten pausados y se hara un mix con sus audios
// Esto dara como resultado el stream de audio que se va a oir
void AudioPlayerNs::audio_callback(void* userdata, Uint8* stream, int len)
{
	AudioPlayer* audioPlayer = static_cast<AudioPlayer*>(userdata);

	if (!audioPlayer)
	{
		std::cout << "audio_callback: AudioPlayer could not be created from userdata static_cast" << std::endl;
		return;
	}

	// Check if audio exists
	if (!audioPlayer->current_audio_data)
		return;

	// Return if the player is paused
	if (audioPlayer->isPaused())
		return;

	Uint8* audio_pos = audioPlayer->wav_buffer;
	Uint32 audio_len = audioPlayer->wav_length;
	
	if (audio_len == 0)
	{
		audioPlayer->stop();
		return;
	}

	len = (len > audio_len ? audio_len : len);
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_memset(stream, 0, len);
	//SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
	SDL_MixAudioFormat(stream, audio_pos, audioPlayer->current_audio_data->audio_spec.format, len, SDL_MIX_MAXVOLUME);


	//std::cout << "Memory address is " << (Uint32)audio_pos << " Next address should be " << Uint32(audio_pos + len) <<std::endl;

	audioPlayer->wav_buffer += len;
	audioPlayer->wav_length -= len;
}

// Attributes
SDL_AudioSpec* AudioPlayer::desired_audio_spec = new SDL_AudioSpec();
SDL_AudioSpec* AudioPlayer::obtained_audio_spec = new SDL_AudioSpec();

bool AudioPlayer::isInitialized = false;

// Constructor
AudioPlayer::AudioPlayer()
{
	if (!isInitialized)
		init();
}

AudioPlayer::AudioPlayer(std::string audio_file) : AudioPlayer()
{
	AudioData* aData = loadAudioFile(audio_file);
	setAudioToPlay(aData);
}

// Own methods
void AudioPlayer::play(int index) 
{
	// Play given audio
	if (index != -1)
		setAudioToPlay(nullptr, index);
	else
		// Reset
		resetAudio();

	// Activate flag
	paused = false;

	SDL_PauseAudio(0);
}

void AudioPlayer::pause() 
{
	paused = true;
}

void AudioPlayer::stop() 
{
	// Reset position and length
	resetAudio();

	// Stopping audio
	SDL_PauseAudio(1);
}

bool AudioPlayer::isPaused()
{
	return paused;
}

// Overrided Methods

void AudioPlayer::update() 
{

}

// Private methods

void AudioPlayer::resetAudio() 
{
	if (!current_audio_data)
		return;

	wav_buffer = current_audio_data->wav_buffer;
	wav_length = current_audio_data->wav_length;
}

void AudioPlayer::init()
{
	// Creating desired spec
	desired_audio_spec->freq = 44100;
	desired_audio_spec->format = AUDIO_S16LSB;
	desired_audio_spec->channels = 2;
	desired_audio_spec->samples = 4096;
	desired_audio_spec->callback = AudioPlayerNs::audio_callback;
	desired_audio_spec->userdata = this;

	if (SDL_OpenAudio(desired_audio_spec, NULL) < 0) {
		std::cout << "SDL_OpenAudio: Could not open audio" << std::endl;
		exit(-1);
	}

	isInitialized = true;
}

AudioData* AudioPlayer::loadAudioFile(std::string path)
{
	// Set location
	std::string rel_path = resource_path + path;

	AudioData* audio_data = new AudioData();

	// Load the audio file
	if (!SDL_LoadWAV(rel_path.c_str(), &audio_data->audio_spec, &audio_data->wav_buffer, &audio_data->wav_length))
	{
		std::cout << "SDL_LoadWAV: Could not load the audio file " << rel_path << std::string(SDL_GetError()) <<  std::endl;
		return nullptr;
	}

	return audio_data;
}

int AudioPlayer::addAudioToList(AudioData* audio_data)
{
	if (!audio_data)
		return -1;

	audio_list.push_back(audio_data);
	int new_index = audio_list.size() - 1;

	return new_index;
}

void AudioPlayer::setAudioToPlay(AudioData* audio_data, int index) 
{
	if (!audio_data)
	{
		if (index >= audio_list.size())
			return;

		current_audio_data = audio_list.at(index);
	}
	else
		current_audio_data = audio_data;

	resetAudio();
}


