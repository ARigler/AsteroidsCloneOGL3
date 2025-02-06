#include"../include/AudioManager.h"
#include"../include/Utilities.h"
#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<SDL_ttf.h>
#include<SDL_mixer.h>

void AudioManager::setClipData(int index, AudioClipMetadata metaData) {
	audioClips[index].metaData = metaData;
}

AudioClipMetadata AudioManager::fetchClipMetadata(int index) {
	if (index < audioClips.size() && index >= 0)
	{
		return audioClips[index].metaData;
	}
}

bool AudioManager::loadClipFromFile(std::string path) {
	bool success = true;
	int newIndex = audioClips.size();
	AudioClipMetadata newClipMetadata;
	newClipMetadata.audioIndex = newIndex;
	AudioClipData newClipData;
	newClipData.audioClip=Mix_LoadWAV(path.c_str());
	if (newClipData.audioClip == NULL) {
		success = false;
	}
	newClipData.metaData = newClipMetadata;
	audioClips.push_back(newClipData);
	return success;
}

void AudioManager::removeClipIndex(int index) {
	if (index < audioClips.size() && index >= 0)
	{
		Mix_FreeChunk(audioClips[index].audioClip);
		audioClips.erase(audioClips.begin() + index);
	}
}

bool AudioManager::playSound(int index, int channel, int loops) {
	bool success = false;
	if (audioClips.size() && index >= 0) {
		Mix_PlayChannel(channel, audioClips[index].audioClip, loops);
		success = true;
	}
	return success;
}

AudioManager::~AudioManager() {
	for (int i = audioClips.size() - 1; i >= 0; i--) {
		removeClipIndex(i);
	}
}