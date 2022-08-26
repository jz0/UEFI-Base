#pragma once

#include <stdint.h>

#if defined _M_AMD64 
typedef int64_t INTN; // Signed value of native width. (4 bytes on supported 32-bit processor instructions, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)
typedef uint64_t UINTN; // Unsigned value of native width. (4 bytes on supported 32-bit processor instructions, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)
#elif defined _M_IX86
typedef int32_t INTN; // Signed value of native width. (4 bytes on supported 32-bit processor instructions, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)
typedef uint32_t UINTN; // Unsigned value of native width. (4 bytes on supported 32-bit processor instructions, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)
#else
#error Unknown platform, add missing typdefs to support it.
#endif

using UINT8 = uint8_t;
using INT8 = int8_t;
using UINT16 = uint16_t;
using INT16 = int16_t;
using UINT32 = uint32_t;
using INT32 = int32_t;
using UINT64 = uint64_t;
using INT64 = int64_t;
using CHAR8 = char;

using VOID = void;

using CHAR16 = uint16_t;

typedef uint8_t ;
typedef uint16_t CHAR16;