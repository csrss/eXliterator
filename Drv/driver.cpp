#include <ntddk.h>
#include <ntddkbd.h>
#include <stdio.h>
#include "keys.h"

#define FILE_DEVICE_CTRL2CAP	0x00008300
#define NTDEVICE_NAME_STRING	L"\\Device\\Transliterator"
#define SYMBOLIC_NAME_STRING	L"\\DosDevices\\Transliterator"
#define IOCTL_FETCH_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x01, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)
typedef HANDLE HWND;
typedef unsigned int UINT;
typedef unsigned int WPARAM;
typedef long LPARAM;
#define WM_USER 0x0400
#define WM_CLOSE 0x0010
#define WM_NOTIFY_GUI (WM_USER + 0x0010)

extern "C" PUSHORT NtBuildNumber;
PDEVICE_OBJECT gKbdHookDevice = NULL;
LONG g_number_of_pending_IRPs = 0;
PDEVICE_OBJECT ControlDeviceObject = NULL;
BOOLEAN g_sniff_keys = TRUE;
static HANDLE gCurrentSinkThreadHandle = 0;
static PVOID gCurrentSinkApcRoutine = 0;
static ULONG dwNtQueueApcThreadService = 0;
unsigned short LAST_CONTROL = 0x00;
static unsigned long nNtUserPostMessage = 0x11da;
HWND hWindow;
PKEVENT KernelEvent;
HANDLE UserEvent;
BOOLEAN BackSpaced = FALSE;

static unsigned long syscall_nr;
#define SYSCALL_STATUS_INVALID_FUNCTION_ADDRESS	0x00001D2E
#define SYSCALL_STATUS_PLATFORM_NOT_SUPPORTED	0x00001D3E

__declspec(naked) NTSTATUS SYSTEM_CALL(...)
{
	__asm {
		cmp		syscall_nr, 000h
		jz		failed
		mov		eax, syscall_nr
		lea		edx, [esp+004h]
		int		02Eh
		retn	00014h
   
		failed:
		mov		eax, SYSCALL_STATUS_INVALID_FUNCTION_ADDRESS		// syscall status failure
		retn	00014h
	}
 
}

NTSTATUS NtUserPostMessage( HWND hWnd,
						    UINT Msg,
							WPARAM wParam,
							LPARAM lParam)
{
	NTSTATUS Status;
	ULONG gMajOSVersion, gMinOSVersion, BuildNumber;
	PsGetVersion(&gMajOSVersion, &gMinOSVersion, &BuildNumber, NULL);

	switch (BuildNumber){
		case 2195: syscall_nr = 0x11cb; break;		// windows 2000
		case 2600: syscall_nr = 0x11db; break;		// windows XP
		case 3790: syscall_nr = 0x11da; break;		// windows server 2003
		case 6000: syscall_nr = 0x11f1; break;		// vista 
		case 6001: syscall_nr = 0x11f1; break;		// vista SP1 and server 2008
		case 6002: syscall_nr = 0x11f1; break;		// vista SP2 and server 2008 SP2
		case 7600: syscall_nr = 0x11fc; break;		// windows 7
		default: syscall_nr = 0x0000; break;		// unrecognized windows is not supported
	} 

	Status = SYSTEM_CALL(hWnd, Msg, wParam, lParam);
	if(Status == SYSCALL_STATUS_INVALID_FUNCTION_ADDRESS){
		return SYSCALL_STATUS_PLATFORM_NOT_SUPPORTED;
	}
	return Status;
}

#include "nt.h"
#include "driver.h"

VOID OnUnload( IN PDRIVER_OBJECT DriverObject ){
	g_sniff_keys = FALSE;
	UNICODE_STRING      symbolicLinkName;
	DbgPrint("[+] Unloading keyboard filter driver...\n");
	if(gKbdHookDevice)
	{
		RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_NAME_STRING);
		IoDeleteSymbolicLink(&symbolicLinkName);
		if(ControlDeviceObject)IoDeleteDevice(ControlDeviceObject);
		OnKbdUnload (gKbdHookDevice);
	}
}

NTSTATUS cmdHookKeyboard(IN PDRIVER_OBJECT DriverObject){
	CCHAR		 ntNameBuffer[64];
    STRING		 ntNameString;
    UNICODE_STRING       ntUnicodeString;
    NTSTATUS             status;
	PKEYSNIFF_DEVEXTN    devextn;

	DbgPrint("[+] Hooking keyboard...\n");
	GetIrqlLevel("cmdHookKeyboard");
    sprintf( ntNameBuffer, "\\Device\\KeyboardClass0" );
    RtlInitAnsiString( &ntNameString, ntNameBuffer );
    RtlAnsiStringToUnicodeString( &ntUnicodeString, &ntNameString, TRUE );
    status = IoCreateDevice( DriverObject,
			     sizeof (KEYSNIFF_DEVEXTN),
			     NULL,
			     FILE_DEVICE_KEYBOARD,
			     0,
			     FALSE,
			     &gKbdHookDevice );

    if( !NT_SUCCESS(status) ) {
		DbgPrint("[+] failed to create device!\n");
		RtlFreeUnicodeString( &ntUnicodeString );
		return STATUS_SUCCESS;
    }
	devextn = (KEYSNIFF_DEVEXTN *)(gKbdHookDevice->DeviceExtension);
	devextn->KbdHookDevice = gKbdHookDevice;
	InitializeListHead (&devextn->InterceptedIRPsHead);
	KeInitializeSpinLock (&devextn->InterceptedIRPsLock);
	gKbdHookDevice->Flags |= DO_BUFFERED_IO;
	gKbdHookDevice->Flags &= ~DO_DEVICE_INITIALIZING; 
	status = IoAttachDevice( gKbdHookDevice, &ntUnicodeString, &devextn->KbdTopOfStack );
	if( !NT_SUCCESS(status) ) {
		DbgPrint(("[-] Connection with keyboard failed!\n"));
		IoDeleteDevice( gKbdHookDevice );
		RtlFreeUnicodeString( &ntUnicodeString );
		return STATUS_SUCCESS;
    }
	RtlFreeUnicodeString( &ntUnicodeString );
	DbgPrint("[+] Successfully connected to keyboard device\n");
	return STATUS_SUCCESS;
}

