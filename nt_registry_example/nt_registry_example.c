#include "nt_registry_example.h"

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
    // considering there is regisry hive name Computer\HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\LogMisc
    wchar_t* regsubtreename = (wchar_t*)L"LogMisc"; 
    wchar_t* regsubtreevaluename = (wchar_t*)L"SampleVal"; //value name to be created in LongMisc
    UNICODE_STRING valuedata; 
    RtlInitUnicodeString(&valuedata, L"This is data."); // Data to be written


    ULONG data = 0xFF; // The DWORD data you want to write.
    WriteDWordToReg(L"TestVal", data);
   

    WriteMultiStringToRegistry(RTL_REGISTRY_CONTROL, L"LogMisc", regsubtreevaluename,
        valuedata.Buffer, valuedata.Length,
        L"ErrorCode");

    NtTerminateProcess(NtCurrentProcess(), 0);
}

