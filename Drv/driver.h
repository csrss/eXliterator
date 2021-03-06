
#ifndef __DRIVER_H__
#define __DRIVER_H__

typedef struct _KEYSNIFF_DEVEXTN {
    // Back pointer to the device object.  In practice, there is only one
    // present in the system at a time, pointed to by gKbdHookDevice.
    PDEVICE_OBJECT    KbdHookDevice;
    // The top of the stack before this filter was added.  AKA the location
    // to which all IRPS should be directed.
    PDEVICE_OBJECT    KbdTopOfStack;
    // Here's the list of queued irps that we have sent our own intercepts
    // downstream for, and the spin lock that we use to protect it....
    LIST_ENTRY        InterceptedIRPsHead;
    KSPIN_LOCK        InterceptedIRPsLock;
    //
} KEYSNIFF_DEVEXTN, *PKEYSNIFF_DEVEXTN;

#define KEY_UP         1
#define KEY_DOWN       0

NTSTATUS cmdHookKeyboard(IN PDRIVER_OBJECT DriverObject);
NTSTATUS OnKbdReadComplete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context );
NTSTATUS OnKbdDispatch (PDEVICE_OBJECT DeviceObject, PIRP Irp);
void OnKbdUnload (PDEVICE_OBJECT DeviceObject);
void OnKbdCancel (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

extern "C" {

NTKERNELAPI
NTSTATUS
PsLookupThreadByThreadId (
    IN PVOID        UniqueThreadId,
    OUT PETHREAD    *Thread);

NTKERNELAPI
NTSTATUS
ObOpenObjectByPointer (
    IN PVOID            Object,
    IN ULONG            HandleAttributes,
    IN PACCESS_STATE    PassedAccessState OPTIONAL,
    IN ACCESS_MASK      DesiredAccess OPTIONAL,
    IN POBJECT_TYPE     ObjectType OPTIONAL,
    IN KPROCESSOR_MODE  AccessMode,
    OUT PHANDLE         Handle);

};

extern PDEVICE_OBJECT gKbdHookDevice;

#ifndef IoSkipCurrentIrpStackLocation
#define IoSkipCurrentIrpStackLocation( Irp )	\
    (Irp)->CurrentLocation++;					\
    (Irp)->Tail.Overlay.CurrentStackLocation++;
#endif

#ifndef IoCopyCurrentIrpStackLocationToNext
#define IoCopyCurrentIrpStackLocationToNext( Irp ) {										\
    PIO_STACK_LOCATION irpSp;																\
    PIO_STACK_LOCATION nextIrpSp;															\
    irpSp = IoGetCurrentIrpStackLocation( (Irp) );											\
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) );											\
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));	\
    nextIrpSp->Control = 0; }
#endif


NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp){
	GetIrqlLevel("OnStubDispatch");
	if(DeviceObject == gKbdHookDevice){
		return OnKbdDispatch (DeviceObject, Irp);
	} else {
		Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
	    IoCompleteRequest (Irp, IO_NO_INCREMENT);
	}
	return Irp->IoStatus.Status;
}

