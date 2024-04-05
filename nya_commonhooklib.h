#ifndef NYA_COMMON_HOOKING
#define NYA_COMMON_HOOKING
#include <cstdint>

namespace NyaHookLib {
	template<typename T>
	void Patch(uintptr_t address, T value) {
		DWORD backup, backup2;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &backup);
		memcpy((void*)address, (void*)&value, sizeof(value));
		VirtualProtect((void*)address, sizeof(value), backup, &backup2);
	}

	void Fill(uintptr_t address, uint8_t value, unsigned int count) {
		DWORD backup, backup2;
		VirtualProtect((void*)address, count, PAGE_EXECUTE_READWRITE, &backup);
		memset((void*)address, value, count);
		VirtualProtect((void*)address, count, backup, &backup2);
	}

	inline intptr_t MakeRelative(uintptr_t src, uintptr_t dest) {
		return dest - (src + 4);
	}

	inline intptr_t ReadRelative(uintptr_t src) {
		return src + *(int32_t*)src + 4;
	}

	enum eOffsetInstruction {
		CALL,
		JMP,
	};

	template<typename T>
	uintptr_t PatchRelative(eOffsetInstruction type, uintptr_t src, T dest) {
		Patch<uint8_t>(src, type == CALL ? 0xE8 : 0xE9);
		auto old = ReadRelative(src + 1);
		Patch<intptr_t>(src + 1, MakeRelative(src + 1, (uintptr_t)dest));
		return old;
	}
}

#endif