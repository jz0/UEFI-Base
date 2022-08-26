#include "Print.hpp"

#ifndef Release
UINT64
RShiftU64(
	IN UINT64   Operand,
	IN UINTN    Count
)
{
	return Operand >> Count;
}

UINT64
DivU64x32(
	IN UINT64   Dividend,
	IN UINTN    Divisor,
	OUT UINTN* Remainder OPTIONAL
)
{
	if (Remainder) {
		*Remainder = Dividend % Divisor;
	}

	return Dividend / Divisor;
}

VOID
ValueToString(
	IN CHAR16* Buffer,
	IN BOOLEAN  Comma,
	IN INT64    v
)
{
	static CHAR8 ca[] = { 3, 1, 2 };
	CHAR8        str[40], * p1;
	CHAR16* p2;
	UINTN        c, r;

	if (!v) {
		Buffer[0] = '0';
		Buffer[1] = 0;
		return;
	}

	p1 = str;
	p2 = Buffer;

	if (v < 0) {
		*(p2++) = '-';
		v = -v;
	}

	while (v) {
		v = (INT64)DivU64x32((UINT64)v, 10, &r);
		*(p1++) = (CHAR8)r + '0';
	}

	c = (UINTN)(Comma ? ca[(p1 - str) % 3] : 999) + 1;
	while (p1 != str) {

		c -= 1;
		if (!c) {
			*(p2++) = ',';
			c = 3;
		}

		*(p2++) = *(--p1);
	}
	*p2 = 0;
}

static CHAR8 Hex[] = { '0','1','2','3','4','5','6','7',
					  '8','9','A','B','C','D','E','F' };
VOID
ValueToHex(
	IN CHAR16* Buffer,
	IN UINT64   v
)
{
	CHAR8           str[30], * p1;
	CHAR16* p2;

	if (!v) {
		Buffer[0] = '0';
		Buffer[1] = 0;
		return;
	}

	p1 = str;
	p2 = Buffer;

	while (v) {
		// Without the cast, the MSVC compiler may insert a reference to __allmull
		*(p1++) = Hex[(UINTN)(v & 0xf)];
		v = RShiftU64(v, 4);
	}

	while (p1 != str) {
		*(p2++) = *(--p1);
	}
	*p2 = 0;
}

static
VOID
PFLUSH(
	IN OUT PRINT_STATE* ps
)
{
	*ps->Pos = 0;
	//if (IsLocalPrint(ps->Output))
	//    ps->Output(ps->Context, ps->Buffer);
	//else
	ps->Output(ps->Context, ps->Buffer);
	ps->Pos = ps->Buffer;
}

static
VOID
PSETATTR(
	IN OUT PRINT_STATE* ps,
	IN UINTN             Attr
)
{
	PFLUSH(ps);

	ps->RestoreAttr = ps->Attr;
	if (ps->SetAttr) {
		ps->SetAttr(ps->Context, Attr);
	}

	ps->Attr = Attr;
}

static
VOID
PPUTC(
	IN OUT PRINT_STATE* ps,
	IN CHAR16              c
)
{
	// if this is a newline, add a carraige return
	if (c == '\n') {
		PPUTC(ps, '\r');
	}

	*ps->Pos = c;
	ps->Pos += 1;
	ps->Len += 1;

	// if at the end of the buffer, flush it
	if (ps->Pos >= ps->End) {
		PFLUSH(ps);
	}
}

static
CHAR16
PGETC(
	IN POINTER* p
)
{
	CHAR16      c;

	c = p->Ascii ? p->un.pc[p->Index] : p->un.pw[p->Index];
	p->Index += 1;

	return  c;
}

static
VOID
PITEM(
	IN OUT PRINT_STATE* ps
)
{
	UINTN               Len, i;
	PRINT_ITEM* Item;
	CHAR16              c;

	// Get the length of the item
	Item = ps->Item;
	Item->Item.Index = 0;
	while (Item->Item.Index < Item->FieldWidth) {
		c = PGETC(&Item->Item);
		if (!c) {
			Item->Item.Index -= 1;
			break;
		}
	}
	Len = Item->Item.Index;

	// if there is no item field width, use the items width
	if (Item->FieldWidth == (UINTN)-1) {
		Item->FieldWidth = Len;
	}

	// if item is larger then width, update width
	if (Len > Item->Width) {
		Item->Width = Len;
	}


	// if pad field before, add pad char
	if (Item->PadBefore) {
		for (i = Item->Width; i < Item->FieldWidth; i += 1) {
			PPUTC(ps, ' ');
		}
	}

	// pad item
	for (i = Len; i < Item->Width; i++) {
		PPUTC(ps, Item->Pad);
	}

	// add the item
	Item->Item.Index = 0;
	while (Item->Item.Index < Len) {
		PPUTC(ps, PGETC(&Item->Item));
	}

	// If pad at the end, add pad char
	if (!Item->PadBefore) {
		for (i = Item->Width; i < Item->FieldWidth; i += 1) {
			PPUTC(ps, ' ');
		}
	}
}

