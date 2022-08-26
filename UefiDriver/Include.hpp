#pragma once
#include <UEFI/UEFI.h>
#include "winnt.hpp"
#include "WDK.hpp"
#include "XorStr.hpp"

//Utils
#include "Utilities/Utilities.hpp"
#include "Utilities/Print.hpp"

//Hooks
#include "Hooks/ExitBootServices.hpp"

inline EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* TextProtocol;
inline EFI_RUNTIME_SERVICES* GlobalRuntimeServices;
inline EFI_BOOT_SERVICES* GlobalBootServices;
inline EFI_SYSTEM_TABLE* GlobalSystemTable;
inline uintptr_t DbgPrintPtr;
inline bool InVirtualSpace = false;

#define MM_COPY_MEMORY_PHYSICAL             0x1
#define MM_COPY_MEMORY_VIRTUAL              0x2

#define	PAGE_NOACCESS		0x01
#define	PAGE_READONLY		0x02
#define	PAGE_READWRITE		0x04
#define	PAGE_WRITECOPY		0x08
#define	PAGE_EXECUTE		0x10
#define	PAGE_EXECUTE_READ	0x20
#define	PAGE_EXECUTE_READWRITE	0x40
#define	PAGE_EXECUTE_WRITECOPY	0x80
#define	PAGE_GUARD		0x100
#define	PAGE_NOCACHE		0x200
#define	PAGE_WRITECOMBINE	0x400

#define NtCurrentProcess() ( (void*) -1 )

typedef int (EFIAPI* PsLookupProcessByProcessId)(
	void* ProcessId,
	void* OutPEProcess
	);

typedef void* (EFIAPI* PsGetProcessSectionBaseAddress)(
	void* PEProcess
	);

typedef PEPROCESS(EFIAPI* PsGetCurrentProcess)(
	);

typedef int (EFIAPI* MmCopyVirtualMemory)(
	void* SourceProcess,
	void* SourceAddress,
	void* TargetProcess,
	void* TargetAddress,
	uintptr_t BufferSize,
	char PreviousMode,
	void* ReturnSize
	);

typedef PPEB(EFIAPI* PsGetProcessPeb)(
	void* PEProcess
	);

typedef void* (EFIAPI* PsGetProcessWow64Process)(
	void* PEProcess
	);

typedef void (EFIAPI* KeStackAttachProcess)(
	void* PEProcess,
	PRKAPC_STATE ApcState
	);

typedef void (EFIAPI* KeUnstackDetachProcess)(
	PRKAPC_STATE ApcState
	);

typedef void* (EFIAPI* RtlFindExportedRoutineByName) (
	void* DllBase,
	char* RoutineName);

typedef int (EFIAPI* ZwAllocateVirtualMemory) (
	void* ProcessHandle,
	void** BaseAddress,
	uintptr_t ZeroBits,
	uintptr_t* RegionSize,
	unsigned long AllocationType,
	unsigned long Protect
	);

typedef int (EFIAPI* ZwFreeVirtualMemory) (
	void* ProcessHandle,
	void** BaseAddress, 
	uintptr_t* RegionSize,
	unsigned long FreeType
	);

typedef int (EFIAPI* ZwProtectVirtualMemory) (
	void* ProcessHandle,
	void** BaseAddress,
	uintptr_t* NumberOfBytesToProtect,
	unsigned long NewAccessProtection,
	unsigned long* OldAccessProtection
	);

typedef int (EFIAPI* ZwQueryVirtualMemory) (
	void* ProcessHandle,
	void* BaseAddress,
	MEMORY_BASIC_INFORMATION MemoryInformationClass,
	void* MemoryInfromation,
	unsigned long long MemoryInformationLength,
	unsigned long long* ReturnLength
	);

typedef void* (EFIAPI* MmGetVirtualForPhysical) (
	PHYSICAL_ADDRESS PhysAddress
	);

typedef int (EFIAPI* MmCopyMemory) (
	void* TargetAddress,
	MM_COPY_ADDRESS SourceAddress,
	uintptr_t NumberOfBytes,
	unsigned long Flags,
	uintptr_t* NumberOfBytesTransferred
	);

typedef PHYSICAL_ADDRESS(EFIAPI* MmGetPhysicalAddress) (
	void* BaseAddress
	);

typedef void* (EFIAPI* MmMapIoSpaceEx) (
	PHYSICAL_ADDRESS PhysicalAddress,
	uintptr_t           NumberOfBytes,
	unsigned long            Protect
	);

typedef void (EFIAPI* MmUnmapIoSpace) (
	void* BaseAddress,
	uintptr_t NumberOfBytes
	);

typedef PKTHREAD (EFIAPI* KeGetCurrentThread) (
	);

typedef void (EFIAPI* KeBugCheck) (
	unsigned long BugCheckCode
	);

inline PEPROCESS PsInitialSystemProcess;
inline PsLookupProcessByProcessId GetProcessByPid;
inline PsGetProcessSectionBaseAddress GetBaseAddress;
inline PsGetCurrentProcess GetCurrentProcess;
inline MmCopyVirtualMemory MCopyVirtualMemory;
inline PsGetProcessPeb GetProcessPeb;
inline PsGetProcessWow64Process GetProcessWow64Process;
inline RtlFindExportedRoutineByName FindExportedRoutineByName;
inline ZwAllocateVirtualMemory AllocateVirtualMemory;
inline ZwFreeVirtualMemory FreeVirtualMemory;
inline ZwProtectVirtualMemory ProtectVirtualMemory;
inline ZwQueryVirtualMemory QueryVirtualMemory;
inline MmGetVirtualForPhysical GetVirtualForPhysical;
inline MmCopyMemory NtCopyMemory;
inline MmGetPhysicalAddress GetPhysicalAddress;
inline MmMapIoSpaceEx MapIoSpaceEx;
inline MmUnmapIoSpace UnmapIoSpace;
inline KeGetCurrentThread GetCurrentThread;
inline KeBugCheck BugCheck;