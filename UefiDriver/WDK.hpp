#pragma once

typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY* Flink;
	struct _LIST_ENTRY* Blink;
} LIST_ENTRY, * PLIST_ENTRY, PRLIST_ENTRY;

typedef struct _UNICODE_STRING
{
	UINT16 Length;
	UINT16 MaximumLength;
	CHAR16* Buffer;
} UNICODE_STRING;

typedef struct _KTHREAD
{
	void* Header;                                       //0x0
	void* SListFaultAddress;                                                //0x18
} *PKTHREAD;

typedef struct _tag_thread_info
{
	PKTHREAD owning_thread;
}tag_thread_info;

typedef struct _EX_FAST_REF
{
	union
	{
		void* Object;                                                       //0x0
		unsigned long long RefCnt : 4;                                                 //0x0
		unsigned long long Value;                                                    //0x0
	};
} EX_FAST_REF, * PEX_FAST_REF;

typedef struct _PEB_LDR_DATA
{
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB_LDR_DATA32
{

} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{

} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	union
	{
		BOOLEAN BitField;
		struct
		{
			BOOLEAN ImageUsesLargePages : 1;
			BOOLEAN IsProtectedProcess : 1;
			BOOLEAN IsImageDynamicallyRelocated : 1;
			BOOLEAN SkipPatchingUser32Forwarders : 1;
			BOOLEAN IsPackagedProcess : 1;
			BOOLEAN IsAppContainer : 1;
			BOOLEAN IsProtectedProcessLight : 1;
			BOOLEAN IsLongPathAwareProcess : 1;
		} s1;
	} u1;
	void* Mutant;
	VOID* ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
} PEB, * PPEB;

typedef struct _PEB32
{
	unsigned char InheritedAddressSpace;
	unsigned char ReadImageFileExecOptions;
	unsigned char BeingDebugged;
	unsigned char BitField;
	unsigned long Mutant;
	unsigned long ImageBaseAddress;
	unsigned long Ldr;
	unsigned long ProcessParameters;
	unsigned long SubSystemData;
	unsigned long ProcessHeap;
	unsigned long FastPebLock;
	unsigned long AtlThunkSListPtr;
	unsigned long IFEOKey;
	unsigned long CrossProcessFlags;
	unsigned long UserSharedInfoPtr;
	unsigned long SystemReserved;
	unsigned long AtlThunkSListPtr32;
	unsigned long ApiSetMap;
} PEB32, * PPEB32;

typedef struct _LDR_DATA_TABLE_ENTRY
{

} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY, ** PPLDR_DATA_TABLE_ENTRY;

typedef struct _KLDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	VOID* ExceptionTable;
	UINT32 ExceptionTableSize;
	VOID* GpValue;
	VOID* NonPagedDebugInfo;
	VOID* ImageBase;
	VOID* EntryPoint;
	UINT32 SizeOfImage;
	UNICODE_STRING FullImageName;
	UNICODE_STRING BaseImageName;
} KLDR_DATA_TABLE_ENTRY;

typedef struct _KPROCESS* PEPROCESS;

typedef enum _MODE {
	KernelMode,
	UserMode,
	MaximumMode
} MODE;

typedef struct _KAPC_STATE
{
	struct _LIST_ENTRY ApcListHead[2];                                      //0x0
	struct _KPROCESS* Process;                                              //0x20
	union
	{
		unsigned char InProgressFlags;                                              //0x28
		struct
		{
			unsigned char KernelApcInProgress : 1;                                    //0x28
			unsigned char SpecialApcInProgress : 1;                                   //0x28
		};
	};
	unsigned char KernelApcPending;                                                 //0x29
	union
	{
		unsigned char UserApcPendingAll;                                            //0x2a
		struct
		{
			unsigned char SpecialUserApcPending : 1;                                  //0x2a
			unsigned char UserApcPending : 1;                                         //0x2a
		};
	};
} KAPC_STATE, * PKAPC_STATE, * PRKAPC_STATE;

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
	struct {
		unsigned long LowPart;
		long HighPart;
	} DUMMYSTRUCTNAME;
	struct {
		unsigned long LowPart;
		long HighPart;
	} u;
#endif //MIDL_PASS
	long long QuadPart;
} LARGE_INTEGER;
typedef LARGE_INTEGER PHYSICAL_ADDRESS, * PPHYSICAL_ADDRESS;

typedef struct _MM_COPY_ADDRESS {
	union {
		void* VirtualAddress;
		PHYSICAL_ADDRESS PhysicalAddress;
	};
} MM_COPY_ADDRESS, * PMMCOPY_ADDRESS;

typedef struct _PHYSICAL_MEMORY_RANGE {

	PHYSICAL_ADDRESS BaseAddress;

	LARGE_INTEGER NumberOfBytes;

} PHYSICAL_MEMORY_RANGE, * PPHYSICAL_MEMORY_RANGE;

