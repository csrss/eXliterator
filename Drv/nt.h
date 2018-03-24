#ifndef __NT_H__
#define __NT_H__

#pragma warning(disable: 4996)

#define MEGABYTE 1024*1024
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES     16
#define IMAGE_SIZEOF_SECTION_HEADER          40
#define IMAGE_SIZEOF_SHORT_NAME              8
#define IMAGE_SUBSYSTEM_NATIVE               1
#define IMAGE_DIRECTORY_ENTRY_EXPORT         0
#define IMAGE_DIRECTORY_ENTRY_BASERELOC      5
#define IMAGE_REL_BASED_HIGHLOW              3
#define IMAGE_DIRECTORY_ENTRY_IMPORT         1

#define DLL_PROCESS_ATTACH 1

typedef unsigned char BYTE;
typedef BYTE * PBYTE;
typedef ULONG DWORD;
typedef USHORT WORD;
typedef PVOID HMODULE;
typedef PVOID FARPROC;

typedef struct _SYSTEM_THREAD_INFORMATION {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	ULONG  Priority;
	LONG BasePriority;
	ULONG ContextSwitches;
	ULONG ThreadState;
	ULONG WaitReason;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG			NextEntryDelta;
	ULONG			ThreadCount;
	ULONG			Reserved1[6];
	LARGE_INTEGER   CreateTime;
	LARGE_INTEGER   UserTime;
	LARGE_INTEGER   KernelTime;
	UNICODE_STRING  ProcessName;
	ULONG   	    BasePriority;
	ULONG			ProcessId;
	ULONG			InheritedFromProcessId;
	ULONG			HandleCount;
	ULONG			Reserved2[2];
	VM_COUNTERS	    VmCounters;
	IO_COUNTERS	    IoCounters;
	SYSTEM_THREAD_INFORMATION  Threads[5];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef struct _QUERY_DIRECTORY {
	ULONG Length;
	PUNICODE_STRING FileName;
	FILE_INFORMATION_CLASS FileInformationClass;
	ULONG FileIndex;
} QUERY_DIRECTORY, *PQUERY_DIRECTORY;

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG   Reserved[2];
    PVOID   Base;
    ULONG   Size;
    ULONG   Flags;
    USHORT  Index;
    USHORT  Unknown;
    USHORT  LoadCount;
    USHORT  ModuleNameOffset;
    CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemNotImplemented1,
    SystemProcessesAndThreadsInformation,
    SystemCallCounts,
    SystemConfigurationInformation,
    SystemProcessorTimes,
    SystemGlobalFlag,
    SystemNotImplemented2,
    SystemModuleInformation,
    SystemLockInformation,
    SystemNotImplemented3,
    SystemNotImplemented4,
    SystemNotImplemented5,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPagefileInformation,
    SystemInstructionEmulationCounts,
    SystemInvalidInfoClass1,
    SystemCacheInformation,
    SystemPoolTagInformation,
    SystemProcessorStatistics,
    SystemDpcInformation,
    SystemNotImplemented6,
    SystemLoadImage,
    SystemUnloadImage,
    SystemTimeAdjustment,
    SystemNotImplemented7,
    SystemNotImplemented8,
    SystemNotImplemented9,
    SystemCrashDumpInformation,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemLoadAndCallImage,
    SystemPrioritySeparation,
    SystemNotImplemented10,
    SystemNotImplemented11,
    SystemInvalidInfoClass2,
    SystemInvalidInfoClass3,
    SystemTimeZoneInformation,
    SystemLookasideInformation,
    SystemSetTimeSlipEvent,
    SystemCreateSession,
    SystemDeleteSession,
    SystemInvalidInfoClass4,
    SystemRangeStartInformation,
    SystemVerifierInformation,
    SystemAddVerifier,
    SystemSessionProcessesInformation
} SYSTEM_INFORMATION_CLASS;

