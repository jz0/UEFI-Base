#pragma once

#include "UEFI_types.h"
#include "UEFI_text_input_protocol.h"
#include "EFI_boot_services.h"
#include "EFI_scan_codes.h"
#include "EFI_graphics_output_protcol.h"
#include "EFI_DevicePath_protocol.h"
#include "EFI_LoadedImage.h"


#include "utilities/utilities.hpp"

#if (defined (__GNUC__) && __GNUC__ >= 4) || defined (__clang__)
#define OFFSET_OF(TYPE, Field)  ((UINTN) __builtin_offsetof(TYPE, Field))
#endif

#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID {0x387477c2,0x69c7,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

//
// Open modes
//
#define EFI_FILE_MODE_READ    0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE   0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE  0x8000000000000000ULL

//
// File attributes
//
#define EFI_FILE_READ_ONLY   0x0000000000000001ULL
#define EFI_FILE_HIDDEN      0x0000000000000002ULL
#define EFI_FILE_SYSTEM      0x0000000000000004ULL
#define EFI_FILE_RESERVED    0x0000000000000008ULL
#define EFI_FILE_DIRECTORY   0x0000000000000010ULL
#define EFI_FILE_ARCHIVE     0x0000000000000020ULL
#define EFI_FILE_VALID_ATTR  0x0000000000000037ULL

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct _EFI_FILE_PROTOCOL* EFI_FILE_HANDLE;
typedef EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_FILE_IO_INTERFACE;
typedef EFI_FILE_PROTOCOL EFI_FILE;

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_DELETE)(
	IN EFI_FILE_PROTOCOL* This
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_CLOSE)(
	IN EFI_FILE_PROTOCOL* This
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_READ)(
	IN EFI_FILE_PROTOCOL* This,
	IN OUT UINTN* BufferSize,
	OUT VOID* Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_WRITE)(
	IN EFI_FILE_PROTOCOL* This,
	IN OUT UINTN* BufferSize,
	IN VOID* Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_SET_POSITION)(
	IN EFI_FILE_PROTOCOL* This,
	IN UINT64                   Position
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_GET_POSITION)(
	IN EFI_FILE_PROTOCOL* This,
	OUT UINT64* Position
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_GET_INFO)(
	IN EFI_FILE_PROTOCOL* This,
	IN EFI_GUID* InformationType,
	IN OUT UINTN* BufferSize,
	OUT VOID* Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_SET_INFO)(
	IN EFI_FILE_PROTOCOL* This,
	IN EFI_GUID* InformationType,
	IN UINTN                    BufferSize,
	IN VOID* Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_FILE_GET_POSITION)(
	IN EFI_FILE_PROTOCOL* This,
	OUT UINT64* Position
	);


typedef
EFI_STATUS
(EFIAPI* EFI_FILE_OPEN)(
	IN EFI_FILE_PROTOCOL* This,
	OUT EFI_FILE_PROTOCOL** NewHandle,
	IN CHAR16* FileName,
	IN UINT64                   OpenMode,
	IN UINT64                   Attributes
	);

typedef
EFI_STATUS
(EFIAPI* EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
	IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* This,
	OUT EFI_FILE_PROTOCOL** Root
	);


typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
	///
	/// The version of the EFI_SIMPLE_FILE_SYSTEM_PROTOCOL. The version
	/// specified by this specification is 0x00010000. All future revisions
	/// must be backwards compatible.
	///
	UINT64                                         Revision;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME    OpenVolume;
};

typedef struct _EFI_FILE_PROTOCOL {
	///
	/// The version of the EFI_FILE_PROTOCOL interface. The version specified
	/// by this specification is EFI_FILE_PROTOCOL_LATEST_REVISION.
	/// Future versions are required to be backward compatible to version 1.0.
	///
	UINT64                   Revision;
	EFI_FILE_OPEN            Open;
	EFI_FILE_CLOSE           Close;
	EFI_FILE_DELETE          Delete;
	EFI_FILE_READ            Read;
	EFI_FILE_WRITE           Write;
	EFI_FILE_GET_POSITION    GetPosition;
	EFI_FILE_SET_POSITION    SetPosition;
	EFI_FILE_GET_INFO        GetInfo;
	EFI_FILE_SET_INFO        SetInfo;
	//EFI_FILE_FLUSH           Flush;
	//EFI_FILE_OPEN_EX         OpenEx;
	//EFI_FILE_READ_EX         ReadEx;
	//EFI_FILE_WRITE_EX        WriteEx;
	//EFI_FILE_FLUSH_EX        FlushEx;
};

