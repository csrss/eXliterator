#ifndef _DEBUGLOG_H
#define _DEBUGLOG_H
#pragma warning(disable: 4996)

class DebugLog {
protected:
	wchar_t *DebugFileLogName;
private:
/*
#ifndef walloc
	#define walloc(Size) (wchar_t*)HeapAlloc(GetProcessHeap(), \
						HEAP_ZERO_MEMORY, sizeof (wchar_t*) * Size + 1)
#endif
#ifndef ufree
	#define ufree(Addr)	HeapFree(GetProcessHeap(), 0, (LPVOID)Addr)
#endif
*/ 
	wchar_t * GetDebugFileName(void){
		LARGE_INTEGER Large;
		NTSTATUS Status;
		DWORD Length;

		if(Status = NtQuerySystemTime(&Large) != STATUS_SUCCESS) return NULL;
		Length = (DWORD)RtlGetCurrentDirectory_U(0, NULL);
		if(!Length) return NULL;
		wchar_t *CurDir = (wchar_t*)malloc(sizeof(wchar_t*) * Length + MAX_PATH);
		RtlGetCurrentDirectory_U(Length, CurDir);
		int output_len = sizeof(wchar_t*) * wcslen(CurDir) + MAX_PATH;
		wchar_t *output = (wchar_t*)malloc(output_len);

		swprintf(output, output_len, L"%s\\%I64d_DEBUGLOG.txt", 
						CurDir, Large.QuadPart);
		return output;
	}

	long LogDebugInformation(__in wchar_t *DebugTexString, 
									   __in wchar_t *FileName)
	{
		HANDLE hFile;
		NTSTATUS Status;
		UNICODE_STRING PathNameString;
		IO_STATUS_BLOCK    ioStatusBlock;
		OBJECT_ATTRIBUTES ObjectAttributes;
		LARGE_INTEGER   fileSize;
		fileSize.QuadPart = 0;
		int lpPathName_len = sizeof(wchar_t *) * wcslen(FileName) + MAX_PATH;
		wchar_t *lpPathName = (wchar_t*)malloc(lpPathName_len);
		swprintf(lpPathName, lpPathName_len, L"\\??\\%s", FileName);
		RtlInitUnicodeString(&PathNameString, lpPathName);
		InitializeObjectAttributes( &ObjectAttributes, 
									&PathNameString, 
									OBJ_CASE_INSENSITIVE, 
									NULL, 
									NULL );
	
		Status = NtCreateFile(	&hFile,
								SYNCHRONIZE | FILE_APPEND_DATA,
								&ObjectAttributes,
								&ioStatusBlock,
								&fileSize,
								FILE_ATTRIBUTE_NORMAL,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								FILE_OPEN_IF,
								FILE_SYNCHRONOUS_IO_NONALERT,
								NULL,
								0);

		if(Status != STATUS_SUCCESS)return Status;
	
		Status = NtWriteFile(hFile,
							 NULL, 
							 NULL, 
							 NULL, 
							 &ioStatusBlock, 
							 DebugTexString, 
							 (wcslen(DebugTexString) * sizeof(wchar_t*)), 
							 NULL, 
							 NULL);

		if(Status != STATUS_SUCCESS)return Status;

		NtClose(hFile);
		free(lpPathName);
		return 0;
	}

public:
	DebugLog(BOOL ToFile){
		if(ToFile)DebugFileLogName = GetDebugFileName();
		else DebugFileLogName = NULL;
	}
	~DebugLog(void){
		DebugFileLogName = NULL;
	}

	int dprint(BOOL ToFile, wchar_t *String, ...){
		va_list ap;
		int Status;
		long St;
		wchar_t debug[MAX_PATH] = {0};
		wchar_t MSG[1024] = {0};
		va_start(ap, String);
		Status = _vsnwprintf(MSG, sizeof MSG, String, ap);
		if(!wcsstr(MSG, L"\r\n")) wcscat(MSG, L"\r\n");
		if(Status != -1){
			OutputDebugStringW(MSG);
			if(ToFile){
				if(!DebugFileLogName) DebugFileLogName = GetDebugFileName();
					St = LogDebugInformation(MSG, DebugFileLogName);
				if(St != 0){
					dprint(FALSE, L"Cannot log into file! Status: 0x%08x", St);
				}
			}
		}
		va_end(ap);
		return Status;
	}

};

#endif