typedef struct _IMAGE_DATA_DIRECTORY {
	DWORD   VirtualAddress;
	DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
	WORD   e_magic;                     // Magic number
	WORD   e_cblp;                      // Bytes on last page of file
	WORD   e_cp;                        // Pages in file
	WORD   e_crlc;                      // Relocations
	WORD   e_cparhdr;                   // Size of header in paragraphs
	WORD   e_minalloc;                  // Minimum extra paragraphs needed
	WORD   e_maxalloc;                  // Maximum extra paragraphs needed
	WORD   e_ss;                        // Initial (relative) SS value
	WORD   e_sp;                        // Initial SP value
	WORD   e_csum;                      // Checksum
	WORD   e_ip;                        // Initial IP value
	WORD   e_cs;                        // Initial (relative) CS value
	WORD   e_lfarlc;                    // File address of relocation table
	WORD   e_ovno;                      // Overlay number
	WORD   e_res[4];                    // Reserved words
	WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
	WORD   e_oeminfo;                   // OEM information; e_oemid specific
	WORD   e_res2[10];                  // Reserved words
	LONG   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER {
	//
	// Standard fields.
	//

	WORD    Magic;
	BYTE    MajorLinkerVersion;
	BYTE    MinorLinkerVersion;
	DWORD   SizeOfCode;
	DWORD   SizeOfInitializedData;
	DWORD   SizeOfUninitializedData;
	DWORD   AddressOfEntryPoint;
	DWORD   BaseOfCode;
	DWORD   BaseOfData;

	//
	// NT additional fields.
	//

	DWORD   ImageBase;
	DWORD   SectionAlignment;
	DWORD   FileAlignment;
	WORD    MajorOperatingSystemVersion;
	WORD    MinorOperatingSystemVersion;
	WORD    MajorImageVersion;
	WORD    MinorImageVersion;
	WORD    MajorSubsystemVersion;
	WORD    MinorSubsystemVersion;
	DWORD   Win32VersionValue;
	DWORD   SizeOfImage;
	DWORD   SizeOfHeaders;
	DWORD   CheckSum;
	WORD    Subsystem;
	WORD    DllCharacteristics;
	DWORD   SizeOfStackReserve;
	DWORD   SizeOfStackCommit;
	DWORD   SizeOfHeapReserve;
	DWORD   SizeOfHeapCommit;
	DWORD   LoaderFlags;
	DWORD   NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	WORD    Machine;
	WORD    NumberOfSections;
	DWORD   TimeDateStamp;
	DWORD   PointerToSymbolTable;
	DWORD   NumberOfSymbols;
	WORD    SizeOfOptionalHeader;
	WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_SECTION_HEADER {
	BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc;
	DWORD   VirtualAddress;
	DWORD   SizeOfRawData;
	DWORD   PointerToRawData;
	DWORD   PointerToRelocations;
	DWORD   PointerToLinenumbers;
	WORD    NumberOfRelocations;
	WORD    NumberOfLinenumbers;
	DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_NT_HEADERS {
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name;
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;     // RVA from base of image
	DWORD   AddressOfNames;         // RVA from base of image
	DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_FIXUP_ENTRY
{
	USHORT Offset:12;
	USHORT Type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;

typedef struct _IMAGE_BASE_RELOCATION {
	DWORD   VirtualAddress;
	DWORD   SizeOfBlock;
	//  WORD    TypeOffset[1];
} IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
	union {
		DWORD   Characteristics;            // 0 for terminating null import descriptor
		DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	};
	DWORD   TimeDateStamp;                  // 0 if not bound,
	// -1 if bound, and real date\time stamp
	//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)

	DWORD   ForwarderChain;                 // -1 if no forwarders
	DWORD   Name;
	DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_THUNK_DATA32 {
	union {
		DWORD ForwarderString;      // PBYTE 
		DWORD Function;             // PDWORD
		DWORD Ordinal;
		DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
	} u1;
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD    Hint;
	BYTE    Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS    ExitStatus;
    PVOID       TebBaseAddress;
    ULONG       UniqueProcessId;
    ULONG       UniqueThreadId;
    KAFFINITY   AffinityMask;
    KPRIORITY   BasePriority;
    ULONG       DiffProcessPriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

extern "C" {	// all functions applies to minimum supported client: Windows XP; server: Windows Server 2003

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation (
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID                   SystemInformation,
    IN ULONG                    Length,
    OUT PULONG                  ReturnLength);

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationThread (
    IN HANDLE           ThreadHandle,
    IN THREADINFOCLASS  ThreadInformationClass,
    OUT PVOID           ThreadInformation,
    IN ULONG            ThreadInformationLength,
    OUT PULONG          ReturnLength OPTIONAL);

};

static ULONG GetNTDLLBase()
{
	ULONG BufSize = MEGABYTE, RetLen, NumberOfModules, i, ntbase = 0;
	PSYSTEM_MODULE_INFORMATION SysInfo;
	PVOID Buffer = ExAllocatePool(NonPagedPool,BufSize);
	if (NT_SUCCESS(ZwQuerySystemInformation(SystemModuleInformation,Buffer,BufSize,&RetLen)))
	{
		NumberOfModules = *(PULONG)Buffer;
		SysInfo = (PSYSTEM_MODULE_INFORMATION)((DWORD_PTR)Buffer+4);

		for (i=0;i<NumberOfModules;i++)
		{
			if (SysInfo[i].ImageName)
			{
				LPCTSTR fname = (LPCTSTR)strrchr(SysInfo[i].ImageName,'\\');
				if (fname)
				{
					if (!_strnicmp(fname+1,"ntdll.",6))
					{
						ntbase = (ULONG)SysInfo[i].Base;
						break;
					}
				}
			}
		}
	}

	ExFreePool(Buffer);
	return ntbase;
}

static ULONG GetExportEntry(IN ULONG Base,IN LPCTSTR ProcName)
{
	PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)Base;
	PIMAGE_NT_HEADERS nh = (PIMAGE_NT_HEADERS)((DWORD_PTR)Base+dh->e_lfanew);
	PIMAGE_OPTIONAL_HEADER oh = &nh->OptionalHeader;
	PIMAGE_EXPORT_DIRECTORY ed = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)Base+oh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	LPCTSTR * Names = (LPCTSTR*)((DWORD_PTR)Base+ed->AddressOfNames);
	PUSHORT Ords = (PUSHORT)((DWORD_PTR)Base+ed->AddressOfNameOrdinals);
	PULONG Entries = (PULONG)((DWORD_PTR)Base+ed->AddressOfFunctions); 

	for (ULONG i=0;i<ed->NumberOfNames;i++)
	{
		if (!strcmp((LPCSTR)((DWORD_PTR)Base+Names[i]),ProcName))
			return Base+Entries[Ords[i]];
	}

	return 0;
}

static ULONG GetSyscallService(ULONG NtDllServiceEntry)
{
	return (*(PULONG)((DWORD_PTR)NtDllServiceEntry+1));
}

static BOOLEAN IsThreadRunning(IN HANDLE hThread)
{
	THREAD_BASIC_INFORMATION ThreadInfo;
	ULONG RetLen;

	NTSTATUS st = ZwQueryInformationThread(hThread,ThreadBasicInformation,&ThreadInfo,sizeof(ThreadInfo),&RetLen);
	if (NT_SUCCESS(st)){
		return (ThreadInfo.ExitStatus == STATUS_PENDING);
	} else {
		DbgPrint("Unable to ZwQueryInfoThread (status %08x)\n", st);
	}
	return FALSE;
}

static __declspec(naked) void __cdecl FastSystemCall() {
	__asm {
		lea edx, dword ptr ss:[esp+8]
		int 0x2e
		ret
	}
}

static __declspec(naked) NTSTATUS NTAPI syscallNtQueueApcThread(...) {
	__asm {
		mov eax, dword ptr [dwNtQueueApcThreadService]
		call FastSystemCall
		retn
	}
}

static void NTAPI CallUserModeAPC(IN int data){
	if (gCurrentSinkThreadHandle){
		if (IsThreadRunning(gCurrentSinkThreadHandle)){
			DbgPrint("[+] Sink thread running... [APC at %08x]\n",gCurrentSinkApcRoutine);
			syscallNtQueueApcThread(gCurrentSinkThreadHandle, gCurrentSinkApcRoutine, data, 0, 0);
		} else {
			DbgPrint("[-] Sink thread not running...\n");
			ZwClose(gCurrentSinkThreadHandle);
			gCurrentSinkThreadHandle = 0;
		}
	} else {
		DbgPrint("No thread handle available!\n");
	}
}

static void NTAPI GetIrqlLevel(char *FunctionName){
	KIRQL CurrentIrql;
	CurrentIrql = KeGetCurrentIrql();
	if(CurrentIrql == PASSIVE_LEVEL)DbgPrint("[*] %s level: PASSIVE_LEVEL\n", FunctionName);
	else if(CurrentIrql == APC_LEVEL)DbgPrint("[*] %s level: APC_LEVEL\n", FunctionName);
	else if(CurrentIrql == DISPATCH_LEVEL)DbgPrint("[*] %s level: DISPATCH_LEVEL\n", FunctionName);
}

#endif 