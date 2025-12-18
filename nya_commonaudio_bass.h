#include <cstdint>
#include <string>

#include "bass.h"

namespace NyaAudio {
	typedef HSTREAM NyaSound;

	bool Init(HWND hwnd);
	NyaSound LoadFile(const std::string& path);
	NyaSound LoadFileStreamed(const std::string& path);
	NyaSound LoadMemory(const char* data, size_t size);
	void Delete(NyaSound* sound);
	void Play(NyaSound sound);
	void Stop(NyaSound sound);
	uint64_t GetLength(NyaSound sound);
	uint64_t GetPosition(NyaSound sound);
	void SkipTo(NyaSound sound, uint64_t time, bool useRealTime = false);
	void SetVolume(NyaSound sound, float volume);
	bool IsFinishedPlaying(NyaSound sound);
}