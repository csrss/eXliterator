#ifndef _SYSTEM_H
#define _SYSTEM_H
#pragma warning(disable: 4996)
#define SMART_GET_VERSION               CTL_CODE(IOCTL_DISK_BASE, 0x0020, METHOD_BUFFERED, FILE_READ_ACCESS)
#define SMART_SEND_DRIVE_COMMAND        CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#pragma pack(1)
typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumberOfCylinders;               // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumberOfHeads;                   // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT SectorsPerTrack;                 // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    USHORT SerialNumber[10];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    USHORT FirmwareRevision[4];             // 2E  23-26
    USHORT ModelNumber[20];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A  53
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[2];                    //     69-70
    USHORT ReleaseTimeOverlapped;           //     71
    USHORT ReleaseTimeServiceCommand;       //     72
    USHORT MajorRevision;                   //     73
    USHORT MinorRevision;                   //     74
    USHORT Reserved6[50];                   //     75-126
    USHORT SpecialFunctionsEnabled;         //     127
    USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;
#pragma pack()

class System {
protected:

private:

	char HardDriveSerialNumber [1024];
	char HardDriveModelNumber [1024];

	wchar_t * GetSystemDevice(){
	    int iLen;
	    wchar_t FileName[MAX_PATH];
	    static wchar_t Dev[MAX_PATH];
		wchar_t let[3], vol[128];
		iLen = GetSystemDirectoryW(FileName, sizeof(FileName));
		if(iLen > sizeof(Dev) || iLen > sizeof(FileName)) return NULL;
		let[0] = FileName[0]; let[1] = ':'; let[2] = '\0';
		QueryDosDeviceW(let, vol, sizeof(vol)) ? swprintf(Dev, L"%s", vol) : 
			swprintf(Dev, L"%s", L"err");
		return Dev;
	}

	HANDLE NtOpenDeviceW(__in wchar_t *DeviceName){
		HANDLE hFile;
		OBJECT_ATTRIBUTES ObjectAttributes;
		IO_STATUS_BLOCK ioStatusBlock;
		UNICODE_STRING PathNameString;
		NTSTATUS Status;
		LARGE_INTEGER   fileSize;
		wchar_t debug[MAX_PATH];
		SecureZeroMemory(&debug, sizeof debug);
		fileSize.QuadPart = 0;
		if(!RtlInitUnicodeString || !NtCreateFile){
			OutputDebugStringW(L"RtlInitUnicodeString and / or NtCreateFile is not initialized!");
			return NULL;
		}
		RtlInitUnicodeString(&PathNameString, DeviceName);
		InitializeObjectAttributes( &ObjectAttributes, &PathNameString, OBJ_CASE_INSENSITIVE, NULL, NULL );
		Status = NtCreateFile(	&hFile,
								GENERIC_READ | GENERIC_WRITE,
								&ObjectAttributes,
								&ioStatusBlock,
								&fileSize,
								FILE_ATTRIBUTE_NORMAL,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0x00000001,		//FILE_OPEN
								0,		//FILE_NON_DIRECTORY_FILE
								NULL,
								0);
		if(Status == STATUS_SUCCESS){
			return hFile;
		} else {
			swprintf(debug, MAX_PATH, L"[-] NtOpenDevice Status = 0x%08x\r\n", Status);
			OutputDebugStringW(debug);
			return NULL;
		}
	return NULL;
	}

	void remove_whitespaces(__in char *str) {
	    char *p;
	    size_t len = strlen(str);
	
	    for(p = str; *p; p ++, len --) {
	        while(isspace(*p)) memmove(p, p+1, len--);
	    }
	}

	void removeWhite (std::string &str)
	{
		std::string temp;
		for (unsigned int i = 0; i < str.length(); i++)
		    if (str[i] == ' ' && str[i-1] == ' ') {}
		    else temp += str[i];
		str = temp;
	}

