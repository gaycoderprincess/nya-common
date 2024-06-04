#include <cstdint>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "nya_commonaudio_ma.h"

namespace NyaAudio {
	bool bInited = false;
	ma_engine* pEngine = nullptr;

	bool Init() {
		if (bInited) return true;

		ma_result result;
		pEngine = new ma_engine;

		result = ma_engine_init(nullptr, pEngine);
		if (result != MA_SUCCESS) {
			delete pEngine;
			return false;
		}
		bInited = true;
		return true;
	}

	NyaSound LoadFile(const char* path) {
		ma_result result;
		auto sound = new ma_sound;

		result = ma_sound_init_from_file(pEngine, path, MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound);
		if (result != MA_SUCCESS) {
			delete sound;
			return nullptr;
		}

		return sound;
	}

	void Delete(NyaSound* sound) {
		if (!sound || !*sound) return;

		ma_sound_uninit(*sound);
		*sound = nullptr;
	}

	void Play(NyaSound sound) {
		ma_sound_start(sound);
	}

	void Stop(NyaSound sound) {
		ma_sound_stop(sound);
	}

	uint64_t GetLength(NyaSound sound) {
		uint64_t ret;
		ma_sound_get_length_in_pcm_frames(sound, &ret);
		return ret;
	}

	uint64_t GetPosition(NyaSound sound) {
		uint64_t ret;
		ma_sound_get_cursor_in_pcm_frames(sound, &ret);
		return ret;
	}

	void SkipTo(NyaSound sound, uint64_t time) {
		ma_sound_seek_to_pcm_frame(sound, time);
	}

	void SetVolume(NyaSound sound, float volume) {
		ma_sound_set_volume(sound, volume);
	}

	bool IsFinishedPlaying(NyaSound sound) {
		return ma_sound_at_end(sound);
	}
}