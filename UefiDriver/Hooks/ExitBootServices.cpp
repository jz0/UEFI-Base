#include "ExitBootServices.hpp"

EFI_EXIT_BOOT_SERVICES ExitBootServicesOriginal;
EFI_STATUS EFIAPI HookedExitBootServices(EFI_HANDLE ImageHandle, UINTN MapKey)
{
	//thx samuel tulach for the idea
	uintptr_t WinloadEntryPtr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));

	while (Utilities::GetDosHeaders(reinterpret_cast<void*>(WinloadEntryPtr)) == NULL)
		WinloadEntryPtr = WinloadEntryPtr - 0x1;

	auto NtHeader = Utilities::GetNtHeaders(reinterpret_cast<void*>(WinloadEntryPtr));

	IMAGE_DATA_DIRECTORY ExportDataDir = NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_EXPORT_DIRECTORY* ExportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(WinloadEntryPtr + ExportDataDir.VirtualAddress);

	//40 53 48 83 EC 20 48 8B 15 BlpArchSwitchContext 
	auto BlpArchSwitchContext = Utilities::FindPattern(WinloadEntryPtr, Crypt(u"40 53 48 83 EC 20 48 8B 15"));

	//48 8B 3D ? ? ? ? 48 8B 8F LoaderBlock
	auto LoaderBlockScan = Utilities::FindPattern(WinloadEntryPtr, Crypt(u"48 8B 3D ? ? ? ? 48 8B 8F"));

	uintptr_t ResolvedAddress = *reinterpret_cast<uintptr_t*>((LoaderBlockScan + 7) + *(int*)(LoaderBlockScan + 3));

	//Switch to application context
	reinterpret_cast<void(EFIAPI*)(int)> (BlpArchSwitchContext)(0); //Application Context

	LOADER_PARAMETER_BLOCK* LoaderBlock = reinterpret_cast<LOADER_PARAMETER_BLOCK*>(ResolvedAddress);
	KLDR_DATA_TABLE_ENTRY* ntoskrnl = Utilities::GetModuleEntry(&LoaderBlock->LoadOrderListHead, Crypt(u"ntoskrnl.exe"));

	DbgPrintPtr = Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("DbgPrint"));
	GetProcessByPid = (PsLookupProcessByProcessId)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsLookupProcessByProcessId"));
	GetBaseAddress = (PsGetProcessSectionBaseAddress)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsGetProcessSectionBaseAddress"));
	GetCurrentProcess = (PsGetCurrentProcess)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsGetCurrentProcess"));
	MCopyVirtualMemory = (MmCopyVirtualMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmCopyVirtualMemory"));
	GetProcessPeb = (PsGetProcessPeb)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsGetProcessPeb"));
	GetProcessWow64Process = (PsGetProcessWow64Process)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsGetProcessWow64Process"));
	AllocateVirtualMemory = (ZwAllocateVirtualMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("ZwAllocateVirtualMemory"));
	FreeVirtualMemory = (ZwFreeVirtualMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("ZwFreeVirtualMemory"));
	ProtectVirtualMemory = (ZwProtectVirtualMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("ZwProtectVirtualMemory"));
	QueryVirtualMemory = (ZwQueryVirtualMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("ZwQueryVirtualMemory"));
	GetVirtualForPhysical = (MmGetVirtualForPhysical)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmGetVirtualForPhysical"));
	NtCopyMemory = (MmCopyMemory)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmCopyMemory"));
	PsInitialSystemProcess = (PEPROCESS)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("PsInitialSystemProcess"));
	GetPhysicalAddress = (MmGetPhysicalAddress)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmGetPhysicalAddress"));
	MapIoSpaceEx = (MmMapIoSpaceEx)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmMapIoSpaceEx"));
	UnmapIoSpace = (MmUnmapIoSpace)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("MmUnmapIoSpace"));
	GetCurrentThread = (KeGetCurrentThread)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("KeGetCurrentThread"));
	BugCheck = (KeBugCheck)Utilities::GetExport((uintptr_t)ntoskrnl->ImageBase, Crypt("KeBugCheck"));

	auto ImgBase = ntoskrnl->ImageBase;
	reinterpret_cast<void(EFIAPI*)(int)> (BlpArchSwitchContext)(1); //Firmware Context
	//Switch back to firmware context so we can print
	Console::Print(Crypt(u"ntoskrnl %x \n"), ImgBase);
	Console::Print(Crypt(u"DbgPrint %x \n"), DbgPrintPtr);
	Utilities::WaitForKey();

	//Restore pointer
	GlobalBootServices->ExitBootServices = ExitBootServicesOriginal;
	return GlobalBootServices->ExitBootServices(ImageHandle, MapKey);
}