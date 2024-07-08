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
void *
ds_rename_handle(
	HANDLE hHandle
)
{
	LPCWSTR lpwStream = DS_STREAM_RENAME;
	PFILE_RENAME_INFO pfRename = (PFILE_RENAME_INFO)malloc(sizeof(FILE_RENAME_INFO) + sizeof(WCHAR) * wcslen(lpwStream)); // FILE_RENAME_INFO contains space for 1 WCHAR without NULL-byte
	if(pfRename == NULL)
	{
		DS_DEBUG_LOG(L"could not allocate memory");
		return NULL;
	}
	RtlSecureZeroMemory(pfRename, sizeof(FILE_RENAME_INFO) + sizeof(WCHAR) * wcslen(lpwStream));

	// set our FileNameLength and FileName to DS_STREAM_RENAME
	pfRename->FileNameLength = (DWORD)(sizeof(WCHAR) * wcslen(lpwStream));
	RtlCopyMemory(pfRename->FileName, lpwStream, sizeof(WCHAR) * (wcslen(lpwStream) + 1));

	BOOL fRenameOk = SetFileInformationByHandle(hHandle, FileRenameInfo, pfRename, (DWORD)(sizeof(FILE_RENAME_INFO) + sizeof(WCHAR) * wcslen(lpwStream)));
	if(!fRenameOk)
	{
		free(pfRename);
		return NULL;
	}
	return pfRename;
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
	void *pfRename = ds_rename_handle(hCurrent);
	if (pfRename == NULL)
	{
		DS_DEBUG_LOG(L"failed to rename to stream");
		return 0;
	}

	DS_DEBUG_LOG(L"successfully renamed file primary :$DATA ADS to specified stream, closing initial handle");
	CloseHandle(hCurrent);
	free(pfRename); // free memory allocated in ds_rename_handle
	pfRename = NULL;

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
