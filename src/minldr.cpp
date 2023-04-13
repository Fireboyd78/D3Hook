
// Copyright (C) Force67

#if defined(HOOK_EXE)
#include <algorithm>
#include <intrin.h>
#include "minldr.h"

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.zdata")
#pragma comment(linker, "/merge:.text=.zdata")
#pragma comment(linker, "/section:.zdata,re")

#pragma bss_seg(".cdriv3r")
char dummy_seg[0x500000];

#pragma data_seg(".zdata")
char zdata[0x50000] = { 1 };

minLdr::minLdr(const uint8_t* origBinary) :
	bin(origBinary)
{
}

void minLdr::loadImports(IMAGE_NT_HEADERS* nt)
{
	auto* dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto* desc = GetTargetRVA<IMAGE_IMPORT_DESCRIPTOR>(dir->VirtualAddress);

	while (desc->Name)
	{
		const char* name = GetTargetRVA<char>(desc->Name);

		HMODULE module = libLoader(name);

		if (!module)
		{
			char buf[512]{};
			snprintf(buf, 512, "Could not load dependent module %s. Error code was %i.", name, GetLastError());
			MessageBoxA(nullptr, buf, "D3Hook", MB_OK | MB_ICONWARNING);
		}

		if (reinterpret_cast<uint32_t>(module) == 0xFFFFFFFF)
		{
			desc++;
			continue;
		}

		auto nameTableEntry = GetTargetRVA<uint32_t>(desc->OriginalFirstThunk);
		auto addressTableEntry = GetTargetRVA<uint32_t>(desc->FirstThunk);

		while (*nameTableEntry)
		{
			FARPROC function;
			const char* functionName;

			if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
			{
				function = GetProcAddress(module, MAKEINTRESOURCEA(IMAGE_ORDINAL(*nameTableEntry)));
				functionName = (const char*)IMAGE_ORDINAL(*nameTableEntry);
			}
			else
			{
				const auto import = GetTargetRVA<IMAGE_IMPORT_BY_NAME>(*nameTableEntry);

				function = reinterpret_cast<FARPROC>(fnResolver(module, import->Name));
				functionName = import->Name;
			}

			if (!function)
			{
				char pathName[MAX_PATH];
				GetModuleFileNameA(module, pathName, sizeof(pathName));

				char buf[512]{};
				snprintf(buf, 512, "Could not load function %s in dependent module %s (%s).", functionName, name, pathName);
				MessageBoxA(nullptr, buf, "D3Hook", MB_OK | MB_ICONWARNING);
			}

			*addressTableEntry = reinterpret_cast<uint32_t>(function);

			nameTableEntry++;
			addressTableEntry++;
		}

		desc++;
	}
}

bool minLdr::mapSections(IMAGE_NT_HEADERS* nt)
{
	auto* section = IMAGE_FIRST_SECTION(nt);

	for (int i = 0; i < nt->FileHeader.NumberOfSections; i++)
	{
		void* targetAddress = GetTargetRVA<uint8_t>(section->VirtualAddress);
		const void* sourceAddress = bin + section->PointerToRawData;

		if ((uintptr_t)targetAddress >= limit)
		{
			//FatalError("Exceeded load limit.");
			return false;
		}

		if (section->SizeOfRawData > 0)
		{
			DWORD oldProtect;
			uint32_t sizeOfData = std::min(section->SizeOfRawData, section->Misc.VirtualSize);

			VirtualProtect(targetAddress, sizeOfData, PAGE_EXECUTE_READWRITE, &oldProtect);
			memcpy(targetAddress, sourceAddress, sizeOfData);
		}

		section++;
	}

	return true;
}

bool minLdr::load(HMODULE mod)
{
	handle = mod;

	auto* dosTarget = (IMAGE_DOS_HEADER*)(bin);
	if (dosTarget->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto* dosSource = (IMAGE_DOS_HEADER*)mod;
	auto* ntSource = GetTargetRVA<IMAGE_NT_HEADERS>(dosSource->e_lfanew);

	auto* ntTarget = (IMAGE_NT_HEADERS*)(bin + dosTarget->e_lfanew);

	// workaround for old PE images (bugs with windows theme)
	if (ntTarget->FileHeader.TimeDateStamp < 0x43AB45B2)
		ntTarget->FileHeader.TimeDateStamp = ntTarget->FileHeader.TimeDateStamp;

	if (!mapSections(ntTarget)) {
		MessageBoxW(nullptr, L"FATAL ERROR: Unable to map image!", L"D3Hook", MB_OK | MB_ICONERROR);
		return false;
	}

	loadImports(ntTarget);

	entry = GetTargetRVA<void>(ntTarget->OptionalHeader.AddressOfEntryPoint);

	DWORD oldProtect;
	VirtualProtect(ntSource, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

	ntSource->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = ntTarget->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	memcpy(ntSource, ntTarget, sizeof(IMAGE_NT_HEADERS) + (ntTarget->FileHeader.NumberOfSections * (sizeof(IMAGE_SECTION_HEADER))));

	return true;

	/* i _am_ driver!*/

	//auto* peb = (char*)__readfsdword(0x30);
	//*(uintptr_t*)(peb + 8) = 
}
#endif