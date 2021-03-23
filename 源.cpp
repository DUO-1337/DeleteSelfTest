#include <stdio.h>
#include <Windows.h>
#include <winternl.h>
#include <shlwapi.h>

typedef struct _FILE_RENAME_INFORMATION {
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_RS1)
    union {
        BOOLEAN ReplaceIfExists;  // FileRenameInformation
        ULONG Flags;              // FileRenameInformationEx
    } DUMMYUNIONNAME;
#else
    BOOLEAN ReplaceIfExists;
#endif
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[5];
} FILE_RENAME_INFORMATION, * PFILE_RENAME_INFORMATION;

typedef struct _FILE_DISPOSITION_INFORMATION {
	BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, * PFILE_DISPOSITION_INFORMATION;

typedef NTSTATUS(__stdcall* NtSetInformationFile)(
    HANDLE                 FileHandle,
    PIO_STATUS_BLOCK       IoStatusBlock,
    PVOID                  FileInformation,
    ULONG                  Length,
    FILE_INFORMATION_CLASS FileInformationClass
);


int main()
{
	HANDLE fileHandler;
	OBJECT_ATTRIBUTES objectAttr;
	UNICODE_STRING filename = { 0 };

	WCHAR selfPath[0x400] = { 0 };
	WCHAR sourceString[0x400] = { 0 };
	GetModuleFileNameW(NULL, selfPath, 0x400);
	wnsprintfW(sourceString, 0x3FF, L"%s%s", L"\\??\\", selfPath);

	RtlInitUnicodeString(&filename, sourceString);
	InitializeObjectAttributes(&objectAttr, &filename, OBJ_CASE_INSENSITIVE, NULL, NULL);
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS status = NtCreateFile(&fileHandler, DELETE | SYNCHRONIZE | GENERIC_READ, &objectAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_DELETE,
		FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	if (status == 0)
	{
		puts("OK");
	}

	HMODULE ntdll = LoadLibraryA("ntdll.dll");
	NtSetInformationFile ntSetInformationFile = (NtSetInformationFile)GetProcAddress(ntdll, "NtSetInformationFile");

	IO_STATUS_BLOCK bs = { 0 };
    FILE_RENAME_INFORMATION fi = { FALSE, NULL, 8, L":wtf" };

	status = ntSetInformationFile(fileHandler, &bs, &fi, sizeof(FILE_RENAME_INFORMATION), (FILE_INFORMATION_CLASS)10);
	
	if (status == 0)
	{
		puts("OK");
	}

	HANDLE fileHandler2;
	OBJECT_ATTRIBUTES objectAttr2;
	UNICODE_STRING filename2 = { 0 };
	RtlInitUnicodeString(&filename2, sourceString);
	InitializeObjectAttributes(&objectAttr2, &filename2, OBJ_CASE_INSENSITIVE, NULL, NULL);
	IO_STATUS_BLOCK ioStatusBlock2;
	status = NtCreateFile(&fileHandler2, DELETE | SYNCHRONIZE | GENERIC_READ, &objectAttr2, &ioStatusBlock2, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_DELETE,
		FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	if (status == 0)
	{
		puts("OK");
	}

	IO_STATUS_BLOCK bs2 = { 0 };
	FILE_DISPOSITION_INFORMATION fi2 = { TRUE };

	status = ntSetInformationFile(fileHandler2, &bs2, &fi2, sizeof(FILE_RENAME_INFORMATION), (FILE_INFORMATION_CLASS)13);

	if (status == 0)
	{
		puts("OK");
	}

	NtClose(fileHandler);
	NtClose(fileHandler2);

	int i = 0;
	while (true)
	{
		printf("%d\n", i++);
	}
	return 0;
}