	void removeAllWhite (std::string &str)
	{
	    std::string temp;
	    for (unsigned int i = 0; i < str.length(); i++)
	        if (str[i] != ' ') temp += str[i];
	    str = temp;
	}

	char * ConvertToString(DWORD diskdata[256], 
									 int firstIndex,
									 int lastIndex, 
									 char* buf)
	{
	   int index = 0;
	   int position = 0;
	   for (index = firstIndex; index <= lastIndex; index++){
	      buf [position++] = (char) (diskdata [index] / 256);
	      buf [position++] = (char) (diskdata [index] % 256);
	   }
	   buf[position] = '\0';
	   for (index = position - 1; index > 0 && isspace(buf [index]); index--)buf [index] = '\0';
	   return buf;
	}

	char * ReturnIdeInfo( int drive, DWORD diskdata[256])
	{
		char serialNumber [1024];
		char modelNumber [1024];
		char revisionNumber [1024];
		char bufferSize [32];
		char *outputSerialZ = NULL;
		int outputSerialZ_len;

		__int64 sectors = 0;
		__int64 bytes = 0;

      //  copy the hard drive serial number to the buffer
		ConvertToString (diskdata, 10, 19, serialNumber);
		ConvertToString (diskdata, 27, 46, modelNumber);
		ConvertToString (diskdata, 23, 26, revisionNumber);
		sprintf(bufferSize, "%u", diskdata [21] * 512);

		if (0 == HardDriveSerialNumber [0] &&
			(isalnum (serialNumber [0]) || isalnum (serialNumber [19]))){
			strcpy (HardDriveSerialNumber, serialNumber);
			strcpy (HardDriveModelNumber, modelNumber);
		}
        
		if (diskdata [83] & 0x400) 
			sectors = diskdata [103] * 65536I64 * 65536I64 * 65536I64 + 
					diskdata [102] * 65536I64 * 65536I64 + 
					diskdata [101] * 65536I64 + 
					diskdata [100];
		else
			sectors = diskdata [61] * 65536 + diskdata [60];

		bytes = sectors * 512;
		outputSerialZ_len = sizeof(char) * (strlen(serialNumber)+
						strlen(revisionNumber)+strlen(bufferSize)) + MAX_PATH;
		outputSerialZ = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, outputSerialZ_len);
		sprintf(outputSerialZ, "%s%s%I64d", serialNumber, bufferSize, bytes);
		remove_whitespaces(outputSerialZ);
		return outputSerialZ;
	}
