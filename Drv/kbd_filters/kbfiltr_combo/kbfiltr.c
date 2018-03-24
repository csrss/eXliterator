// howto use this:
// download the windows DDK (driver development kit)
// open the src/input/kbfiltr/kbfiltr.c file
// sbstitute the default routine in that file by this one
// "make" the file (using the ddk build environment that ships with the ddk).
// copyright Rob Devilee, 2006 - you're free to use this.



VOID
KbFilter_ServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    )
/*++

Routine Description:

    Called when there are keyboard packets to report to the RIT.  You can do 
    anything you like to the packets.  For instance:
    
    o Drop a packet altogether
    o Mutate the contents of a packet 
    o Insert packets into the stream 
                    
Arguments:

    DeviceObject - Context passed during the connect IOCTL
    
    InputDataStart - First packet to be reported
    
    InputDataEnd - One past the last packet to be reported.  Total number of
                   packets is equal to InputDataEnd - InputDataStart
    
    InputDataConsumed - Set to the total number of packets consumed by the RIT
                        (via the function pointer we replaced in the connect
                        IOCTL)

Return Value:

    Status is returned.

--*/
{
    PDEVICE_EXTENSION   devExt;
		/*
	static bool extended_key = false;

	if (InputDataStart->MakeCode == 0x0E0)
	{
		...
	}
	*/

	// if you're pressing a key, you will expect a "combo".
	static char flags_left = 0;
	static char flags_right = 0;
	static char key_enter = 0;

	char keycode = 0;

	// first row=none right pressed (i=1,2,3,4)
	// last column = none left pressed (i=5,10,15,20)
	// other celss are left key + one key to right kept pressed
	const char keymapping[29] = {	'a', 's', 'd', 'f',     'h',//left 

									'w', 'x', 'e', 'c',     'j',//+h
									'r', 'v', 't', 'b',     'k',//+j
									'y', 'n', 'u', 'm',     'l',//+k
									'g', 'i', 'o', 'p',     ' ', //+l
									'q', 'z', ',', '.'//+space
								};

	const char abc_code_map[26] = {	0x01e, 0x030, 0x02e, 0x020,	// abcd
									0x012, 0x021, 0x022, 0x023, //efgh
									0x017, 0x024, 0x025, 0x026, // ijkl
									0x032, 0x031, 0x018, 0x019,	// mnop
									0x010, 0x013, 0x01f, 0x014, // qrst
									0x016, 0x02f, 0x011, 0x02d, // uvwx
									0x015, 0x02c //yz
									};

	char key_combo;

	char flag_index_left = 0;
	char flag_index_right = 0;

	
	devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	
	switch (InputDataStart->MakeCode)
	{
	case 0x01e:	//A
		flag_index_left = 1;
		break;
	case 0x01f:	//S
		flag_index_left = 2;
		break;
	case 0x020: // D
		flag_index_left = 3;
		break;
	case 0x021: // F
		flag_index_left = 4;
		break;

	case 0x023: // H
		flag_index_right = 1;
		break;
	case 0x024: // J
		flag_index_right = 2;
		break;
	case 0x025: // K
		flag_index_right = 3;
		break;
	case 0x026:	// L
		flag_index_right = 4;
		break;
	case 0x039:	//space
		flag_index_right = 5;
		break;
	}

	
	//if ( (InputDataStart->Flags & KEY_MAKE) != 0)	// this is never true !!
	if ( (InputDataStart->Flags & KEY_BREAK) == 0)
	{
		/*
		// test: just send the data.
		(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			InputDataStart,
			InputDataEnd,
			InputDataConsumed);	
			*/

		
		
		// only sensitive to the asdf, hjkl keys, and the space key:
		
		if ( flag_index_left == 0 && flag_index_right == 0 )
		{
			// send normal keys unaltered
			// those are keys such as SHIFFT, ALT, left/right ...

			(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
				devExt->UpperConnectData.ClassDeviceObject,
				InputDataStart,
				InputDataEnd,
				InputDataConsumed);
		} else {
		
			// key is pressed that is asdf, hjkl, or space.
			if (flag_index_left)
				flags_left = flag_index_left;
			
			if (flag_index_right)
				flags_right = flag_index_right;
			
			key_enter = 1;

			*InputDataConsumed += 1;	// skip it...
		}
		
	} else {
		
		// on first release, publish the "old" configuration ...
		if (key_enter != 0)
		{
			key_combo = flags_left + (flags_right * 5) - 1;
			
			if (key_combo >= 0 && key_combo < 29)
			{
				// evaluate combination
				char key = keymapping[key_combo];
				if (key >= 'a' && key <= 'z')
				{
					InputDataStart->MakeCode = abc_code_map[key  - 'a'];	// scan-code for a to z
				} else {
					switch (key)
					{
					case ',':
						InputDataStart->MakeCode = 0x033;	// scan-code for comma
						break;
					case '.':
						InputDataStart->MakeCode = 0x034;	// scan-code for dot/period
						break;
					case ' ':
						InputDataStart->MakeCode = 0x039;	// scan-code for space
						break;
					}
				}
				
			} else {
				// if no combination was pressed, revert to default value
				// which means,
				// you don't change anything.
			}
			
			InputDataStart->Flags |= KEY_MAKE;
			InputDataStart->Flags &= ~KEY_BREAK;
			
			
			// send the key
			
			(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
				devExt->UpperConnectData.ClassDeviceObject,
				InputDataStart,
				InputDataEnd,
				InputDataConsumed);
			
			
			key_enter = 0;

		}
		
		// key is released
		if (flag_index_left)
			flags_left = 0;
		
		if (flag_index_right)
			flags_right = 0;
		
		// deactivate the key
		InputDataStart->Flags &= ~KEY_MAKE;
		InputDataStart->Flags |= KEY_BREAK;
		
		
		(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			InputDataStart,
			InputDataEnd,
			InputDataConsumed);
		
	}
		



}
