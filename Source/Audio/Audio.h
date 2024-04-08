#pragma once

#include <xaudio2.h>
#include "Audio/AudioSource.h"
#include <cassert>

// オーディオ
class Audio
{
public:
	Audio();
	~Audio();

public:

	static void Initalize()
	{
		assert(instance == nullptr);
		instance = std::make_unique<Audio>();
	}

	static void Finalize()
	{
		assert(instance != nullptr);
		instance.reset();
	}

	// インスタンス取得
	static Audio& Instance() { 
		assert(instance != nullptr);
		return *instance.get(); 
	}

	// オーディオソース読み込み
	std::unique_ptr<AudioSource> LoadAudioSource(const char* filename);

private:
	static inline std::unique_ptr<Audio> instance;

	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;
};
