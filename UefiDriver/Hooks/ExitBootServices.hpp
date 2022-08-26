#include "../Include.hpp"

extern EFI_EXIT_BOOT_SERVICES ExitBootServicesOriginal;
EFI_STATUS EFIAPI HookedExitBootServices(EFI_HANDLE ImageHandle, UINTN MapKey);