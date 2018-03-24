
// howto use this:
// download the windows DDK (driver development kit)
// open the src/input/kbfiltr/kbfiltr.c file
// sbstitute the default routine in that file by this one
// "make" the file (using the ddk build environment that ships with the ddk).



VOID
KbFilter_ServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    )
{
    PDEVICE_EXTENSION   devExt;

	if ( (InputDataStart->Flags & KEY_E0) == 0)
	{
		// turn it into an extended key.
		InputDataStart->Flags |= KEY_E0;
	}


    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
		devExt->UpperConnectData.ClassDeviceObject,
		InputDataStart,
		InputDataEnd,
		InputDataConsumed);

}