static
UINTN
_Print(
	IN PRINT_STATE* ps
)
/*++
Routine Description:
	%w.lF   -   w = width
				l = field width
				F = format of arg
  Args F:
	0       -   pad with zeros
	-       -   justify on left (default is on right)
	,       -   add comma's to field
	*       -   width provided on stack
	n       -   Set output attribute to normal (for this field only)
	h       -   Set output attribute to highlight (for this field only)
	e       -   Set output attribute to error (for this field only)
	l       -   Value is 64 bits
	a       -   ascii string
	s       -   unicode string
	X       -   fixed 8 byte value in hex
	x       -   hex value
	d       -   value as signed decimal
	u       -   value as unsigned decimal
	f       -   value as floating point
	c       -   Unicode char
	t       -   EFI time structure
	g       -   Pointer to GUID
	r       -   EFI status code (result code)
	D       -   pointer to Device Path with normal ending.
	N       -   Set output attribute to normal
	H       -   Set output attribute to highlight
	E       -   Set output attribute to error
	%       -   Print a %
Arguments:
	SystemTable     - The system table
Returns:
	Number of charactors written
--*/
{
	CHAR16          c;
	UINTN           Attr;
	PRINT_ITEM      Item;
	CHAR16          Buffer[PRINT_STRING_LEN];

	ps->Len = 0;
	ps->Buffer = Buffer;
	ps->Pos = Buffer;
	ps->End = Buffer + PRINT_STRING_LEN - 1;
	ps->Item = &Item;

	ps->fmt.Index = 0;
	while ((c = PGETC(&ps->fmt))) {

		if (c != '%') {
			PPUTC(ps, c);
			continue;
		}

		// setup for new item
		Item.FieldWidth = (UINTN)-1;
		Item.Width = 0;
		Item.WidthParse = &Item.Width;
		Item.Pad = ' ';
		Item.PadBefore = TRUE;
		Item.Comma = FALSE;
		Item.Long = FALSE;
		Item.Item.Ascii = FALSE;
		Item.Item.un.pw = NULL;
		ps->RestoreAttr = 0;
		Attr = 0;

		while ((c = PGETC(&ps->fmt))) {

			switch (c) {

			case '%':
				//
				// %% -> %
				//
				Item.Scratch[0] = '%';
				Item.Scratch[1] = 0;
				Item.Item.un.pw = Item.Scratch;
				break;

			case '0':
				Item.Pad = '0';
				break;

			case '-':
				Item.PadBefore = FALSE;
				break;

			case ',':
				Item.Comma = TRUE;
				break;

			case '.':
				Item.WidthParse = &Item.FieldWidth;
				break;

			case '*':
				*Item.WidthParse = va_arg(ps->args, UINTN);
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				*Item.WidthParse = 0;
				do {
					*Item.WidthParse = *Item.WidthParse * 10 + c - '0';
					c = PGETC(&ps->fmt);
				} while (c >= '0' && c <= '9');
				ps->fmt.Index -= 1;
				break;

			case 'a':
				Item.Item.un.pc = va_arg(ps->args, CHAR8*);
				Item.Item.Ascii = TRUE;
				if (!Item.Item.un.pc) {
					Item.Item.un.pc = (CHAR8*)Crypt("(null)");
				}
				break;

				//case 's':
				//	Item.Item.un.pw = va_arg(ps->args, CHAR16*);
				//	if (!Item.Item.un.pw) {
				//		Item.Item.un.pw = L"(null)";
				//	}
				//	break;

			case 'c':
				Item.Scratch[0] = (CHAR16)va_arg(ps->args, UINTN);
				Item.Scratch[1] = 0;
				Item.Item.un.pw = Item.Scratch;
				break;

			case 'l':
				Item.Long = TRUE;
				break;

			case 'X':
				Item.Width = Item.Long ? 16 : 8;
				Item.Pad = '0';
#if __GNUC__ >= 7
				__attribute__((fallthrough));
#endif
			case 'x':
				ValueToHex(
					Item.Scratch,
					Item.Long ? va_arg(ps->args, UINT64) : va_arg(ps->args, UINT32)
				);
				Item.Item.un.pw = Item.Scratch;

				break;


				//case 'g':
				   // GuidToString(Item.Scratch, va_arg(ps->args, EFI_GUID*));
				   // Item.Item.un.pw = Item.Scratch;
				   // break;

			case 'u':
				ValueToString(
					Item.Scratch,
					Item.Comma,
					Item.Long ? va_arg(ps->args, UINT64) : va_arg(ps->args, UINT32)
				);
				Item.Item.un.pw = Item.Scratch;
				break;

			case 'd':
				ValueToString(
					Item.Scratch,
					Item.Comma,
					Item.Long ? va_arg(ps->args, INT64) : va_arg(ps->args, INT32)
				);
				Item.Item.un.pw = Item.Scratch;
				break;

				//case 'D':
				//{
				   // EFI_DEVICE_PATH* dp = va_arg(ps->args, EFI_DEVICE_PATH*);
				   // CHAR16* dpstr = DevicePathToStr(dp);
				   // StrnCpy(Item.Scratch, dpstr, PRINT_ITEM_BUFFER_LEN);
				   // Item.Scratch[PRINT_ITEM_BUFFER_LEN - 1] = L'\0';
				   // FreePool(dpstr);

				   // Item.Item.un.pw = Item.Scratch;
				   // break;
				//}

				//case 'f':
				   // FloatToString(
					  //  Item.Scratch,
					  //  Item.Comma,
					  //  va_arg(ps->args, double)
				   // );
				   // Item.Item.un.pw = Item.Scratch;
				   // break;

				//case 't':
				   // TimeToString(Item.Scratch, va_arg(ps->args, EFI_TIME*));
				   // Item.Item.un.pw = Item.Scratch;
				   // break;

				//case 'r':
				   // StatusToString(Item.Scratch, va_arg(ps->args, EFI_STATUS));
				   // Item.Item.un.pw = Item.Scratch;
				   // break;

			case 'n':
				PSETATTR(ps, ps->AttrNorm);
				break;

			case 'h':
				PSETATTR(ps, ps->AttrHighlight);
				break;

			case 'e':
				PSETATTR(ps, ps->AttrError);
				break;

			case 'N':
				Attr = ps->AttrNorm;
				break;

			case 'H':
				Attr = ps->AttrHighlight;
				break;

			case 'E':
				Attr = ps->AttrError;
				break;

			default:
				Item.Scratch[0] = '?';
				Item.Scratch[1] = 0;
				Item.Item.un.pw = Item.Scratch;
				break;
			}

			// if we have an Item
			if (Item.Item.un.pw) {
				PITEM(ps);
				break;
			}

			// if we have an Attr set
			if (Attr) {
				PSETATTR(ps, Attr);
				ps->RestoreAttr = 0;
				break;
			}
		}

		if (ps->RestoreAttr) {
			PSETATTR(ps, ps->RestoreAttr);
		}
	}

	// Flush buffer
	PFLUSH(ps);
	return ps->Len;
}


