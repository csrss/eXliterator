#ifndef _NTAPI_H
#define _NTAPI_H

typedef  LONG NTSTATUS;
#define  NtCurrentProcess()		           ((HANDLE) -1)
#define  STATUS_SUCCESS                    ((NTSTATUS)0x00000000L)
#define  STATUS_UNSUCCESSFUL			   ((NTSTATUS)0xC0000001L)
#define  OBJ_CASE_INSENSITIVE			   0x00000040L
#define  RTL_REGISTRY_HANDLE               0x40000000
#define  FILE_OPEN_IF					   0x00000003
#define  FILE_SYNCHRONOUS_IO_NONALERT	   0x00000020
#define  NTABSOLUTE(wait)				   (wait)
#define  NTRELATIVE(wait)				   (-(wait))
#define  NANOSECONDS(nanos)				   (((signed __int64)(nanos)) / 100L)
#define  MICROSECONDS(micros)			   (((signed __int64)(micros)) * NANOSECONDS(1000L))
#define  MILLISECONDS(milli)			   (((signed __int64)(milli)) * MICROSECONDS(1000L))
#define  SECONDS(seconds)				   (((signed __int64)(seconds)) * MILLISECONDS(1000L))

typedef struct _CLIENT_ID{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
    PCHAR Buffer;
} STRING, *PSTRING;

typedef STRING *PSTRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef enum {
     AdjustCurrentProcess,
     AdjustCurrentThread
} ADJUST_PRIVILEGE_TYPE;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

