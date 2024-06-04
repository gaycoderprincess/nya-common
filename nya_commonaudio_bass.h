#include <cstdint>

#include "bass.h"

namespace NyaAudio {
	typedef HSTREAM NyaSound;

	bool Init(HWND hwnd);
	NyaSound LoadFile(const char* path);
	void Delete(NyaSound* sound);
	void Play(NyaSound sound);
	void Stop(NyaSound sound);
	uint64_t GetLength(NyaSound sound);
	uint64_t GetPosition(NyaSound sound);
	void SkipTo(NyaSound sound, uint64_t time);
	void SetVolume(NyaSound sound, float volume);
	bool IsFinishedPlaying(NyaSound sound);
}