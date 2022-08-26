#include "Include.hpp"
EFI_GUID EfiLoadedImageProtocolGuid = { 0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } };
EFI_GUID VirtualGuid = { 0x851a820a, 0x1239, 0x436c, { 0x94, 0xc6, 0xc3, 0xbf, 0x86, 0xb4, 0xc7, 0xef } };
EFI_EVENT NotifyEvent = NULL;

EFI_STATUS UefiUnload(IN EFI_HANDLE ImageHandle)
{
	//We want to keep the driver loaded
	return EFI_ACCESS_DENIED;
}

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable)
{
	GlobalSystemTable = SystemTable;
	GlobalRuntimeServices = SystemTable->RuntimeServices;
	GlobalBootServices = SystemTable->BootServices;

	EFI_LOADED_IMAGE* LoadedImage = NULL;
	EFI_STATUS Status = GlobalBootServices->OpenProtocol(ImageHandle, &EfiLoadedImageProtocolGuid, (void**)&LoadedImage, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if (EFI_ERROR(Status))
	{
		Console::Print(Crypt(u"[-] Failed to open protocol! \n"));
		return Status;
	}

	LoadedImage->Unload = (EFI_IMAGE_UNLOAD)UefiUnload;

	//Hooks here...

	//Hook ExitBootServices :)
	ExitBootServicesOriginal = GlobalBootServices->ExitBootServices;
	GlobalBootServices->ExitBootServices = HookedExitBootServices;

	Console::Print(Crypt(u"[+] Loaded \n"));

	//Please don't dump my driver during runtime
	Utilities::ErasePE(LoadedImage);
	return EFI_SUCCESS;
}