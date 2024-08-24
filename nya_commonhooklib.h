#ifndef NYA_COMMON_HOOKING
#define NYA_COMMON_HOOKING
#include <cstdint>
#include <psapi.h>

namespace NyaHookLib {
	template<typename T>
	void Patch(uintptr_t address, T value) {
		DWORD backup, backup2;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &backup);
		memcpy((void*)address, (void*)&value, sizeof(value));
		VirtualProtect((void*)address, sizeof(value), backup, &backup2);
	}

	void WriteString(uintptr_t address, const char* string, bool includeTerminator = true) {
		size_t size = strlen(string) + includeTerminator;

		DWORD backup, backup2;
		VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &backup);
		memcpy((void*)address, string, size);
		VirtualProtect((void*)address, size, backup, &backup2);
	}

	void Fill(uintptr_t address, uint8_t value, unsigned int count) {
		DWORD backup, backup2;
		VirtualProtect((void*)address, count, PAGE_EXECUTE_READWRITE, &backup);
		memset((void*)address, value, count);
		VirtualProtect((void*)address, count, backup, &backup2);
	}

	inline int32_t MakeRelative(uintptr_t src, uintptr_t dest) {
		return dest - (src + 4);
	}

	inline intptr_t ReadRelative(uintptr_t src) {
		return src + *(int32_t*)src + 4;
	}

	enum eOffsetInstruction {
		CALL,
		JMP,
		RAW,
	};

#ifdef __x86_64
	uintptr_t MakeTrampoline(uintptr_t src, uintptr_t dest) {
		// don't create a trampoline if already within int32 range
		int64_t initialDiff = src - dest;
		if (abs(initialDiff) < INT32_MAX) return dest;

		struct Trampoline {
			uint8_t instructions[6];
			uintptr_t destination;
		} __attribute__((packed));
		static_assert(sizeof(Trampoline) == 14);

		SYSTEM_INFO sys;
		GetSystemInfo( &sys );

		size_t size = sizeof(Trampoline);

		uintptr_t lowestAddr = src - INT32_MAX;
		if (lowestAddr > src) lowestAddr = sys.dwAllocationGranularity; // not sure if trying the zero page is a good idea >w<
		uintptr_t highestAddr = src + INT32_MAX;
		if (highestAddr < src) highestAddr = INT64_MAX;

		uintptr_t addr = lowestAddr;
		while (addr < highestAddr) {
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery( (void*)addr, &info, sizeof(info));
			if (info.State == MEM_FREE && info.RegionSize >= size && ((uintptr_t)info.BaseAddress >= lowestAddr) && ((uintptr_t)info.BaseAddress <= highestAddr)) {
				uintptr_t target = (uintptr_t)info.BaseAddress;
				if (target % sys.dwAllocationGranularity != 0) target += sys.dwAllocationGranularity - (target % sys.dwAllocationGranularity);
				if (auto trampoline = (Trampoline*)VirtualAlloc( (void*)target, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE )) {
					memset(trampoline, 0, size);
					trampoline->instructions[0] = 0xFF;
					trampoline->instructions[1] = 0x25;
					trampoline->destination = dest;
					return (uintptr_t)trampoline;
				}
			}

			addr += info.RegionSize;
		}
		return 0;
	}
#endif

	template<typename T>
	uintptr_t PatchRelative(eOffsetInstruction type, uintptr_t src, T dest) {
		if (type == RAW) src -= 1;
		else Patch<uint8_t>(src, type == CALL ? 0xE8 : 0xE9);
		auto old = ReadRelative(src + 1);

#ifdef __x86_64
		auto realDest = MakeTrampoline(src + 1, (uintptr_t)dest);
		if (!realDest) return old;
#else
		auto realDest = (uintptr_t)dest;
#endif

		Patch<int32_t>(src + 1, MakeRelative(src + 1, realDest));
		return old;
	}

	auto mEXEBase = (uintptr_t)GetModuleHandleA(nullptr);

	uintptr_t GetEntryPoint(HMODULE module) {
		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(info));
		return (uintptr_t)info.EntryPoint - (uintptr_t)info.lpBaseOfDll;
	}

	uintptr_t GetEntryPoint() {
		return GetEntryPoint((HMODULE)mEXEBase);
	}
}

#endif