NTSTATUS OnKbdDispatch (PDEVICE_OBJECT DeviceObject, PIRP Irp){
NTSTATUS rv;
PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
PIRP newIrp;
PKEYSNIFF_DEVEXTN devextn;
	GetIrqlLevel("OnKbdDispatch");
    devextn = (KEYSNIFF_DEVEXTN *)(DeviceObject->DeviceExtension);
    // Our keyboard hook has been called, just pass on the data
    // however, if the IRP is a read completion, let's steal
    // a look at the scan code...
    if( (g_sniff_keys) && (IRP_MJ_READ == irpStack->MajorFunction)) 
    {
        // Set the completion callback, so we can "frob" the keyboard data.
        // ----------------------------------------------------------------
        // important note: IRP's are fired down and they wait, dormant,
        // until completed.  This means that a 'blank' IRP is fired down
        // and waits for a keypress.  The very next keypress will 
        // complete the IRP and our callback routine will fire.
        //
        // The problem: if we have unloaded the driver, the callback
        // routine will not be there - but the very last IRP may 
        // still be waiting for completion and still have our callback
        // address - aka BSOD on next keypress. - SO, we HAVE to get
        // rid of this offending IRP before we unload.  This applies to
        // ANY filter driver.  Pending IRP's are a Bad Thing when we
        // want to Unload().

        // FIXME: The solution is in fact to mark the irp pending and
        // place it on a queue, then we fire off an irp of our own
        // to the keyboard device.  When our irp completes, we transfer
        // the data (or don't - we can just use the same system buffer)
        // and complete the original irp back to the caller just as the
        // kbd device itself would have had we actually passed on the
        // irp.  If we are asked to unload while irps are outstanding,
        // we simply cancel any irps we sent ourselves, skipping the
        // current stack location and passing on the irps that we 
        // had previously queued...

        // ----------------------------------------------------------------
        //  Intercept this irp: send a cancellable substitute down in its place
        newIrp = IoAllocateIrp (DeviceObject->StackSize, FALSE);
        if (newIrp)
        {
            PIO_STACK_LOCATION newirpStack;
            // copy operational details across....
            IoSetNextIrpStackLocation (newIrp);
            newirpStack = IoGetCurrentIrpStackLocation (newIrp);
            newIrp->AssociatedIrp.SystemBuffer = Irp->AssociatedIrp.SystemBuffer;
            newIrp->RequestorMode = KernelMode;   // Irp->RequestorMode;
            newIrp->Tail.Overlay.Thread = Irp->Tail.Overlay.Thread;
            newIrp->Tail.Overlay.OriginalFileObject = Irp->Tail.Overlay.OriginalFileObject;
            newIrp->Flags = Irp->Flags;
            // set up... initial....
            newirpStack->MajorFunction = IRP_MJ_READ;
            newirpStack->MinorFunction = irpStack->MinorFunction;
            newirpStack->Parameters.Read = irpStack->Parameters.Read;
            newirpStack->DeviceObject = DeviceObject;
            newirpStack->FileObject = irpStack->FileObject;
            newirpStack->Flags = irpStack->Flags;
            newirpStack->Control = 0;
            // that was our space..... now one for lower level driver
            IoCopyCurrentIrpStackLocationToNext (newIrp);
            // because that's where we set our completion routine....
            IoSetCompletionRoutine (newIrp, OnKbdReadComplete, Irp, TRUE, TRUE, TRUE);
            // no, silly.
            //IoSetNextIrpStackLocation (newIrp);
            // IoCallDriver does that for us.

            // q the incoming irp; while we own it, we may use the 
            // driver context to track the substitute irp we send down
            Irp->Tail.Overlay.DriverContext[0] = newIrp;
            ExInterlockedInsertHeadList (&devextn->InterceptedIRPsHead,
            &Irp->Tail.Overlay.ListEntry, &devextn->InterceptedIRPsLock);
            IoSetCancelRoutine (Irp, OnKbdCancel);
            // the real question is how do we keep track of the original irp
            // in the one we send down ? do we allocate a stack location for
            // ourselves? we can just put the original irp pointer in the
            // completion routine's context, and then get at it that way I 
            // believe.  what if there is a cancellation?  how do we cancel
            // our irps?  if we are unloaded, they will all be cancelled
            // by io mgr perhaps?  where's that bit I remember about not
            // setting a completion routine in the first stack location?!
            InterlockedIncrement (&g_number_of_pending_IRPs);
            // Return the results of the call to the keyboard class driver.
            rv = IoCallDriver (devextn->KbdTopOfStack, newIrp);
            if (rv == STATUS_PENDING)
            {
                // FIXME:  I've seen this crash, when I was hammering lots of keys
                // during shutdown.  ISTM that although IoCallDriver returned status
                // pending, the completion routine must have already been called 
                // before we got back here, and the original irp completed (without
                // even having been marked pending).  This is wrong and bad.  Once
                // we've set the completion routine for the new irp and fired it 
                // down the driver stack, we have to treat the orig irp as out of
                // our hands also.  That means that it must have ALREADY been
                // marked pending BEFORE we return status pending here.  Or is it
                // possible that the completion routine could mark it pending
                // before completing it ?  But then what if the completion routine
                // DOESNT fire before we return from here?  We would be returning
                // pending without having marked the irp pending.
                //  I guess the ONLY answer is to mark it pending before
                // we send down the new irp and unmark it if it did not pend,
                // before we complete it, OR PERHAPS we could not complete the
                // orig irp in the completion routine but somewhere else somehow?
                // FIXED: The simple answer is to mark it pending in the completion
                // routine of the new irp, iff the new irp pended itself.
                return rv;
            }
            // else completed synchronously...  completion routine unlinked it
            // but has left it to us to complete the orig irp synchronously too.
            rv = Irp->IoStatus.Status;
            IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);
            return rv;
        }
    }

    // Otherwise just forward it.
    IoSkipCurrentIrpStackLocation (Irp);
    // pass this onto whoever we stole it from...
    return IoCallDriver (devextn->KbdTopOfStack, Irp);
}    

