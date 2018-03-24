#ifndef _INICONTROL_H
#define _INICONTROL_H
class eXslitOptions {
protected:
	wchar_t buffer[MAX_PATH];
	size_t buffLen;
	wchar_t *INIFile;
private:

	wchar_t * GetIniFile(wchar_t *Relative){
		DWORD dwRet;
		static wchar_t *output;
		size_t Len;
		dwRet = GetCurrentDirectoryW(0, NULL);
		Len = sizeof(wchar_t*) * dwRet + (MAX_PATH + wcslen(Relative));
		output = (wchar_t*)malloc(Len);
		GetCurrentDirectoryW(Len, output);
		wcscat(output, Relative);
		return output;
	}

public:
	eXslitOptions(wchar_t *RelativePath){
		RtlSecureZeroMemory(buffer, sizeof buffer);
		buffLen = sizeof(buffer);
		INIFile = GetIniFile(RelativePath);
	}
	~eXslitOptions(){
		RtlSecureZeroMemory(buffer, sizeof buffer);
		INIFile = NULL;
	}

	BOOL IsOptionTrue(__in wchar_t *Section){
		BOOL ReturnVal;
		GetPrivateProfileStringW(Section, L"value", NULL, buffer, buffLen, INIFile);
		if(wcscmp(buffer, L"TRUE") == 0) ReturnVal = TRUE;
		else ReturnVal = FALSE;
		RtlSecureZeroMemory(buffer, sizeof buffer);
		return ReturnVal;
	}

	BOOL SetOption(__in BOOL Mode, 
							 __in wchar_t *Section)
	{
		return WritePrivateProfileStringW(Section, 
										  L"value", 
										  Mode ? L"TRUE" : L"FALSE", 
										  INIFile);
	}

	wchar_t * GetExtendedOption(__in wchar_t *Section, 
										  __in wchar_t *Value)
	{
		GetPrivateProfileStringW(Section, Value, NULL, buffer, buffLen, INIFile);
		int RetnBufferSize = sizeof(wchar_t*)*wcslen(buffer)+1;
		wchar_t *RentBuffer = (wchar_t*)malloc(RetnBufferSize);
		wcscpy(RentBuffer, buffer);
		RtlSecureZeroMemory(buffer, sizeof buffer);
		return RentBuffer;
	}

	BOOL SetExtendedOption(__in wchar_t *Section, 
									 __in wchar_t *Value, 
									 __in wchar_t *Option)
	{
		return WritePrivateProfileStringW(Section, 
										  Value, 
										  Option, 
										  INIFile);
	}

	BOOL DeleteOption(__in wchar_t* Section){
		return WritePrivateProfileStringW(Section, NULL, NULL, INIFile);
	}
};
#endif 