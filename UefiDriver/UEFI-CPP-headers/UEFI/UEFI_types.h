#pragma once

#include "basic_types.h"

#define EFIAPI __attribute__((ms_abi))

#ifndef EFIAPI // Forces EFI calling conventions regardless of compiler options 
#ifdef _MSC_VER
#define EFIAPI __cdecl  // Force C calling convention for Microsoft C compiler 
#elif defined(HAVE_USE_MS_ABI)
// Force amd64/ms calling conventions.
#define EFIAPI __attribute__((ms_abi))
#else
#error Unknown compiler. Define EFIAPI so that it forced "cdecl" calling convention.
#endif
#endif

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL        0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL       0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER           0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE           0x00000020
typedef UINTN RETURN_STATUS;
#define EFI_ERROR(StatusCode)  (((INTN)(RETURN_STATUS)(StatusCode)) < 0)

using EFI_HANDLE = void*;
using EFI_EVENT = void*;

enum BOOLEAN : UINT8
{
	FALSE = 0,
	TRUE = 1
};

enum EFI_STATUS : UINTN
{
	EFI_SUCCESS = 0,
	EFI_LOAD_ERROR = 1,
	EFI_INVALID_PARAMETER = 2,
	EFI_UNSUPPORTED = 3,
	EFI_BAD_BUFFER_SIZE = 4,
	EFI_BUFFER_TOO_SMALL = 5,
	EFI_NOT_READY = 6,
	EFI_DEVICE_ERROR = 7,
	EFI_WRITE_PROTECTED,
	EFI_OUT_OF_RESOURCES,
	EFI_VOLUME_CORRUPTED,
	EFI_VOLUME_FULL,
	EFI_NO_MEDIA,
	EFI_MEDIA_CHANGED,
	EFI_NOT_FOUND,
	EFI_ACCESS_DENIED,
	EFI_NO_RESPONSE,
	EFI_NO_MAPPING,
	EFI_TIMEOUT,
	EFI_NOT_STARTED,
	EFI_ALREADY_STARTED,
	EFI_ABORTED,
	EFI_ICMP_ERROR,
	EFI_TFTP_ERROR,
	EFI_PROTOCOL_ERROR,
	EFI_INCOMPATIBLE_VERSION,
	EFI_SECURITY_VIOLATION,
	EFI_CRC_ERROR,
	EFI_END_OF_MEDIA = 28,
	EFI_END_OF_FILE = 31,
	EFI_INVALID_LANGUAGE = 32,
	EFI_COMPROMISED_DATA = 33,
	EFI_HTTP_ERROR = 35
};

// Type modifiers defined in the UEFI standard
#define IN
#define OUT
#define OPTIONAL

struct EFI_TABLE_HEADER {
	UINT64 Signature;
	UINT32 Revision;
	UINT32 HeaderSize;
	UINT32 CRC32;
private:
	UINT32 Reserved;
};

struct EFI_GUID {
	UINT32  Data1;
	UINT16  Data2;
	UINT16  Data3;
	UINT8   Data4[8];
};

typedef UINTN EFI_TPL; // Task priority level