UINTN
_IPrint(
	IN UINTN                            Column,
	IN UINTN                            Row,
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Out,
	IN const CHAR16* fmt,
	IN const CHAR8* fmta,
	IN va_list                          args
)
// Display string worker for: Print, PrintAt, IPrint, IPrintAt
{
	PRINT_STATE     ps;
	UINTN            back;

	Utilities::ZeroMem(&ps, sizeof(ps));
	ps.Context = Out;
	ps.Output = (INTN(EFIAPI*)(VOID*, CHAR16*)) Out->OutputString;
	ps.SetAttr = (INTN(EFIAPI*)(VOID*, UINTN))  Out->SetAttribute;
	ps.Attr = Out->Mode->Attribute;

	back = (ps.Attr >> 4) & 0xF;
	ps.AttrNorm = EFI_TEXT_ATTR(EFI_LIGHTGRAY, back);
	ps.AttrHighlight = EFI_TEXT_ATTR(EFI_WHITE, back);
	ps.AttrError = EFI_TEXT_ATTR(EFI_YELLOW, back);

	if (fmt) {
		ps.fmt.un.pw = fmt;
	}
	else {
		ps.fmt.Ascii = TRUE;
		ps.fmt.un.pc = fmta;
	}

	va_copy(ps.args, args);

	if (Column != (UINTN)-1) {
		Out->SetCursorPosition(Out, Column, Row);
	}

	back = _Print(&ps);
	va_end(ps.args);
	return back;
}

#endif

UINTN
Console::Print(
	IN const char16_t* fmt,
	...
)
/*++
Routine Description:
	Prints a formatted unicode string to the default console
Arguments:
	fmt         - Format string
Returns:
	Length of string printed to the console
--*/
{
#ifndef Release
	va_list     args;
	UINTN       back;

	va_start(args, fmt);
	back = _IPrint((UINTN)-1, (UINTN)-1, GlobalSystemTable->ConOut, (CHAR16*)fmt, NULL, args);
	va_end(args);
	return back;
#endif
}