typedef enum _KEY_VALUE_INFORMATION_CLASS {
  KeyValueBasicInformation,
  KeyValueFullInformation,
  KeyValuePartialInformation
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
  ULONG  TitleIndex;
  ULONG    Type;
  ULONG    NameLength;
  WCHAR  Name[1];  //  Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
  ULONG  TitleIndex;
  ULONG  Type;
  ULONG  DataLength;
  UCHAR  Data[1];  //  Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
  ULONG  TitleIndex;
  ULONG  Type;
  ULONG  DataOffset;
  ULONG  DataLength;
  ULONG  NameLength;
  WCHAR  Name[1];  //  Variable size
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _IO_STATUS_BLOCK
{
     union
     {
          LONG Status;
          PVOID Pointer;
     };
     ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef void (*PIO_APC_ROUTINE)	(PVOID				ApcContext,
                                 PIO_STATUS_BLOCK	IoStatusBlock,
                                 ULONG				Reserved);
NTSTATUS (NTAPI *NtTerminateProcess)(IN HANDLE               ProcessHandle OPTIONAL,
									 IN NTSTATUS             ExitStatus );
VOID (NTAPI *RtlInitUnicodeString)(PUNICODE_STRING DestinationString,
								   PCWSTR SourceString);
NTSTATUS (NTAPI *NtOpenKey)(OUT PHANDLE             pKeyHandle,
							IN ACCESS_MASK          DesiredAccess,
							IN POBJECT_ATTRIBUTES   ObjectAttributes );
NTSTATUS (NTAPI *NtQueryValueKey)(IN HANDLE               KeyHandle,
								 IN PUNICODE_STRING      ValueName,
								 IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
								 OUT PVOID               KeyValueInformation,
								 IN ULONG                Length,
								 OUT PULONG              ResultLength);
NTSTATUS (NTAPI *RtlDeleteRegistryValue)(IN ULONG  RelativeTo,
										 IN PCWSTR Path,
										 IN PCWSTR ValueName);
NTSTATUS (NTAPI *NtClose)(HANDLE ObjectHandle);
NTSTATUS (NTAPI *NtCreateKey)(OUT PHANDLE pKeyHandle, 
							  IN ACCESS_MASK DesiredAccess, 
							  IN POBJECT_ATTRIBUTES ObjectAttributes, 
							  IN ULONG TitleIndex, 
							  IN PUNICODE_STRING Class OPTIONAL, 
							  IN ULONG CreateOptions, 
							  OUT PULONG Disposition OPTIONAL ); 
NTSTATUS (NTAPI *NtSetValueKey)(IN HANDLE KeyHandle, 
								IN PUNICODE_STRING ValueName, 
								IN ULONG TitleIndex OPTIONAL, 
								IN ULONG Type, 
								IN PVOID Data, 
								IN ULONG DataSize );
NTSTATUS (NTAPI *NtQuerySystemTime)(OUT PLARGE_INTEGER      SystemTime);
NTSTATUS (NTAPI *RtlGetCurrentDirectory_U)(ULONG buflen, LPWSTR buf);
NTSTATUS (NTAPI *NtCreateFile)(OUT PHANDLE             FileHandle,
							   IN ACCESS_MASK          DesiredAccess,
							   IN POBJECT_ATTRIBUTES   ObjectAttributes,
							   OUT PIO_STATUS_BLOCK    IoStatusBlock,
							   IN PLARGE_INTEGER       AllocationSize OPTIONAL,
							   IN ULONG                FileAttributes,
							   IN ULONG                ShareAccess,
							   IN ULONG                CreateDisposition,
							   IN ULONG                CreateOptions,
							   IN PVOID                EaBuffer OPTIONAL,
							   IN ULONG                EaLength);
NTSTATUS (NTAPI *NtWriteFile)(	IN HANDLE               FileHandle,
								IN HANDLE               Event OPTIONAL,
								IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
								IN PVOID                ApcContext OPTIONAL,
								OUT PIO_STATUS_BLOCK    IoStatusBlock,
								IN PVOID                Buffer,
								IN ULONG                Length,
								IN PLARGE_INTEGER       ByteOffset OPTIONAL,
								IN PULONG               Key OPTIONAL );
NTSTATUS (NTAPI *RtlCreateUserThread)(IN HANDLE ProcessHandle,
									  IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
									  IN BOOLEAN CreateSuspended,
									  IN ULONG StackZeroBits OPTIONAL,
									  IN SIZE_T StackReserve OPTIONAL,
									  IN SIZE_T StackCommit OPTIONAL,
									  IN LPTHREAD_START_ROUTINE StartAddress,
									  IN PVOID Parameter  OPTIONAL,
									  OUT PHANDLE ThreadHandle OPTIONAL,
									  OUT PCLIENT_ID ClientId OPTIONAL );
NTSTATUS (NTAPI *NtQueryInformationProcess)(HANDLE ProcessHandle,
											int ProcessInformationClass,
											PVOID ProcessInformation,
											ULONG ProcessInformationLength,
											PULONG ReturnLength);
NTSTATUS (NTAPI *NtSetDebugFilterState)(ULONG ComponentId, ULONG Level, BOOLEAN State);
NTSTATUS (NTAPI *NtDeviceIoControlFile)(IN HANDLE               FileHandle,
										IN HANDLE               Event OPTIONAL,
										IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
										IN PVOID                ApcContext OPTIONAL,
										OUT PIO_STATUS_BLOCK    IoStatusBlock,
										IN ULONG                IoControlCode,
										IN PVOID                InputBuffer OPTIONAL,
										IN ULONG                InputBufferLength,
										OUT PVOID               OutputBuffer OPTIONAL,
										IN ULONG                OutputBufferLength);
NTSTATUS (NTAPI *NtTerminateThread)(IN HANDLE               ThreadHandle,
									IN NTSTATUS             ExitStatus);
NTSTATUS (NTAPI *NtWaitForSingleObject)(HANDLE Handle, 
										BOOLEAN Alertable, 
										PLARGE_INTEGER Timeout);
NTSTATUS (NTAPI *NtOpenProcess)(PHANDLE ProcessHandle, 
								ACCESS_MASK AccessMask, 
								POBJECT_ATTRIBUTES ObjectAttributes, 
								PCLIENT_ID ClientId);
NTSTATUS (NTAPI *RtlAdjustPrivilege)(int,BOOL,BOOL,BOOL *);
NTSTATUS (NTAPI *NtDelayExecution)(IN BOOLEAN Alertable, IN PLARGE_INTEGER DelayInterval);
BOOL (NTAPI *RtlFreeHeap)(PVOID HeapHandle, ULONG Flags, PVOID MemoryPointer);
PVOID (NTAPI *RtlAllocateHeap)(PVOID HeapHandle, ULONG Flags, ULONG Size);
PVOID (NTAPI *RtlReAllocateHeap)(PVOID HeapHandle, ULONG Flags, PVOID MemoryPointer, ULONG Size);
NTSTATUS (NTAPI *NtQuerySystemInformation)(__in       unsigned int SystemInformationClass,
										   __inout    PVOID SystemInformation,
										   __in       ULONG SystemInformationLength,
										   __out_opt  PULONG ReturnLength);

BOOL InitializeNTAPI(){
	HMODULE hObsolete       = GetModuleHandleW(L"ntdll.dll");
	*(FARPROC *)&NtTerminateProcess	= GetProcAddress(hObsolete, "NtTerminateProcess");
	*(FARPROC *)&RtlInitUnicodeString	= GetProcAddress(hObsolete, "RtlInitUnicodeString");
	*(FARPROC *)&NtOpenKey	= GetProcAddress(hObsolete, "NtOpenKey");
	*(FARPROC *)&NtQueryValueKey	= GetProcAddress(hObsolete, "NtQueryValueKey");
	*(FARPROC *)&RtlDeleteRegistryValue	= GetProcAddress(hObsolete, "RtlDeleteRegistryValue");
	*(FARPROC *)&NtClose	= GetProcAddress(hObsolete, "NtClose");
	*(FARPROC *)&NtCreateKey	= GetProcAddress(hObsolete, "NtCreateKey");
	*(FARPROC *)&NtSetValueKey	= GetProcAddress(hObsolete, "NtSetValueKey");
	*(FARPROC *)&NtQuerySystemTime	= GetProcAddress(hObsolete, "NtQuerySystemTime");
	*(FARPROC *)&RtlGetCurrentDirectory_U	= GetProcAddress(hObsolete, "RtlGetCurrentDirectory_U");
	*(FARPROC *)&NtCreateFile	= GetProcAddress(hObsolete, "NtCreateFile");
	*(FARPROC *)&NtWriteFile	= GetProcAddress(hObsolete, "NtWriteFile");
	*(FARPROC *)&RtlCreateUserThread	= GetProcAddress(hObsolete, "RtlCreateUserThread");
	*(FARPROC *)&NtQueryInformationProcess	= GetProcAddress(hObsolete, "NtQueryInformationProcess");
	*(FARPROC *)&NtSetDebugFilterState	= GetProcAddress(hObsolete, "NtSetDebugFilterState");
	*(FARPROC *)&NtDeviceIoControlFile	= GetProcAddress(hObsolete, "NtDeviceIoControlFile");
	*(FARPROC *)&NtTerminateThread	= GetProcAddress(hObsolete, "NtTerminateThread");
	*(FARPROC *)&NtWaitForSingleObject	= GetProcAddress(hObsolete, "NtWaitForSingleObject");
	*(FARPROC *)&NtOpenProcess	= GetProcAddress(hObsolete, "NtOpenProcess");
	*(FARPROC *)&RtlAdjustPrivilege	= GetProcAddress(hObsolete, "RtlAdjustPrivilege");
	*(FARPROC *)&NtDelayExecution	= GetProcAddress(hObsolete, "NtDelayExecution");
	*(FARPROC *)&RtlFreeHeap	= GetProcAddress(hObsolete, "RtlFreeHeap");
	*(FARPROC *)&RtlAllocateHeap	= GetProcAddress(hObsolete, "RtlAllocateHeap");
	*(FARPROC *)&RtlReAllocateHeap	= GetProcAddress(hObsolete, "RtlReAllocateHeap");
	*(FARPROC *)&NtQuerySystemInformation	= GetProcAddress(hObsolete, "NtQuerySystemInformation");
	if(NtTerminateProcess && RtlInitUnicodeString && NtOpenKey &&
		NtQueryValueKey && RtlDeleteRegistryValue && NtClose &&
		NtCreateKey && NtSetValueKey && NtQuerySystemTime &&
		RtlGetCurrentDirectory_U && NtCreateFile && NtWriteFile && 
		RtlCreateUserThread && NtQueryInformationProcess && 
		NtSetDebugFilterState && NtDeviceIoControlFile && 
		NtTerminateThread && NtWaitForSingleObject && NtOpenProcess &&
		RtlAdjustPrivilege && NtDelayExecution && RtlFreeHeap && 
		RtlAllocateHeap && RtlReAllocateHeap && NtQuerySystemInformation) return TRUE;
	return FALSE;
}

unsigned int wtoi(wchar_t *n){
		register unsigned int ret = 0;
        while ((((*n) < '0') || ((*n) > '9')) && (*n))
                n++;
        while ((*n) >= '0' && (*n) <= '9')
                ret = ret * 10 + (*n++) - '0';
        return ret;
}

NTSTATUS NtSleep(BOOL Alertable, DWORD Seconds){
	LARGE_INTEGER Interval;
	NTSTATUS Status;
	Interval.QuadPart = NTRELATIVE(SECONDS(Seconds));
	Status = NtDelayExecution (Alertable, &Interval);
	if(Status != STATUS_SUCCESS){
		return Status;
	}
	return STATUS_SUCCESS;
}

__declspec(naked) void * __stdcall AsmHEAP(){
	__asm {
		mov eax, dword ptr fs:[00000018h]
		mov eax, dword ptr [eax+30h]
		mov eax, dword ptr [eax+18h]
		ret
	}
}

void * __stdcall NtMalloc(__in BOOL IsRealloc, 
						  __in BOOL IsFree, 
						  __in unsigned long size, 
						  __in void *MemoryPointer)
{
	if (IsFree) return (void *)RtlFreeHeap(GetProcessHeap(), 0, MemoryPointer);
	else if (IsRealloc) return (void *)RtlReAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, MemoryPointer, size);
	else return (void *)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

#endif