// This routine is called whenever one of our queued irps is cancelled.
//  This is unlikely to happen, but we must de-q it...
void OnKbdCancel (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp){
PKEYSNIFF_DEVEXTN devextn;
PIRP newIrp;
KIRQL irql;
int found;
	GetIrqlLevel("OnKbdCancel");
    devextn = (KEYSNIFF_DEVEXTN *)(DeviceObject->DeviceExtension);

    IoSetCancelRoutine (Irp, NULL);
    IoReleaseCancelSpinLock (Irp->CancelIrql);

    // Take a look at this, because it's a nice structure: by treating
    // the spinlock as if it were a code block in this way, and scoping 
    // variables that access the spinlock-protected data inside the code
    // block, we prevent ourselves from accidentally hanging on to data
    // we shouldn't have after it is no longer protected (and therefore
    // may be stale), we indicate in an immediately visually-obvious way
    // the scope of the lock, and we are less likely to write a codepath
    // that forgets to unlock the lock (as long as we don't use return
    // or goto inside the code block!)
    found = FALSE;
    KeAcquireSpinLock (&devextn->InterceptedIRPsLock, &irql);
    {
        PLIST_ENTRY thisone;

        thisone = devextn->InterceptedIRPsHead.Flink;
        while ((thisone != &Irp->Tail.Overlay.ListEntry)
            && (thisone != &devextn->InterceptedIRPsHead))
        {
            thisone = thisone->Flink;
        }
        found = (thisone == &Irp->Tail.Overlay.ListEntry);
        if (found)
            RemoveEntryList (&Irp->Tail.Overlay.ListEntry);
    }
    KeReleaseSpinLock (&devextn->InterceptedIRPsLock, irql);

    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;

    newIrp = (PIRP)Irp->Tail.Overlay.DriverContext[0];

 //   IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);

    // It is of course possible the new irp will have completed while
    // we're busy cancelling this existing irp..... we must take care
    // in the completion routine to leave cancelled origirps well alone.
    if (newIrp)
        IoCancelIrp (newIrp);
	IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);		/* moved here from top due to race condition */
  return;
}

