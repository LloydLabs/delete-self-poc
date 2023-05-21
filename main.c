#include "main.h"

static
HANDLE
ds_open_handle(
	PWCHAR pwPath
)
{
	return CreateFileW(pwPath, DELETE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static
BOOL
ds_rename_handle(
	HANDLE hHandle
)
{
	// set our FileNameLength and FileName to DS_STREAM_RENAME
	int rename_info_size = sizeof(FILE_RENAME_INFO) + sizeof(DS_STREAM_RENAME) - 1;

	PFILE_RENAME_INFO fRename = (PFILE_RENAME_INFO)malloc(rename_info_size);
	if (!fRename)
		return 0;

	LPWSTR lpwStream = DS_STREAM_RENAME;
	fRename->FileNameLength = sizeof(DS_STREAM_RENAME);
	RtlCopyMemory(fRename->FileName, lpwStream, sizeof(DS_STREAM_RENAME));

	BOOL status = SetFileInformationByHandle(hHandle, FileRenameInfo, fRename, rename_info_size);
	free(fRename);
	return status;
}

static
BOOL 
ds_deposite_handle(
	HANDLE hHandle
)
{
	// set FILE_DISPOSITION_INFO::DeleteFile to TRUE
	FILE_DISPOSITION_INFO fDelete;
	RtlSecureZeroMemory(&fDelete, sizeof(fDelete));

	fDelete.DeleteFile = TRUE;

	return SetFileInformationByHandle(hHandle, FileDispositionInfo, &fDelete, sizeof(fDelete));
}

int
main(
	int argc,
	char** argv
)
{
	WCHAR wcPath[MAX_PATH + 1];
	RtlSecureZeroMemory(wcPath, sizeof(wcPath));

	// get the path to the current running process ctx
	if (GetModuleFileNameW(NULL, wcPath, MAX_PATH) == 0)
	{
		DS_DEBUG_LOG(L"failed to get the current module handle");
		return 0;
	}

	HANDLE hCurrent = ds_open_handle(wcPath);
	if (hCurrent == INVALID_HANDLE_VALUE)
	{
		DS_DEBUG_LOG(L"failed to acquire handle to current running process");
		return 0;
	}

	// rename the associated HANDLE's file name
	DS_DEBUG_LOG(L"attempting to rename file name");
	if (!ds_rename_handle(hCurrent))
	{
		DS_DEBUG_LOG(L"failed to rename to stream");
		return 0;
	}

	DS_DEBUG_LOG(L"successfully renamed file primary :$DATA ADS to specified stream, closing initial handle");
	CloseHandle(hCurrent);

	// open another handle, trigger deletion on close
	hCurrent = ds_open_handle(wcPath);
	if (hCurrent == INVALID_HANDLE_VALUE)
	{
		DS_DEBUG_LOG(L"failed to reopen current module");
		return 0;
	}

	if (!ds_deposite_handle(hCurrent))
	{
		DS_DEBUG_LOG(L"failed to set delete deposition");
		return 0;
	}

	// trigger the deletion deposition on hCurrent
	DS_DEBUG_LOG(L"closing handle to trigger deletion deposition");
	CloseHandle(hCurrent);

	// verify we've been deleted
	if (PathFileExistsW(wcPath))
	{
		DS_DEBUG_LOG(L"failed to delete copy, file still exists");
		return 0;
	}

	DS_DEBUG_LOG(L"successfully deleted self from disk");
	return 1;
}
