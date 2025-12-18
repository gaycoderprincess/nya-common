#include "nya_commonaudio.h"

namespace NyaAudio {
	bool bInited = false;

	bool Init(HWND hwnd) {
		if (bInited) return true;

		if (!BASS_Init(-1, 44100, 0, hwnd, nullptr)) {
			return false;
		}
		bInited = true;
		return true;
	}

	NyaSound LoadFile(const std::string& path) {
		if (auto sound = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_PRESCAN)) {
			return sound;
		}
		return 0;
	}

	NyaSound LoadFileStreamed(const std::string& path) {
		if (auto sound = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_ASYNCFILE)) {
			return sound;
		}
		return 0;
	}

	NyaSound LoadMemory(const char* data, size_t size) {
		if (auto sound = BASS_StreamCreateFile(true, data, 0, size, BASS_STREAM_PRESCAN)) {
			return sound;
		}
		return 0;
	}

	void Delete(NyaSound* sound) {
		if (!sound) return;

		BASS_StreamFree(*sound);
		*sound = 0;
	}

	void Play(NyaSound sound) {
		BASS_ChannelPlay(sound, 0);
	}

	void Stop(NyaSound sound) {
		BASS_ChannelStop(sound);
	}

	uint64_t GetLength(NyaSound sound) {
		return BASS_ChannelGetLength(sound, 0);
	}

	uint64_t GetPosition(NyaSound sound) {
		return BASS_ChannelGetPosition(sound, 0);
	}

	void SkipTo(NyaSound sound, uint64_t time, bool useRealTime) {
		if (useRealTime) time = BASS_ChannelSeconds2Bytes(sound, time*0.001);
		BASS_ChannelSetPosition(sound, time, 0);
	}

	void SetVolume(NyaSound sound, float volume) {
		BASS_ChannelSetAttribute(sound, BASS_ATTRIB_VOL, volume);
	}

	bool IsFinishedPlaying(NyaSound sound) {
		return !BASS_ChannelIsActive(sound);
	}
}