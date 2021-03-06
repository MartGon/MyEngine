#include "AudioPlayer.h"
#include "SceneManager.h"

// AudioPlayerNs
// TODO - Es necesario hacer la clase AudioPlayerManager que se use como userdata de este m'etodo
// En ella se pasara por todos los audio players que no esten pausados y se hara un mix con sus audios
// Esto dara como resultado el stream de audio que se va a oir

std::unordered_map<std::string, AudioData*> AudioPlayer::audios = std::unordered_map<std::string, AudioData*>();

// Constructor
AudioPlayer::AudioPlayer()
{
	// Add to audio manager
	SceneManager::scene->addComponentToManager(this);
}

AudioPlayer::AudioPlayer(std::string audio_file) : AudioPlayer()
{
	AudioData* aData = loadAudioFile(audio_file);
	int index = addAudioToList(aData);
	setAudioToPlay(index);
}

AudioPlayer::~AudioPlayer()
{
}

// Overrided methods
void AudioPlayer::destroy()
{
	SceneManager::scene->removeComponentFromManager(this);
	/*
	for(auto audio_data : audio_list)
		if (audio_data)
		{
			SDL_FreeWAV(audio_data->wav_buffer);
			delete audio_data;
		}
		*/
}

// Own methods
void AudioPlayer::play() 
{
	// If it is not paused, do nothing
	if (!paused)
		return;

	// Activate flag
	paused = false;
}

void AudioPlayer::pause() 
{
	paused = true;
}

void AudioPlayer::stop() 
{
	// Reset position and length
	resetAudio();

	// Pause audio
	pause();
}

void AudioPlayer::reset()
{
	resetAudio();
}

bool AudioPlayer::isPaused()
{
	return paused;
}

// TODO - Check if file has already been loaded
AudioData* AudioPlayer::loadAudioFile(std::string path)
{
	AudioData* audio_data = nullptr;

	// Set location
	std::string rel_path = resource_path + path;

	auto audio = audios.find(rel_path);
	if (audio == audios.end())
	{
		audio_data = new AudioData();
		audio_data->name = rel_path;

		// Load the audio file
		if (!SDL_LoadWAV(rel_path.c_str(), &audio_data->audio_spec, &audio_data->wav_buffer, &audio_data->wav_length))
		{
			std::cout << "SDL_LoadWAV: Could not load the audio file " << rel_path << std::string(SDL_GetError()) << std::endl;

			// Delete allocated memory
			delete audio_data;
			audio_data = nullptr;
		}
		else
		{
			// Add to audios list
			audios.insert({ rel_path, audio_data });
		}
	}
	else
		audio_data = audio->second;

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

void AudioPlayer::setAudioToPlay(int index, bool loop)
{
	if (!isAudioIndexValid(index))
		return;

	// Set new audio
	current_audio_data = audio_list.at(index);

	// Reset positon vars
	resetAudio();

	// Unset loop
	this->loop = loop;
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

bool AudioPlayer::isAudioIndexValid(int index) 
{
	return !(index >= audio_list.size() || index < 0);
}