/*
	NTSTATUS IsHDDSerialAvailable(void){
		NTSTATUS RetStatus = STATUS_UNSUCCESSFUL;
		NTSTATUS Status;
		IO_STATUS_BLOCK ioStatusBlock;
		HANDLE hPhysicalDriveIOCTL = 0;
	
		if(!NtDeviceIoControlFile || !NtClose){
			OutputDebugStringW(L"NtDeviceIoControlFile and / or NtClose is not initialized!");
			return (__w64 long)-2;
		}

		hPhysicalDriveIOCTL = NtOpenDevice(GetSystemDevice());
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){
			OutputDebugStringW(L"@IsHDDSerialAvailable: Unable to open device!\r\n");
			RetStatus = (__w64 long)-1;
		} else {
			STORAGE_PROPERTY_QUERY query;
	        DWORD cbBytesReturned = 0;
			char buffer [10000];
			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;
			memset (buffer, 0, sizeof (buffer));
			if (Status = NtDeviceIoControlFile (hPhysicalDriveIOCTL, NULL, NULL, NULL,
												&ioStatusBlock, IOCTL_STORAGE_QUERY_PROPERTY,
												&query, sizeof(query), &buffer, 
												sizeof(buffer)) != STATUS_SUCCESS){
				char debug[MAX_PATH] = {0};
				sprintf(debug, "NtDeviceIoControlFile status: 0x%08x\r\n", Status);
				OutputDebugStringA(debug);
				RetStatus = Status;
			} else {
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				if((unsigned long) descrip->SerialNumberOffset == -1){
					RetStatus = STATUS_UNSUCCESSFUL;
					MessageBoxA(0,"SerialNumberOffset == -1",NULL,0);
				} else {
					RetStatus = STATUS_SUCCESS;
					OutputDebugStringW(L"Seems like hard drive serial is available.\r\n");
				}
			}
		}
		NtClose(hPhysicalDriveIOCTL);
		return RetStatus;
	}
*/ 
	char * RetrieveHardDriveSerialNumber(void){
		int drive = 0;
		char *SerialA = NULL;
		char debug[MAX_PATH] = {0};
		DWORD BytesReturned = 0;
		IO_STATUS_BLOCK    ioStatusBlock;
		NTSTATUS Status;
		int ijk;
	//	bool Mode;
		static char SerialIsNot[] = "NOT_AVAILABLE";
	
	//	TranslitOptions.debug_info == 1 ? Mode = true : Mode = false;
	//	if(IsHDDSerialAvailable() != STATUS_SUCCESS) return SerialIsNot;
	
	      HANDLE hPhysicalDriveIOCTL = 0;
		  hPhysicalDriveIOCTL = NtOpenDevice(GetSystemDevice());
	
	      if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){
			  OutputDebugStringW(L"Unable to open device!\r\n");
			  return NULL;
	      } else {
			RtlSecureZeroMemory(&debug, sizeof debug);
			sprintf(debug, "hdd handle is at 0x%08x\r\n", hPhysicalDriveIOCTL);
			OutputDebugStringA(debug);
	         GETVERSIONINPARAMS GetVersionParams;
	         DWORD cbBytesReturned = 0;
	         memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));
	         if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
                   NULL, 
                   0,
     			   &GetVersionParams, sizeof (GETVERSIONINPARAMS),
				   &cbBytesReturned, NULL) ){
				RtlSecureZeroMemory(&debug, sizeof debug);
				sprintf(debug, "SMART_GET_VERSION failed! Status = %d\r\n", GetLastError());
				OutputDebugStringA(debug);
				NtClose (hPhysicalDriveIOCTL);
				return SerialIsNot;
	         } else {
				ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
	        	PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, CommandSize);
				Command -> irDriveRegs.bCommandReg = 0xEC;
				BytesReturned = 0;
				if ( ! DeviceIoControl (hPhysicalDriveIOCTL, 
				                    SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
									Command, CommandSize,
									&BytesReturned, NULL) ){
					sprintf(debug, "SMART_RCV_DRIVE_DATA failed! Status = %d\r\n", GetLastError());
					OutputDebugStringA(debug);
					NtClose (hPhysicalDriveIOCTL);
					return SerialIsNot;
		        } else {
	                DWORD diskdata [256];
	                USHORT *pIdSector = (USHORT *)(PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;
	                for (ijk = 0; ijk < 256; ijk++) diskdata [ijk] = pIdSector [ijk];
	                SerialA = ReturnIdeInfo (drive, diskdata);
				}
	            NtClose (hPhysicalDriveIOCTL);
				HeapFree (GetProcessHeap(), 0, Command);
			 }
	   }
	   return SerialA;
	}

	char * base64_encode(__in const char *src)
	{
	  unsigned int len = strlen(src);
	  char *dst = (char*)malloc(sizeof(char) * len + MAX_PATH);
	  char base64_table[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz""0123456789+/"};
	  int y = 0, i, flag=0, m=0;
	  int n = 3;
	  char triple[3], quad[4];//, c;
	  unsigned int x;
	
	    for(x = 0; x < len; x = x + 3){
	        if((len - x) / 3 == 0) n = (len - x) % 3;
	        for(i=0; i < 3; i++) triple[i] = '\0';
	        for(i=0; i < n; i++) triple[i] = src[x + i];
	        quad[0] = base64_table[(triple[0] & 0xFC) >> 2]; // FC = 11111100
	        quad[1] = base64_table[((triple[0] & 0x03) << 4) | ((triple[1] & 0xF0) >> 4)]; // 03 = 11
	        quad[2] = base64_table[((triple[1] & 0x0F) << 2) | ((triple[2] & 0xC0) >> 6)]; // 0F = 1111, C0=11110
	        quad[3] = base64_table[triple[2] & 0x3F]; // 3F = 111111
	        if(n < 3) quad[3] = '='; if(n < 2) quad[2] = '=';
	        for(i=0; i < 4; i++) dst[y + i] = quad[i];
	        y = y + 4; m = m + 4;
	        if((m != 0)&&((m % 76) == 0)){
	            dst[y] = '\r'; dst[y+1] = '\n';
	            flag++; y += 2; m = 0;
	        }
	    }
	  dst[y] = '\0';
	  return dst;
	}

	void _xor(__in char *string, 
						__in char *key)
	{
	    int string_len = strlen(string);
	    int key_length = strlen(key);
	
	    int i, position;
	
	    for(i = 0; i < string_len; i++)
	    {
	        position = i % key_length;
	        string[i] = (char)((int)string[i] ^ (int)key[position]);
	    }
	}

	char * rc4(unsigned char * ByteInput, unsigned char * pwd){
	    unsigned char * temp;
	    int i,j=0,t,tmp,tmp2,s[256], k[256];
	    for (tmp=0;tmp<256;tmp++){
	        s[tmp]=tmp;
	        k[tmp]=pwd[(tmp % strlen((char *)pwd))];
	    }
	    for (i=0;i<256;i++){
	        j = (j + s[i] + k[i]) % 256;
	        tmp=s[i];
	        s[i]=s[j];
	        s[j]=tmp;
	    }
	    temp = new unsigned char [ (int)strlen((char *)ByteInput) + 1 ] ;
	    i=j=0;
	    for (tmp=0;tmp<(int)strlen((char *)ByteInput);tmp++){
	        i = (i + 1) % 256;
	        j = (j + s[i]) % 256;
	        tmp2=s[i];
	        s[i]=s[j];
	        s[j]=tmp2;
	        t = (s[i] + s[j]) % 256;
	        if (s[t]==ByteInput[tmp])
	            temp[tmp]=ByteInput[tmp];
	        else
	            temp[tmp]=s[t]^ByteInput[tmp];
	    }
	    temp[tmp] = 0;
	    int iLen = (int)strlen((char *)temp);
	    static char *out = (char *)malloc(sizeof(char)*(iLen*MAX_PATH));
	    int xXx;
	    for (xXx=0; xXx < iLen; xXx++)sprintf(out+xXx*2,"%02X",(unsigned char)temp[xXx]);
	    return out;
	}

public:
	System(){}
	~System(){}

	wchar_t * QuerySystemDeviceLetter(void){
		return GetSystemDevice();
	}

	HANDLE NtOpenDevice(wchar_t *DeviceName){
		return NtOpenDeviceW(DeviceName);
	}

	char* UpToLow(char* str) {
		unsigned int i;
	   for ( i=0;i<strlen(str);i++) 
		    if (str[i] >= 0x41 && str[i] <= 0x5A) 
		        str[i] = str[i] + 0x20;
		return str;
	}

	char * HardGem(void){
		return RetrieveHardDriveSerialNumber();
	}

	char * HardGemEx(void){
		char Key[] = "1";
		char *Serial = RetrieveHardDriveSerialNumber();
		char *enCed = rc4((unsigned char *)Serial, (unsigned char *)Key);
		static char *Result = base64_encode(enCed);
		RemoveWhitespaces(Result);
		return Result;
	}

	char * base64encode(__in const char *src){
		return base64_encode(src);
	}

	void __xor(__in char string[], 
						 __in char key[])
	{
		return _xor(string, key);
	}

	void RemoveWhitespaces(__in char *str) {
		return remove_whitespaces(str);
	}

	char * rc4crypt(__in unsigned char * ByteInput, 
							  __in unsigned char * pwd)
	{
		return rc4(ByteInput, pwd);
	}
};

#endif