typedef struct {
	///
	/// The size of the EFI_FILE_INFO structure, including the Null-terminated FileName string.
	///
	UINT64      Size;
	///
	/// The size of the file in bytes.
	///
	UINT64      FileSize;
	///
	/// PhysicalSize The amount of physical space the file consumes on the file system volume.
	///
	UINT64      PhysicalSize;
	///
	/// The time the file was created.
	///
	void*    CreateTime;
	///
	/// The time when the file was last accessed.
	///
	void* LastAccessTime;
	///
	/// The time when the file's contents were last modified.
	///
	void* ModificationTime;
	///
	/// The attribute bits for the file.
	///
	UINT64      Attribute;
	///
	/// The Null-terminated name of the file.
	///
	CHAR16      FileName[1];
} EFI_FILE_INFO;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
	UINT16  Year;
	UINT8   Month;
	UINT8   Day;
	UINT8   Hour;
	UINT8   Minute;
	UINT8   Second;
	UINT8   Pad1;
	UINT32  Nanosecond;
	INT16   TimeZone;
	UINT8   Daylight;
	UINT8   Pad2;
} EFI_TIME;

typedef struct {
	EFI_GUID          CapsuleGuid;
	UINT32            HeaderSize;
	UINT32            Flags;
	UINT32            CapsuleImageSize;
} EFI_CAPSULE_HEADER;

typedef struct {
	UINT32    Resolution;
	UINT32    Accuracy;
	BOOLEAN   SetsToZero;
} EFI_TIME_CAPABILITIES;

typedef enum {
	EfiResetCold,
	EfiResetWarm,
	EfiResetShutdown,
	EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN BOOLEAN ExtendedVerification
	);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN const CHAR16 *String // I added `const` so passing string literals and const strings directly is possible. This shouldn't break binary compatibility.
	);

// Verifies that all characters in a string can be output to the target device.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN const CHAR16 *String // I added `const` so passing string literals and const strings directly is possible. This shouldn't break binary compatibility.
	);

// Returns information for an available text mode that the output device(s) supports.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN ModeNumber,
	OUT UINTN *Columns,
	OUT UINTN *Rows
	);

// Sets the output device(s) to a specified mode.
typedef
EFI_STATUS
(*EFIAPI EFI_TEXT_SET_MODE) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN ModeNumber
	);

// Sets the background and foreground colors for theOutputString() and ClearScreen() functions.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN Attribute
	);

// Clears the output device(s) display to the currently selected background color.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
	);

// Sets the current coordinates of the cursor position.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN Column,
	IN UINTN Row
	);

// Makes the cursor visible or invisible.
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN BOOLEAN Visible
	);

typedef
EFI_TPL
(EFIAPI* EFI_RAISE_TPL)(
	IN EFI_TPL      NewTpl
	);

typedef
VOID
(EFIAPI* EFI_RESTORE_TPL)(
	IN EFI_TPL      OldTpl
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_VARIABLE)(
	IN     CHAR16* VariableName,
	IN     EFI_GUID* VendorGuid,
	OUT    UINT32* Attributes, OPTIONAL
	IN OUT UINTN* DataSize,
	OUT    VOID* Data           OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_NEXT_VARIABLE_NAME)(
	IN OUT UINTN* VariableNameSize,
	IN OUT CHAR16* VariableName,
	IN OUT EFI_GUID* VendorGuid
	);

typedef
EFI_STATUS
(EFIAPI* EFI_SET_VARIABLE)(
	IN  CHAR16* VariableName,
	IN  EFI_GUID* VendorGuid,
	IN  UINT32                       Attributes,
	IN  UINTN                        DataSize,
	IN  VOID* Data
	);

typedef
EFI_STATUS
(EFIAPI* EFI_STALL)(
	IN  UINTN                    Microseconds
	);

typedef
EFI_STATUS
(EFIAPI* EFI_SET_WATCHDOG_TIMER)(
	IN UINTN                    Timeout,
	IN UINT64                   WatchdogCode,
	IN UINTN                    DataSize,
	IN CHAR16* WatchdogData OPTIONAL
	);

typedef
VOID
(EFIAPI* EFI_RESET_SYSTEM)(
	IN EFI_RESET_TYPE           ResetType,
	IN EFI_STATUS               ResetStatus,
	IN UINTN                    DataSize,
	IN VOID* ResetData OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_NEXT_MONOTONIC_COUNT)(
	OUT UINT64* Count
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_NEXT_HIGH_MONO_COUNT)(
	OUT UINT32* HighCount
	);

typedef
EFI_STATUS
(EFIAPI* EFI_CALCULATE_CRC32)(
	IN  VOID* Data,
	IN  UINTN                             DataSize,
	OUT UINT32* Crc32
	);

typedef
VOID
(EFIAPI* EFI_COPY_MEM)(
	IN VOID* Destination,
	IN VOID* Source,
	IN UINTN    Length
	);

