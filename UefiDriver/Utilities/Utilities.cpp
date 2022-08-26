#include "Utilities.hpp"
#include <intrin.h>

void
Utilities::ZeroMem(
	void* Buffer,
	UINTN     Size
) __attribute__((no_builtin("memset")))
{
	INT8* pt;

	pt = (INT8*)Buffer;
	while (Size--) {
		*(pt++) = 0;
	}

}

void
Utilities::CopyMem(
	void* Dest,
	const void* Src,
	UINTN       Len
)
{
	CHAR8* d = (CHAR8*)Dest;
	CHAR8* s = (CHAR8*)Src;

	if (d == NULL || s == NULL || s == d)
		return;

	// If the beginning of the destination range overlaps with the end of
	// the source range, make sure to start the copy from the end so that
	// we don't end up overwriting source data that we need for the copy.
	if ((d > s) && (d < s + Len)) {
		for (d += Len, s += Len; Len--; )
			*--d = *--s;
	}
	else {
		while (Len--)
			*d++ = *s++;
	}
}

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (IN_RANGE(x, '0', '9') ? x - '0': 0))
#define GET_BYTE(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

uintptr_t Utilities::FindPattern(uintptr_t Image, const char16_t* Sig)
{
	const auto DOSHeader = PIMAGE_DOS_HEADER(Image);
	const auto NTHeaders = PIMAGE_NT_HEADERS(reinterpret_cast<uint8_t*>(Image) + DOSHeader->e_lfanew);

	uintptr_t StartAddress = Image;
	uintptr_t Size = (StartAddress + (uintptr_t)NTHeaders->OptionalHeader.SizeOfImage);

	const char16_t* Pat = Sig;
	uintptr_t FirstMatch = 0;
	for (uintptr_t pCur = StartAddress; pCur < Size; pCur++)
	{
		if (!*Pat)
			return FirstMatch;

		if (*(uint8_t*)Pat == ('\?') || *(uint8_t*)pCur == GET_BYTE(Pat))
		{
			if (!FirstMatch)
				FirstMatch = pCur;

			if (!Pat[2])
				return FirstMatch;

			if (*(uint16_t*)Pat == ('\?\?') || *(uint8_t*)Pat != ('\?'))
				Pat += 3;

			else Pat += 2;
		}
		else
		{
			Pat = Sig;
			FirstMatch = 0;
		}
	}

	return NULL;
}

CHAR8
EFIAPI
AsciiCharToUpper(
	IN      CHAR8  Chr
)
{
	return (UINT8)((Chr >= 'a' && Chr <= 'z') ? Chr - ('a' - 'A') : Chr);
}

INTN
EFIAPI
StrnCmpA(
	IN      const CHAR16* FirstString,
	IN      const CHAR16* SecondString,
	IN      UINTN                     Length
)
{
	while ((*FirstString != L'\0') &&
		(*SecondString != L'\0') &&
		(AsciiCharToUpper((CHAR8)*FirstString) == AsciiCharToUpper((CHAR8)*SecondString)) &&
		(Length > 1)) {
		FirstString++;
		SecondString++;
		Length--;
	}
	return *FirstString - *SecondString;
}

KLDR_DATA_TABLE_ENTRY* Utilities::GetModuleEntry(LIST_ENTRY* List, const char16_t* Name)
{
	for (LIST_ENTRY* entry = List->Flink; entry != List; entry = entry->Flink)
	{
		KLDR_DATA_TABLE_ENTRY* module = CONTAINING_RECORD(entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (module && StrnCmpA((CHAR16*)Name, module->BaseImageName.Buffer, module->BaseImageName.Length) == 0)
		{
			return module;
		}
	}

	return NULL;
}

INTN
EFIAPI
Utilities::AsciiStrCmp(
	IN      const CHAR8* FirstString,
	IN      const CHAR8* SecondString
)
{
	while ((*FirstString != '\0') && (*FirstString == *SecondString)) {
		FirstString++;
		SecondString++;
	}

	return *FirstString - *SecondString;
}

PIMAGE_DOS_HEADER Utilities::GetDosHeaders(void* ImageBase)
{
	PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)(ImageBase);
	if (!ImageDosHeader)
		return NULL;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;
	return ImageDosHeader;
}

