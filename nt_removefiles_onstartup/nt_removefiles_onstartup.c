#include "nt_removefiles_onstartup.h"

void WriteLn(LPWSTR Message)
{
    UNICODE_STRING string;
    RtlInitUnicodeString(&string, Message);

    NtDisplayString(&string);
}

void WriteDWordToReg(wchar_t* valuename, ULONG data)
{
    NTSTATUS status;

    status = RtlWriteRegistryValue(
        RTL_REGISTRY_CONTROL, L"LogMisc",
        valuename,
        REG_DWORD,
        &data,
        sizeof(ULONG)
    );

    if (!NT_SUCCESS(status)) {
        // Handle the error.

        RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
            L"LogMisc", L"DwordWrite", REG_MULTI_SZ,
            L"Failed", 6 * sizeof(wchar_t));

        //WriteNTStatusToRegistry(L"CraeteFileErr", status);
    }
}

void WriteMultiStringToRegistry(ULONG base,wchar_t* path,wchar_t *valuename, 
        PVOID data, ULONG len, wchar_t* fail_valuename)
{
    NTSTATUS status;
    status = RtlWriteRegistryValue(base,
        path, valuename, REG_MULTI_SZ,
        data, len);
    if (status == STATUS_SUCCESS)
    {
    }
    else
    {

        WriteDWordToReg(fail_valuename, status);
    }
}

void NtProcessStartup(IN PPEB peb)
{
    // it is important to declare all variables at the beginning

    NTSTATUS status;
    UNICODE_STRING regEntry;
    RtlInitUnicodeString(&regEntry, L"\\??\\C:\\Windows\\System32\\myapp.exe\r\n\\??\\C:\\Users\\default\\AppData\\Local\\Temp\\myapp.exe\r\n");

    ULONG data1 = 0xFF; // The data you want to write.
    status = RtlWriteRegistryValue(
        RTL_REGISTRY_CONTROL, L"Session Manager", L"AllowProtectedRenames ",
        REG_DWORD,
        &data1,
        sizeof(ULONG)
    );

    WriteDWordToReg(L"APRFailed", status);


    status = RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
        L"LogAkhil", L"PendingFileRenameOperations", REG_MULTI_SZ,
        regEntry.Buffer, regEntry.Length + 2);

    WriteDWordToReg(L"PFROStatus", status);

    NtTerminateProcess(NtCurrentProcess(), 0);
}

