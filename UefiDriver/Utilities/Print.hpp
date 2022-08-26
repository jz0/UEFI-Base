#pragma once
#include "../Include.hpp"

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#define va_copy(d,s)	__builtin_va_copy(d,s)

#define EFI_BLACK   0x00
#define EFI_BLUE    0x01
#define EFI_GREEN   0x02
#define EFI_CYAN            (EFI_BLUE | EFI_GREEN)
#define EFI_RED     0x04
#define EFI_MAGENTA         (EFI_BLUE | EFI_RED)
#define EFI_BROWN           (EFI_GREEN | EFI_RED)
#define EFI_LIGHTGRAY       (EFI_BLUE | EFI_GREEN | EFI_RED)
#define EFI_BRIGHT  0x08
#define EFI_DARKGRAY        (EFI_BRIGHT)
#define EFI_LIGHTBLUE       (EFI_BLUE | EFI_BRIGHT)
#define EFI_LIGHTGREEN      (EFI_GREEN | EFI_BRIGHT)
#define EFI_LIGHTCYAN       (EFI_CYAN | EFI_BRIGHT)
#define EFI_LIGHTRED        (EFI_RED | EFI_BRIGHT)
#define EFI_LIGHTMAGENTA    (EFI_MAGENTA | EFI_BRIGHT)
#define EFI_YELLOW          (EFI_BROWN | EFI_BRIGHT)
#define EFI_WHITE           (EFI_BLUE | EFI_GREEN | EFI_RED | EFI_BRIGHT)
#define EFI_TEXT_ATTR(f,b)  ((f) | ((b) << 4))

#define PRINT_STRING_LEN            200
#define PRINT_ITEM_BUFFER_LEN       100

typedef struct {
	BOOLEAN             Ascii;
	UINTN               Index;
	union {
		const CHAR16* pw;
		const CHAR8* pc;
	} un;
} POINTER;

typedef struct _pstate {
	// Input
	POINTER     fmt;
	va_list     args;

	// Output
	CHAR16* Buffer;
	CHAR16* End;
	CHAR16* Pos;
	UINTN       Len;

	UINTN       Attr;
	UINTN       RestoreAttr;

	UINTN       AttrNorm;
	UINTN       AttrHighlight;
	UINTN       AttrError;

	INTN(EFIAPI* Output)(VOID* context, CHAR16* str);
	INTN(EFIAPI* SetAttr)(VOID* context, UINTN attr);
	VOID* Context;

	// Current item being formatted
	struct _pitem* Item;
} PRINT_STATE;

typedef struct _pitem {

	POINTER     Item;
	CHAR16      Scratch[PRINT_ITEM_BUFFER_LEN];
	UINTN       Width;
	UINTN       FieldWidth;
	UINTN* WidthParse;
	CHAR16      Pad;
	BOOLEAN     PadBefore;
	BOOLEAN     Comma;
	BOOLEAN     Long;
} PRINT_ITEM;

namespace Console
{
	UINTN Print(IN const char16_t* fmt, ...);
}