typedef
VOID
(EFIAPI* EFI_SET_MEM)(
	IN VOID* Buffer,
	IN UINTN    Size,
	IN UINT8    Value
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_TIME)(
	OUT  EFI_TIME* Time,
	OUT  EFI_TIME_CAPABILITIES* Capabilities OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_SET_TIME)(
	IN  EFI_TIME* Time
	);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_WAKEUP_TIME)(
	OUT BOOLEAN* Enabled,
	OUT BOOLEAN* Pending,
	OUT EFI_TIME* Time
	);

typedef
EFI_STATUS
(EFIAPI* EFI_SET_WAKEUP_TIME)(
	IN  BOOLEAN                      Enable,
	IN  EFI_TIME* Time   OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_UPDATE_CAPSULE)(
	IN EFI_CAPSULE_HEADER** CapsuleHeaderArray,
	IN UINTN                  CapsuleCount,
	IN EFI_PHYSICAL_ADDRESS   ScatterGatherList   OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_QUERY_CAPSULE_CAPABILITIES)(
	IN  EFI_CAPSULE_HEADER** CapsuleHeaderArray,
	IN  UINTN                  CapsuleCount,
	OUT UINT64* MaximumCapsuleSize,
	OUT EFI_RESET_TYPE* ResetType
	);

typedef
EFI_STATUS
(EFIAPI* EFI_QUERY_VARIABLE_INFO)(
	IN  UINT32            Attributes,
	OUT UINT64* MaximumVariableStorageSize,
	OUT UINT64* RemainingVariableStorageSize,
	OUT UINT64* MaximumVariableSize
	);


//*******************************************************
// SIMPLE_TEXT_OUTPUT_MODE
//*******************************************************
struct SIMPLE_TEXT_OUTPUT_MODE {
	INT32 MaxMode;
	// current settings
	INT32 Mode;
	INT32 Attribute;
	INT32 CursorColumn;
	INT32 CursorRow;
	BOOLEAN CursorVisible;
};

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	EFI_TEXT_RESET                  Reset;

	EFI_TEXT_STRING          OutputString;
	EFI_TEXT_TEST_STRING            TestString;

	EFI_TEXT_QUERY_MODE             QueryMode;
	EFI_TEXT_SET_MODE               SetMode;
	EFI_TEXT_SET_ATTRIBUTE          SetAttribute;

	EFI_TEXT_CLEAR_SCREEN           ClearScreen;
	EFI_TEXT_SET_CURSOR_POSITION    SetCursorPosition;
	EFI_TEXT_ENABLE_CURSOR          EnableCursor;

	// Current mode
	SIMPLE_TEXT_OUTPUT_MODE         *Mode;
};

typedef struct {
	EFI_TABLE_HEADER                Hdr;

	//
	// Time Services
	//
	EFI_GET_TIME                    GetTime;
	EFI_SET_TIME                    SetTime;
	EFI_GET_WAKEUP_TIME             GetWakeupTime;
	EFI_SET_WAKEUP_TIME             SetWakeupTime;

	//
	// Virtual Memory Services
	//
	EFI_SET_VIRTUAL_ADDRESS_MAP     SetVirtualAddressMap;
	EFI_CONVERT_POINTER             ConvertPointer;

	//
	// Variable Services
	//
	EFI_GET_VARIABLE                GetVariable;
	EFI_GET_NEXT_VARIABLE_NAME      GetNextVariableName;
	EFI_SET_VARIABLE                SetVariable;

	//
	// Miscellaneous Services
	//
	EFI_GET_NEXT_HIGH_MONO_COUNT    GetNextHighMonotonicCount;
	EFI_RESET_SYSTEM                ResetSystem;

	//
	// UEFI 2.0 Capsule Services
	//
	EFI_UPDATE_CAPSULE              UpdateCapsule;
	EFI_QUERY_CAPSULE_CAPABILITIES  QueryCapsuleCapabilities;

	//
	// Miscellaneous UEFI 2.0 Service
	//
	EFI_QUERY_VARIABLE_INFO         QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

struct EFI_SYSTEM_TABLE {
	EFI_TABLE_HEADER                Hdr;

	CHAR16* FirmwareVendor;
	UINT32  FirmwareRevision;

	EFI_HANDLE ConsoleInHandle;
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;

	EFI_HANDLE ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*ConOut;

	EFI_HANDLE StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;

	EFI_RUNTIME_SERVICES* RuntimeServices;
	struct EFI_BOOT_SERVICES*    BootServices;

	UINTN NumberOfTableEntries;
	struct EFI_CONFIGURATION_TABLE *ConfigurationTable;

};