typedef struct _MEMORY_BASIC_INFORMATION {
	void* BaseAddress;
	void* AllocationBase;
	unsigned long AllocationProtect;
	unsigned short   PartitionId;
	unsigned long long RegionSize;
	unsigned long State;
	unsigned long Protect;
	unsigned long Type;
} MEMORY_BASIC_INFORMATION, * PMEMORY_BASIC_INFORMATION;

typedef struct _VIRTUAL_EFI_RUNTIME_SERVICES
{
	uintptr_t GetTime;                                                      //0x0
	uintptr_t SetTime;                                                      //0x8
	uintptr_t GetWakeupTime;                                                //0x10
	uintptr_t SetWakeupTime;                                                //0x18
	uintptr_t SetVirtualAddressMap;                                         //0x20
	uintptr_t ConvertPointer;                                               //0x28
	uintptr_t GetVariable;                                                  //0x30
	uintptr_t GetNextVariableName;                                          //0x38
	uintptr_t SetVariable;                                                  //0x40
	uintptr_t GetNextHighMonotonicCount;                                    //0x48
	uintptr_t ResetSystem;                                                  //0x50
	uintptr_t UpdateCapsule;                                                //0x58
	uintptr_t QueryCapsuleCapabilities;                                     //0x60
	uintptr_t QueryVariableInfo;                                            //0x68
} VIRTUAL_EFI_RUNTIME_SERVICES;

typedef struct _EFI_FIRMWARE_INFORMATION
{
	UINT32 FirmwareVersion;                                                  //0x0
	VIRTUAL_EFI_RUNTIME_SERVICES* VirtualEfiRuntimeServices;        //0x8
	EFI_STATUS SetVirtualAddressMapStatus;                                        //0x10
	UINT32 MissedMappingsCount;                                              //0x14
	LIST_ENTRY FirmwareResourceList;                                //0x18
	VOID* EfiMemoryMap;                                                     //0x28
	UINT32 EfiMemoryMapSize;                                                 //0x30
	UINT32 EfiMemoryMapDescriptorSize;                                       //0x34
} EFI_FIRMWARE_INFORMATION;

typedef struct _FIRMWARE_INFORMATION_LOADER_BLOCK
{
	uintptr_t PAD;
	_EFI_FIRMWARE_INFORMATION EfiInformation;
} FIRMWARE_INFORMATION_LOADER_BLOCK;

typedef struct _LOADER_PARAMETER_BLOCK
{
	UINT32 OsMajorVersion;                                                   //0x0
	UINT32 OsMinorVersion;                                                   //0x4
	UINT32 Size;                                                             //0x8
	UINT32 OsLoaderSecurityVersion;                                          //0xc
	LIST_ENTRY LoadOrderListHead;                                   //0x10
	LIST_ENTRY MemoryDescriptorListHead;                            //0x20
	LIST_ENTRY BootDriverListHead;                                  //0x30
	LIST_ENTRY EarlyLaunchListHead;                                 //0x40
	LIST_ENTRY CoreDriverListHead;                                  //0x50
	LIST_ENTRY CoreExtensionsDriverListHead;                        //0x60
	LIST_ENTRY TpmCoreDriverListHead;                               //0x70
	uintptr_t KernelStack;                                                  //0x80
	uintptr_t Prcb;                                                         //0x88
	uintptr_t Process;                                                      //0x90
	uintptr_t Thread;                                                       //0x98
	uintptr_t KernelStackSize;                                                  //0xa0
	uintptr_t RegistryLength;                                                   //0xa4
	VOID* RegistryBase;                                                     //0xa8
	uintptr_t ConfigurationRoot;                //0xb0
//	typedef struct _CONFIGURATION_COMPONENT_DATA* ConfigurationRoot;                //0xb0
	char* ArcBootDeviceName;                                                //0xb8
	char* ArcHalDeviceName;                                                 //0xc0
	char* NtBootPathName;                                                   //0xc8
	char* NtHalPathName;                                                    //0xd0
	uintptr_t LoadOptions;                                                      //0xd8
	uintptr_t NlsData;
	uintptr_t ArcDiskInformation;
	uintptr_t Extension;
	uintptr_t I386;
	//typedef struct _NLS_DATA_BLOCK* NlsData;                                        //0xe0
	//typedef struct _ARC_DISK_INFORMATION* ArcDiskInformation;                       //0xe8
	//typedef struct _LOADER_PARAMETER_EXTENSION* Extension;                          //0xf0
	//union
	//{
	//	typedef struct _I386_LOADER_BLOCK I386;                                     //0xf8
	//	typedef struct _ARM_LOADER_BLOCK Arm;                                       //0xf8
	//} u;                                                                    //0xf8
	_FIRMWARE_INFORMATION_LOADER_BLOCK FirmwareInformation;          //0x108
	char* OsBootstatPathName;                                               //0x148
	char* ArcOSDataDeviceName;                                              //0x150
	char* ArcWindowsSysPartName;                                            //0x158
} LOADER_PARAMETER_BLOCK;