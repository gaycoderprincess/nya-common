#include "miniaudio.h"

namespace NyaAudio {
	bool Init();
	ma_sound* LoadFile(const char* path);
	void Delete(ma_sound** sound);
	void Play(ma_sound* sound);
	void Stop(ma_sound* sound);
	uint64_t GetLength(ma_sound* sound);
	uint64_t GetPosition(ma_sound* sound);
	void SkipTo(ma_sound* sound, uint64_t time);
	void SetVolume(ma_sound* sound, float volume);
	bool IsFinishedPlaying(ma_sound* sound);
}