NTSTATUS OnKbdReadComplete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context )
{
    PIO_STACK_LOCATION        IrpSp;
    PKEYBOARD_INPUT_DATA      KeyData;
    int                       numKeys, i, found;
    KIRQL					  aIrqL;
	PIRP                      origIrp;
	PKEYSNIFF_DEVEXTN         devextn;
	NTSTATUS				  Status;
	DbgPrint("[+] Altering keyboard layout...\n");

	GetIrqlLevel("OnKbdReadComplete");
    //
    // Request completed - look at the result.
    //

    IrpSp = IoGetCurrentIrpStackLocation( Irp );
	devextn = (KEYSNIFF_DEVEXTN *)(DeviceObject->DeviceExtension);

    // It the newirp was cancelled, it was by OnKbdCancel, which
    // means that we must not touch the old irp in any way. Or
    // perhaps by OnUnload.
    if (Irp->Cancel)
    {
        // new irp was cancelled, we must be unloading, so 
        // do nothing: leave it all to the foreground.
        // frankly we might as well not bother to be called
        // for a cancellation.....
        //return STATUS_SUCCESS;
        // actually we should free ourselves, since the
        // cancel routine for orig irps that tries to cancel
        // us can't safely do that... (we may not be unloading
        // after all)
        IoFreeIrp (Irp);
        // this irp no longer counts as an outstanding irp!
        InterlockedDecrement (&g_number_of_pending_IRPs);
        // We allocated and freed this IRP; tell IOmgr that
        // it has no further business with it.
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    // get pointer to the original irp......
    origIrp = (PIRP)Context;

    // find and de-queue the original irp......
    found = FALSE;
    KeAcquireSpinLock (&devextn->InterceptedIRPsLock, &aIrqL);
    {
        PLIST_ENTRY thisone;
        thisone = devextn->InterceptedIRPsHead.Flink;
        while ((thisone != &origIrp->Tail.Overlay.ListEntry)
            && (thisone != &devextn->InterceptedIRPsHead))
        {
            thisone = thisone->Flink;
        }
        found = (thisone == &origIrp->Tail.Overlay.ListEntry);
        if (found)
            RemoveEntryList (&origIrp->Tail.Overlay.ListEntry);
    }
    KeReleaseSpinLock (&devextn->InterceptedIRPsLock, aIrqL);

    // It's ok to propagate the pending returned status then.
    if (Irp->PendingReturned)
    {
        IoMarkIrpPending (Irp); // This is in fact superfluous!
        // If the new irp pended, so did the original! But
        // we can only touch it if it hasn't been cancelled
        // or forwarded.
        if (found)
            IoMarkIrpPending (origIrp);
    }

	
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) {	// is IRP status ok
		// just frobbing the MakeCode handles both the up-key
        // and down-key cases since the up/down information is specified
        // seperately in the Flags field of the keyboard input data 
        // (0 means key-down, 1 means key-up).
        //
  	    if(g_sniff_keys)
  	    {	// if sniffer is active
	  	    KeyData = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;

	        if(KeyData)
	        {	// key data is not empty
		        numKeys = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);
		        for( i = 0; i < numKeys; i++ ) // keys processing
		        {
					DbgPrint("ScanCode: %x ", KeyData->MakeCode );
					DbgPrint("Flags %d\n", KeyData->Flags );

					if(KeyData->MakeCode == J_CONTROL){	// j -> й
						KeyData->MakeCode = Q_CONTROL;
						LAST_CONTROL = J_CONTROL;
					} else if(KeyData->MakeCode == A_CONTROL){ // a -> а
						KeyData->MakeCode = F_CONTROL;
					} else if(KeyData->MakeCode == Z_CONTROL){ // z -> з
						KeyData->MakeCode = P_CONTROL;
					} else if(KeyData->MakeCode == C_CONTROL){	// c -> ц
						KeyData->MakeCode = W_CONTROL;
					} else if(KeyData->MakeCode == U_CONTROL){	// u -> у
						if(LAST_CONTROL == J_CONTROL){
							BackSpaced = TRUE;
							KeyData->MakeCode = BACKSPACE;
							LAST_CONTROL = 0x00;
							IoCallDriver (devextn->KbdTopOfStack, Irp);
						} else {
							if(BackSpaced && KeyData->Flags == 0){ 
								KeyData->MakeCode = X_CONTROL3;
								BackSpaced = FALSE;
							} else {
								KeyData->MakeCode = E_CONTROL;
							}
						}
					} else if(KeyData->MakeCode == K_CONTROL){	// k -> к
						KeyData->MakeCode = R_CONTROL;
					} else if(KeyData->MakeCode == E_CONTROL){	// e -> е
						KeyData->MakeCode = T_CONTROL;
					} else if(KeyData->MakeCode == N_CONTROL){	// n -> н
						KeyData->MakeCode = Y_CONTROL;
					} else if(KeyData->MakeCode == G_CONTROL){	// g -> г
						KeyData->MakeCode = U_CONTROL;
					} else if(KeyData->MakeCode == F_CONTROL){	// f -> ф
						KeyData->MakeCode = A_CONTROL;			
					} else if(KeyData->MakeCode == Y_CONTROL){	// y -> ы
						KeyData->MakeCode = S_CONTROL;
					} else if(KeyData->MakeCode == V_CONTROL){	// v -> в
						KeyData->MakeCode = D_CONTROL;
					} else if(KeyData->MakeCode == P_CONTROL){	// p -> п
						KeyData->MakeCode = G_CONTROL;
					} else if(KeyData->MakeCode == R_CONTROL){	// r -> р
						KeyData->MakeCode = H_CONTROL;
					} else if(KeyData->MakeCode == O_CONTROL){	// o -> о
						KeyData->MakeCode = J_CONTROL;
					} else if(KeyData->MakeCode == L_CONTROL){	// l -> л
						KeyData->MakeCode = K_CONTROL;
					} else if(KeyData->MakeCode == D_CONTROL){	// d -> д
						KeyData->MakeCode = L_CONTROL;
					} else if(KeyData->MakeCode == S_CONTROL){	// s -> с
						KeyData->MakeCode = C_CONTROL;
					} else if(KeyData->MakeCode == M_CONTROL){	// m -> м
						KeyData->MakeCode = V_CONTROL;
					} else if(KeyData->MakeCode == I_CONTROL){	// i -> и
						KeyData->MakeCode = B_CONTROL;
					} else if(KeyData->MakeCode == T_CONTROL){	// t -> т
						KeyData->MakeCode = N_CONTROL;
					} else if(KeyData->MakeCode == H_CONTROL){	// h -> х
						KeyData->MakeCode = X_CONTROL8;
					} else if(KeyData->MakeCode == B_CONTROL){	// b -> б
						KeyData->MakeCode = X_CONTROL2;
					} else if(KeyData->MakeCode == Q_CONTROL){	// q -> я
						KeyData->MakeCode = Z_CONTROL;
					} else if(KeyData->MakeCode == W_CONTROL){	// w -> щ
						KeyData->MakeCode = O_CONTROL;
					}
		        }	// end of keys processing
		    }	// end of 'keydata is not empty' check
	    }	// end of 'if sniffer is active'
    }	// end of 'if IRP status ok'

    // ok... now we should complete the orig irp if we
    // found it (if we didn't find it, someone else has
    // it and we leave it to them...
    if (found)
    {
        // Set the old IRP's status from the new IRP's one, since
        // it was the same operation.
        origIrp->IoStatus.Status = Irp->IoStatus.Status;
        origIrp->IoStatus.Information = Irp->IoStatus.Information;
        IoSetCancelRoutine (origIrp, NULL);
        // we only complete the request if this newirp completed
        // async; otherwise we leave it to the dispatch routine,
        // to which we will return shortly as we unwind the stack,
        // since we've completed synchronously.
        if (Irp->PendingReturned)
        {
            IoCompleteRequest (origIrp, IO_KEYBOARD_INCREMENT);
        }
     }

	IoFreeIrp (Irp);
	InterlockedDecrement (&g_number_of_pending_IRPs);
	return STATUS_MORE_PROCESSING_REQUIRED;
}

