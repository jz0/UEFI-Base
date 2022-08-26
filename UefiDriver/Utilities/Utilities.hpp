#pragma once
#include "../Include.hpp"

#define CONTAINING_RECORD(address, type, field) ((type *)((UINT8 *)(address) - (UINTN)(&((type *)0)->field)))

namespace Utilities
{
	void ZeroMem(void* Buffer, UINTN Size);
	VOID CopyMem(void* Dest, const void* Src, UINTN Len);
	uintptr_t FindPattern(uintptr_t Image, const char16_t* Sig);
	KLDR_DATA_TABLE_ENTRY* GetModuleEntry(LIST_ENTRY* List, const char16_t* Name);
	INTN EFIAPI AsciiStrCmp(IN const CHAR8* FirstString, IN const CHAR8* SecondString);
	PIMAGE_DOS_HEADER GetDosHeaders(void* ImageBase);
	PIMAGE_NT_HEADERS64 GetNtHeaders(void* ImageBase);
	uintptr_t GetExport(uintptr_t ImageBase, CHAR8* Export);
	void ErasePE(EFI_LOADED_IMAGE_PROTOCOL* LoadedImage);
	void WaitForKey();
}