
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
	KEYBOARD_INPUT_DATA extra;
	//ULONG nTestConsumed;
	
	USHORT add_keycombo[2] = {0x036, 0x038};
	int Nkeys = 2;

	int i;

	

	// try to switch on(/off) the key pairs.
	for ( i = 0; i < Nkeys; i ++ )
	{
		extra.UnitId = InputDataStart->UnitId;
		extra.MakeCode = add_keycombo[i] | (InputDataStart->MakeCode & 0x080);		//switch-on (or off )
			//InputDataStart->MakeCode + i + 1; (test input)
		extra.Flags = InputDataStart->Flags;
		extra.Reserved = InputDataStart->Reserved;
		extra.ExtraInformation = InputDataStart->ExtraInformation;
		
		devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
		
		(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			&extra,
			&extra + 1,	// just add 1 byte
			InputDataConsumed);
	}
	
	//InputDataStart->MakeCode = (USHORT)(InputDataEnd - InputDataStart) + 1;
	// this returns "2".
	
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
		devExt->UpperConnectData.ClassDeviceObject,
		InputDataStart,
		InputDataEnd,
		InputDataConsumed);

}