void OnKbdUnload (PDEVICE_OBJECT DeviceObject){
PKEYSNIFF_DEVEXTN devextn;
KIRQL irql;
LIST_ENTRY forwarding_list;

    devextn = (KEYSNIFF_DEVEXTN *)(DeviceObject->DeviceExtension);
    IoDetachDevice (devextn->KbdTopOfStack);
    InitializeListHead (&forwarding_list);
    // Now there are no more IRPs arriving, we pull irps off the list
    // cancel their newirps and forward them........
    KeAcquireSpinLock (&devextn->InterceptedIRPsLock, &irql);
    {
        PLIST_ENTRY thisone;
        thisone = devextn->InterceptedIRPsHead.Flink;
        while (thisone != &devextn->InterceptedIRPsHead)
        {
            PIRP newIrp, Irp;

            Irp = (PIRP) (CONTAINING_RECORD(thisone, IRP, Tail.Overlay.ListEntry));
            // in theory cancelling all these irps should work as long as they
            // haven't completed.
            newIrp = (PIRP) (Irp->Tail.Overlay.DriverContext[0]);
            // advance thisone now in case we are going to unlink the current irp
            thisone = thisone->Flink;
            if (newIrp)
            {
                // If we cancel it successfully we flag the original irp
                // as no longer having a newirp....
                if (IoCancelIrp (newIrp))
                {
                    Irp->Tail.Overlay.DriverContext[0] = NULL;
                    // and forward it like we could have done all along..
                    IoSetCancelRoutine (Irp, NULL);
                    RemoveEntryList (&Irp->Tail.Overlay.ListEntry);
                    InsertHeadList (&forwarding_list, &Irp->Tail.Overlay.ListEntry);
                }
                else DbgPrint ("Failed to cancel newirp $%08x orig $%08x\n", newIrp, Irp);
                // the only reason we could fail to cancel our own IRP is if it
                // has completed, in which case it will soon remove the original
                // irp itself and sort it out.
            }
        }
    }
    KeReleaseSpinLock (&devextn->InterceptedIRPsLock, irql);
    // now forward all the irps on the list....
    while (!IsListEmpty (&forwarding_list))
    {
        PLIST_ENTRY thisone;
        PIRP Irp;
        NTSTATUS rv;

        thisone = RemoveHeadList (&forwarding_list);
        Irp = (PIRP) (CONTAINING_RECORD(thisone, IRP, Tail.Overlay.ListEntry));
        IoSkipCurrentIrpStackLocation (Irp);
        rv = IoCallDriver (devextn->KbdTopOfStack, Irp);
    }
    // Now delete our detached device, as we have handed off all irps...
    IoDeleteDevice (DeviceObject);
	DbgPrint("[+] Keyboard filter driver successfuly unloaded!\n");
}

#endif 