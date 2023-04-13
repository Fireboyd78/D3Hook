#pragma once

// Copyright (C) Force67

#if defined(HOOK_EXE)
#include <cstdint>
#include <Windows.h>

// minimalistic loader for win PE
class minLdr
{
public:
	using libLoader_t = HMODULE(*)(const char*);
	using fnResolver_t = LPVOID(*)(HMODULE, const char*);

	minLdr(const uint8_t* origBinary);

	void SetLoadLimit(uintptr_t loadLimit) { limit = loadLimit; }
	void* GetEntryPoint() const { return entry; }

	bool load(HMODULE module);

	void setLibLoader(libLoader_t loader) { libLoader = loader; }
	void setFuncResolver(fnResolver_t resolver) { fnResolver = resolver; }

private:
	const uint8_t* bin;
	HMODULE handle;
	uintptr_t limit = UINT_MAX;
	void* entry;

	libLoader_t libLoader;
	fnResolver_t fnResolver;

	bool mapSections(IMAGE_NT_HEADERS*);
	void loadImports(IMAGE_NT_HEADERS*);

	template <class T> inline const T* GetRVA(uint32_t rva) { return (T*)(bin + rva); }
	template <class T> inline T* GetTargetRVA(uint32_t rva) { return (T*)((uint8_t*)handle + rva); }
};
#endif