PIMAGE_NT_HEADERS64 Utilities::GetNtHeaders(void* ImageBase)
{
	PIMAGE_DOS_HEADER ImageDosHdr;
	PIMAGE_NT_HEADERS64 ImageNtHdr;
	ImageDosHdr = GetDosHeaders(ImageBase);
	if (!ImageDosHdr)
		return NULL;

	ImageNtHdr = (PIMAGE_NT_HEADERS64)((UINT8*)ImageBase + ImageDosHdr->e_lfanew);
	if (ImageNtHdr->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	return ImageNtHdr;
}

uintptr_t Utilities::GetExport(uintptr_t ImageBase, CHAR8* Export)
{
	auto DOSHeader = GetDosHeaders((void*)ImageBase);

	IMAGE_NT_HEADERS64* NTHeaders = (IMAGE_NT_HEADERS64*)(ImageBase + DOSHeader->e_lfanew);

	uint32_t ExportsRVA = NTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (!ExportsRVA)
		return uintptr_t{};

	IMAGE_EXPORT_DIRECTORY* Exports = (IMAGE_EXPORT_DIRECTORY*)(ImageBase + ExportsRVA);

	uint32_t* NameRVA = (uint32_t*)(ImageBase + Exports->AddressOfNames);

	for (uint32_t i = 0; i < Exports->NumberOfNames; ++i)
	{
		CHAR8* Func = (CHAR8*)(ImageBase + NameRVA[i]);

		if (AsciiStrCmp(Func, Export) == 0)
		{
			uint32_t* FuncRVA = (uint32_t*)(ImageBase + Exports->AddressOfFunctions);
			uint16_t* OrdinalRVA = (uint16_t*)(ImageBase + Exports->AddressOfNameOrdinals);
			return (uintptr_t)ImageBase + FuncRVA[OrdinalRVA[i]];
		}
	}

	return uintptr_t{};
}

void Utilities::ErasePE(EFI_LOADED_IMAGE_PROTOCOL* LoadedImage)
{
	void* ImageAddress = LoadedImage->ImageBase;
	PIMAGE_NT_HEADERS64 NtHeader = GetNtHeaders(ImageAddress);
	PIMAGE_DOS_HEADER DosHeader = GetDosHeaders(ImageAddress);

	//Console::Print(u"Magic %x \n", DosHeader->e_magic);
	//Console::Print(u"Machine %x \n", NtHeader->FileHeader.Machine);
	//Console::Print(u"ImageBase %x \n", NtHeader->OptionalHeader.ImageBase);
	//Console::Print(u"CheckSum %x \n", NtHeader->OptionalHeader.CheckSum);
	//Console::Print(u"SizeOfHeaders %x \n", NtHeader->OptionalHeader.SizeOfHeaders);
	//Console::Print(u"AddressOfEntryPoint %x \n", NtHeader->OptionalHeader.AddressOfEntryPoint);

	ZeroMem(ImageAddress, NtHeader->OptionalHeader.SizeOfHeaders);

	//Console::Print(u"Magic %x \n", DosHeader->e_magic);
	//Console::Print(u"Machine %x \n", NtHeader->FileHeader.Machine);
	//Console::Print(u"ImageBase %x \n", NtHeader->OptionalHeader.ImageBase);
	//Console::Print(u"CheckSum %x \n", NtHeader->OptionalHeader.CheckSum);
	//Console::Print(u"SizeOfHeaders %x \n", NtHeader->OptionalHeader.SizeOfHeaders);
	//Console::Print(u"AddressOfEntryPoint %x \n", NtHeader->OptionalHeader.AddressOfEntryPoint);
}

void Utilities::WaitForKey()
{
	EFI_STATUS         Status;
	EFI_EVENT          WaitList;
	EFI_INPUT_KEY      Key;
	UINTN              Index;
	Console::Print(Crypt(u"Press escape to continue..."));

	do {
		WaitList = GlobalSystemTable->ConIn->WaitForKey;
		Status = GlobalBootServices->WaitForEvent(1, &WaitList, &Index);
		GlobalSystemTable->ConIn->ReadKeyStroke(GlobalSystemTable->ConIn, &Key);
		if (Key.ScanCode == 0x17) //Escape
			break;
	} while (1);

	GlobalSystemTable->ConOut->ClearScreen(GlobalSystemTable->ConOut);
}