#pragma once
#pragma once
#ifndef RSOS_AUDIOMANAGER
#define RSOS_AUDIOMANAGER
#include<vector>
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<string>
#include<unordered_map>
#include"Structures.h"

struct AudioClipMetadata {
	int audioIndex;
};

struct AudioClipData {
	Mix_Chunk* audioClip;
	AudioClipMetadata metaData;
};

class AudioManager {
public:
	static AudioManager* getInstance() {
		if (instance == nullptr) {
			instance = new AudioManager();
		}
		return instance;
	}
	AudioManager(const AudioManager&) = delete;
	AudioManager operator=(const AudioManager&) = delete;
	//Renders Audio at given point
	bool loadClipFromFile(std::string path);
	void removeClipIndex(int index);
	bool playSound(int index, int channel = -1, int loops = 0);
	int fetchAudioClipListLength()const { return audioClips.size(); }
	AudioClipMetadata fetchClipMetadata(int index);
	void setClipData(int index, AudioClipMetadata metadata);

	~AudioManager();
private:
	std::vector<AudioClipData> audioClips;
	AudioManager() {
		audioClips = std::vector<AudioClipData>();	
		
	}
	inline static AudioManager* instance = nullptr;
};

#endif