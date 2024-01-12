// create_native+process.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>
#include "ntdll.h"
#pragma comment(lib, "ntdll")

/*Alternative function for starting process but NTSTATUS of NtCreateUserProcess is */
int start_cmd()
{
	// Path to the image file from which the process will be created
	UNICODE_STRING NtImagePath, Params, ImagePath;
	RtlInitUnicodeString(&ImagePath, (PWSTR)L"\\??\\C:\\Windows\\System32\\cmd.exe");

	RtlInitUnicodeString(&NtImagePath, (PWSTR)L"\\??\\C:\\Windows\\System32\\cmd.exe");
	RtlInitUnicodeString(&Params, (PWSTR)L"\"C:\\WINDOWS\\SYSTEM32\\cmd.exe\" /k echo Hello world!");
	// Create the process parameters
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters = NULL;
	RtlCreateProcessParametersEx(&ProcessParameters, &ImagePath, NULL, NULL, &Params, NULL, NULL, NULL, NULL, NULL, RTL_USER_PROCESS_PARAMETERS_NORMALIZED);

	// Initialize the PS_CREATE_INFO structure
	PS_CREATE_INFO CreateInfo = { 0 };
	CreateInfo.Size = sizeof(CreateInfo);
	CreateInfo.State = PsCreateInitialState;

	//Skip Image File Execution Options debugger
	CreateInfo.InitState.u1.InitFlags = 0x04;

	OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
	PPS_STD_HANDLE_INFO stdHandleInfo = (PPS_STD_HANDLE_INFO)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PS_STD_HANDLE_INFO));
	PCLIENT_ID clientId = (PCLIENT_ID)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PS_ATTRIBUTE));
	PSECTION_IMAGE_INFORMATION SecImgInfo = (PSECTION_IMAGE_INFORMATION)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SECTION_IMAGE_INFORMATION));
	PPS_ATTRIBUTE_LIST AttributeList = (PS_ATTRIBUTE_LIST*)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PS_ATTRIBUTE_LIST));

	// Create necessary attributes
	AttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST);
	AttributeList->Attributes[0].Attribute = PS_ATTRIBUTE_CLIENT_ID;
	AttributeList->Attributes[0].Size = sizeof(CLIENT_ID);
	AttributeList->Attributes[0].ValuePtr = clientId;

	AttributeList->Attributes[1].Attribute = PS_ATTRIBUTE_IMAGE_INFO;
	AttributeList->Attributes[1].Size = sizeof(SECTION_IMAGE_INFORMATION);
	AttributeList->Attributes[1].ValuePtr = SecImgInfo;

	AttributeList->Attributes[2].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
	AttributeList->Attributes[2].Size = NtImagePath.Length;
	AttributeList->Attributes[2].ValuePtr = NtImagePath.Buffer;

	AttributeList->Attributes[3].Attribute = PS_ATTRIBUTE_STD_HANDLE_INFO;
	AttributeList->Attributes[3].Size = sizeof(PS_STD_HANDLE_INFO);
	AttributeList->Attributes[3].ValuePtr = stdHandleInfo;

	DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;

	// Add process mitigation attribute
	AttributeList->Attributes[4].Attribute = PS_ATTRIBUTE_MITIGATION_OPTIONS;
	AttributeList->Attributes[4].Size = sizeof(DWORD64);
	AttributeList->Attributes[4].ValuePtr = &policy;

	// Spoof Parent Process Id as explorer.exe
	DWORD trayPID;
	HWND trayWnd = FindWindowW(L"Shell_TrayWnd", NULL);
	GetWindowThreadProcessId(trayWnd, &trayPID);
	HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, false, trayPID);
	if (hParent)
	{
		AttributeList->Attributes[5].Attribute = PS_ATTRIBUTE_PARENT_PROCESS;
		AttributeList->Attributes[5].Size = sizeof(HANDLE);
		AttributeList->Attributes[5].ValuePtr = hParent;
	}
	else
	{
		AttributeList->TotalLength -= sizeof(PS_ATTRIBUTE);
	}
	// Create the process
	HANDLE hProcess = NULL, hThread = NULL;
	NTSTATUS nstatus = NtCreateUserProcess(&hProcess, &hThread, MAXIMUM_ALLOWED, MAXIMUM_ALLOWED, &objAttr, &objAttr, 0, 0, ProcessParameters, &CreateInfo, AttributeList);

	// Clean up
	if (hParent) CloseHandle(hParent);
	RtlFreeHeap(RtlProcessHeap(), 0, AttributeList);
	RtlFreeHeap(RtlProcessHeap(), 0, stdHandleInfo);
	RtlFreeHeap(RtlProcessHeap(), 0, clientId);
	RtlFreeHeap(RtlProcessHeap(), 0, SecImgInfo);
	RtlDestroyProcessParameters(ProcessParameters);

	return 0;
}

int main()
{
	// Path to the image file from which the process will be created NtCreateUserProcessis return STATUS_SUCCESS
	//seems some command promt windows getting started, crashed immediately. I am not sure whether it is even starting.
	
	/*
	
	DeleteMe.bat contents :
	
	echo "Hello, $name"

	echo Script Executed >> \\??\\D:\\Shared\\scriptlog.txt
	echo Script Executed1 >> D:\\Shared\\scriptlog.txt
	echo Script Executed2 >> \\??\\\\D:\\Shared\\scriptlog.txt
	call echo Script Executed2 >> scriptlog.txt
	pause >nul
	
*/
	if (1)
	{
		/*Actively trying solution for starting process. */
		UNICODE_STRING NtImagePath;
		RtlInitUnicodeString(&NtImagePath, (PWSTR)L"\\??\\\\C:\\Windows\\System32\\cmd.exe");
		UNICODE_STRING NtCmd;
		//RtlInitUnicodeString(&NtCmd, (PWSTR)L"/k \\??\\D:\\Shared\\DeleteMe.bat");
		RtlInitUnicodeString(&NtCmd, (PWSTR)L" \\??\\\\C:\\Windows\\System32\\cmd.exe /C \\??\\D:\\Shared\\DeleteMe.bat");

		// Create the process parameters
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters = NULL;
		RtlCreateProcessParametersEx(&ProcessParameters, &NtImagePath, NULL, NULL, &NtCmd, NULL, NULL, NULL, NULL, NULL, RTL_USER_PROCESS_PARAMETERS_NORMALIZED);

		// Initialize the PS_CREATE_INFO structure
		PS_CREATE_INFO CreateInfo = { 0 };
		CreateInfo.Size = sizeof(CreateInfo);
		CreateInfo.State = PsCreateInitialState;

		// Initialize the PS_ATTRIBUTE_LIST structure
		PPS_ATTRIBUTE_LIST AttributeList = (PS_ATTRIBUTE_LIST*)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PS_ATTRIBUTE));
		AttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE);
		AttributeList->Attributes[0].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
		AttributeList->Attributes[0].Size = NtImagePath.Length;
		AttributeList->Attributes[0].Value = (ULONG_PTR)NtImagePath.Buffer;

		// Create the process
		HANDLE hProcess, hThread = NULL;
		NTSTATUS status = NtCreateUserProcess(&hProcess, &hThread, PROCESS_ALL_ACCESS, THREAD_ALL_ACCESS, NULL, NULL, NULL, NULL, ProcessParameters, &CreateInfo, AttributeList);

		// Clean up
		RtlFreeHeap(RtlProcessHeap(), 0, AttributeList);
		RtlDestroyProcessParameters(ProcessParameters);
	}
	else
		start_cmd();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