NTSTATUS FilterDispatchIo(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp){
	PIO_STACK_LOCATION stack;
	NTSTATUS ns = STATUS_SUCCESS, Status;
	ULONG code, ret = 0;
	PIRP notify_irp;
	HANDLE ThreadId,ThreadHandle;
	PVOID ApcRoutine;
	PETHREAD pThread;
//	HANDLE UserEvent;

	DbgPrint("[*] === FilterDispatch ===\n");
	GetIrqlLevel("FilterDispatch");
	Irp->IoStatus.Status = ns;
	Irp->IoStatus.Information = 0;

	stack = IoGetCurrentIrpStackLocation(Irp);
	if (stack->MajorFunction == IRP_MJ_DEVICE_CONTROL){
		code = stack->Parameters.DeviceIoControl.IoControlCode;
		switch (code){ 
			case IOCTL_FETCH_DATA:
				DbgPrint("Current kernel event AT: 0x%08x\n", KernelEvent);
			break;
			
			default:
				Irp->IoStatus.Information = 0;
				ns = STATUS_INVALID_PARAMETER;
			break;
		}
	}
	
	if (ns != STATUS_PENDING)
	{
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		Irp->IoStatus.Status = ns;
	}

	return ns;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT  DriverObject,IN PUNICODE_STRING RegistryPath){
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	NTSTATUS ntStatus;
	ULONG ntdll_base, ntqueue;
	int i;

	DbgPrint("[+] Keyboard filter driver initialized.\n");
	GetIrqlLevel("DriverEntry");

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++){
        	DriverObject->MajorFunction[i] = OnStubDispatch;
    }

	RtlInitUnicodeString (&deviceNameUnicodeString, NTDEVICE_NAME_STRING);
	ntStatus = IoCreateDevice (DriverObject,
                               0,
                               &deviceNameUnicodeString,
                               FILE_DEVICE_UNKNOWN,
                               FILE_DEVICE_SECURE_OPEN,
                               FALSE, 
                               &ControlDeviceObject);
	if (NT_SUCCESS(ntStatus)) {
		DriverObject->MajorFunction[IRP_MJ_CREATE] = 
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = 
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = FilterDispatchIo;
		ControlDeviceObject->Flags |= DO_BUFFERED_IO;
	//	ControlDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING; 
		DbgPrint("[+] Filter control device created.\n");
		RtlInitUnicodeString (&deviceLinkUnicodeString, SYMBOLIC_NAME_STRING );
		ntStatus = IoCreateSymbolicLink (&deviceLinkUnicodeString, &deviceNameUnicodeString );
		if (!NT_SUCCESS(ntStatus)) {
			DbgPrint("[-] Creating symbolic link failed with status: 0x%08x!\n", ntStatus);
			if (ControlDeviceObject)IoDeleteDevice (ControlDeviceObject);
		} else {
			DbgPrint("[+] Symbolic link to filter control device created.\n");
		}
	} else {
		DbgPrint("[-] Filter control device creation failed!\n");
	}

//	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++){
  //      	DriverObject->MajorFunction[i] = OnStubDispatch;
    //}

	DriverObject->DriverUnload  = OnUnload;
	
	if (ntdll_base = GetNTDLLBase()){
		DbgPrint("[+] NTDLL : %08x\n",ntdll_base);
	} else {
		DbgPrint("[-] NTDLL not found. Transliterator driver terminating.\n");
		return STATUS_DLL_NOT_FOUND;
	}

	if (ntqueue = GetExportEntry(ntdll_base, "NtQueueApcThread")){
		DbgPrint("[+] NtQueueApcThread found at %08x\n", ntqueue);
	} else {
		DbgPrint("[-] NtQueueApcThread not found. Transliterator driver terminating.\n");
		return STATUS_PROCEDURE_NOT_FOUND;
	}

	dwNtQueueApcThreadService = GetSyscallService(ntqueue);
	if (dwNtQueueApcThreadService != -1){
		DbgPrint("[+] NtQueueApcThread service number : %d (%02x)\n", dwNtQueueApcThreadService);
	} else {
		DbgPrint("[-] NtQueueApcThread service number not found. Terminating.\n");
		return STATUS_ACCESS_VIOLATION;
	}

	return cmdHookKeyboard( DriverObject ); 
}