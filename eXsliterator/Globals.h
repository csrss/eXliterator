

eXslitOptions *eXoptions;
int ActivateClasses(){

	GuiEffectsEx = new GuiEffects();	// activate glass effects

	DataGrid[0] = new DataGridControl(Opt.ColHeaders ? FALSE : TRUE);	// activate datagrid for transliterated
	DataGrid[1] = new DataGridControl(Opt.ColHeaders ? FALSE : TRUE);	// activate datagrid for translations

	DarkGlasButton[0] = new DGButton(FALSE);
	DarkGlasButton[1] = new DGButton(FALSE);
	DarkGlasButton[2] = new DGButton(FALSE);
	DarkGlasButton[3] = new DGButton(FALSE);
	DarkGlasButton[4] = new DGButton(FALSE);
	DarkGlasButton[5] = new DGButton(TRUE);

	Frame[0] = new FrameControl();
	TranslitRadio[0] = new RadioControl(TRUE, TRUE, FALSE);
	TranslationDir[0] = new ComboControl();
	WordEntry[0] = new EntryControl(FALSE, FALSE);

	ToolTip[0] = new ToolTipWindow();	// for translit area
	ToolTip[1] = new ToolTipWindow();	// for translations area
	ToolTip[2] = new ToolTipWindow();	// for main radio
	ToolTip[3] = new ToolTipWindow();	// for translation dir selection
	ToolTip[4] = new ToolTipWindow();	// for word entry
	ToolTip[5] = new ToolTipWindow();	// for fetch button
	ToolTip[6] = new ToolTipWindow();	// for dict db button
	ToolTip[7] = new ToolTipWindow();	// for settings button
	ToolTip[8] = new ToolTipWindow();	// for quit button

	eXoptions = new eXslitOptions(L"\\Settings.ini");
	Deb = new DebugLog(FALSE);

	CMenu[0] = new ContextMenu();
	CMenu[1] = new ContextMenu();
	CMenu[2] = new ContextMenu();
	finalcunt = new Timing();
	return 0;
}

int InsertWindow(__in HWND hWnd, 
						   __in int CtrlID, 
						   __in bool Transliterated, 
						   __in wchar_t *WindowName)
{
	int RootIndex = DataGrid[0]->InsertRootItem(Transliterated ? L"ON" : L"OFF",
		Transliterated ? 1 : 0);
	DataGrid[0]->InsertChildItem(WindowName, RootIndex, 1);
	return 0;
}

wchar_t * GetProcessName(__in DWORD processID)
{
	static wchar_t szProcessName[MAX_PATH];
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );
 
    if (NULL != hProcess ){
        HMODULE hMod;
        DWORD cbNeeded;
 
        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ){
            GetModuleBaseNameW( hProcess, hMod, szProcessName, MAX_PATH );
        }
    }
    CloseHandle( hProcess );
    return szProcessName;
}

DWORD ProcMonitorThread(LPVOID lParam){
	CLIENT_ID ClientID;
	NTSTATUS Status;
	DWORD ExitCode;
	BOOL en;
	bool Mode;
	HANDLE ProcHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	HWND ProcWnd = (HWND)lParam;
	DWORD ThreadID, ProcessID;

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

	ThreadID = GetWindowThreadProcessId(ProcWnd, &ProcessID);
	ClientID.UniqueProcess = (HANDLE)ProcessID;
	ClientID.UniqueThread = 0;

	Deb->dprint(Mode, L"@ProcMonitorThread: Monitoring %s, Item iNdex = 0\r\n", 
		GetProcessName(ProcessID));

	RtlAdjustPrivilege(20, TRUE, AdjustCurrentProcess, &en);
	InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);
	Status = NtOpenProcess(	&ProcHandle, 
							SYNCHRONIZE| PROCESS_QUERY_INFORMATION, 
							&ObjectAttributes,
							&ClientID);
	if(Status != STATUS_SUCCESS){
		Deb->dprint(Mode, L"@ProcMonitorThread: Failed to open process. Status: 0x%08x\r\n", Status);
		return -1;
	}

	ExitCode = NtWaitForSingleObject(ProcHandle, FALSE, NULL);
	if(ExitCode == 0){
		Deb->dprint(Mode, L"@ProcMonitorThread: %s finished. ExitCode: %d\r\n", 
			GetProcessName(ProcessID), ExitCode);
	}
	wchar_t *WindowName = SearchWindowNameByHandle(ProcWnd, MemDatabase);
	int iNdex = DataGrid[0]->GetIndexByItemText(WindowName, 1, FALSE);

	Deb->dprint(Mode, L"INDEX: %d\r\n", iNdex);
	DeleteTransliterated(MemDatabase, WindowName);
	DataGrid[0]->DeleteItem(iNdex);

	return ExitCode;
}

DWORD __stdcall GetAddr(char *Function, wchar_t *Module){
	__asm {
		push Function
		push Module
		call dword ptr LoadLibraryW
		push eax
		call dword ptr GetProcAddress
	}
	/* returns function address stored in eax register */
}

BOOL CALLBACK EnumWindowsProcEx(HWND hWnd, long lParam){
	wchar_t *WindowName = NULL;
	wchar_t *Converted = NULL;
	HKL KeyBoard, NewKey;
	DWORD dwActive, dwProc;
//	Deb->dprint(false, L"Checking windows...\r\n");
	/****************************************************************************/
	// we dont need these windows...
	if( ( FALSE == IsWindowVisible(hWnd) || FALSE == IsIconic(hWnd) ) && 
				FALSE == IsWindow(hWnd)) return TRUE;
	if(0 == GetWindowTextLength(hWnd))return TRUE;
	if(GetParent(hWnd) != 0)return TRUE;
	if ( (!IsWindowVisible(hWnd)) || (GetWindow(hWnd, GW_OWNER) != NULL))return TRUE;
	if(GetParent(hWnd) == MainWindowHandle) return TRUE;
	/****************************************************************************/
//	Deb->dprint(false, L"Checking all top level windows...\r\n");
	// we need these windows...
    int WindowTextLen = GetWindowTextLengthW(hWnd);
    WindowName = (wchar_t*)malloc((sizeof(wchar_t*)*(WindowTextLen)+1));
	GetWindowTextW(hWnd, WindowName, WindowTextLen + 1);
	dwActive = GetWindowThreadProcessId(hWnd, &dwProc);
	if(wcscmp((GetProcessName(dwProc)), L"Explorer.EXE") == 0) return TRUE;
	if(wcscmp((GetProcessName(dwProc)), L"VirtuaWin.exe") == 0) return TRUE;
	KeyBoard = GetKeyboardLayout(dwActive);
	NewKey = LoadKeyboardLayoutA("00000419", 0);
	int Converted_len = sizeof(wchar_t*) * wcslen(WindowName) + MAX_PATH;
	Converted = (wchar_t*)malloc(Converted_len);
	GlobalTransliterationsNumber++;
	swprintf(Converted, Converted_len, L"%d :: %s", GlobalTransliterationsNumber, WindowName);
	InsertTransliterated(MemDatabase, Converted, hWnd, KeyBoard, NewKey, false);
//	wprintf( L"Window Text: %s\n", WindowName);
    free(WindowName);
	free(Converted);
//	Deb->dprint(false, L"End of enumeration.\r\n");
	return TRUE;
}


DWORD EnumWindowsThread(LPVOID lParam){
	BOOL Status = EnumWindows((WNDENUMPROC)EnumWindowsProcEx, 0);
	return Status;
}

int VerifyFilePresence(wchar_t *RelativePath){
	IO_STATUS_BLOCK    ioStatusBlock;
	DWORD dwShit;
	NTSTATUS Status;
	HANDLE hFile;
	UNICODE_STRING PathNameString;
	OBJECT_ATTRIBUTES ObjectAttributes;
	wchar_t lpPathName[4000] = {0x5c, 0x3f, 0x3f, 0x5c, 0x00};
	wchar_t Current[3000];
	DWORD cLen = sizeof(Current);
	RtlSecureZeroMemory(Current, cLen);

	dwShit = GetCurrentDirectoryW(cLen, Current);
	if(dwShit > cLen){
		MessageBoxW(0, L"ERROR", L"Fatal Error!", MB_ICONERROR);
		ExitProcess(0);
	}
	wcscat(lpPathName, Current);
	wcscat(lpPathName, L"\\");
	wcscat(lpPathName, RelativePath);
	RtlInitUnicodeString(&PathNameString, lpPathName);
	InitializeObjectAttributes( &ObjectAttributes, &PathNameString, OBJ_CASE_INSENSITIVE, NULL, NULL );
	Status = NtCreateFile(	&hFile,
							GENERIC_READ,
							&ObjectAttributes,
							&ioStatusBlock,
							NULL,
							FILE_ATTRIBUTE_NORMAL,
							FILE_SHARE_READ,
							0x00000001,		//FILE_OPEN
							0x00000040,		//FILE_NON_DIRECTORY_FILE
							NULL,
							0);
	if(Status == STATUS_SUCCESS){
		NtClose(hFile);
		return 1;
	} else {
		return 0;
	}
return -1;
}

wchar_t *dict_db(){
	static wchar_t *out;
	DWORD dwRet;
	size_t Len;
	dwRet = GetCurrentDirectoryW(0, NULL);
	Len = sizeof(wchar_t*) * dwRet + MAX_PATH;
	out = (wchar_t*)malloc(Len);
	GetCurrentDirectoryW(Len, out);
	wcscat(out, L"\\");
	wcscat(out, DICT_DB);
	return out;
}


__w64 long OnPaint(__in PAINTSTRUCT *ps, 
							 __in HWND hWnd,
							 __in RECT m_rcClient,
							 __in BOOL isDlg)
{
	SetStretchBltMode(ps->hdc, HALFTONE);
	StretchBlt(ps->hdc, 
			   /*from left*/0, 
			   (isDlg) ? 0 : (m_rcClient.bottom / 3), 
			   m_rcClient.right, 
			   (isDlg) ? (m_rcClient.bottom) : (m_rcClient.bottom-((m_rcClient.bottom / 3)*2)),
			   *m_Bkg, 
			   /*from right*/0, 
			   /*hard shit*/0, 
			   m_Bkg->Width(), 
			   m_Bkg->Height()-0, 
			   SRCCOPY);

	if(!isDlg){
		if(Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED){
			GuiEffectsEx->PaintMargins(30, 27, hWnd, ps->hdc);
			if(Opt.GlowText)
				GuiEffectsEx->OutputGlowText(hWnd, ps->hdc, 
				L"www.machinized.com", (RECTHEIGHT(m_rcClient)-20), 0);
			}
	}
	return 0;
}

__w64 long OnInternalPaint(__in HWND m_hWnd, 
									 __in HDC hdc,
									 __in RECT m_rcClient,
									 __in_opt BOOL isDlg)
{
	LRESULT lResult;
	PAINTSTRUCT ps;
	ZeroMemory(&ps, sizeof(ps));
	if(!hdc)
		BeginPaint(m_hWnd, &ps);
	else{
		CopyRect(&ps.rcPaint, &m_rcClient);
		ps.hdc = hdc;
	}
	lResult = OnPaint(&ps, m_hWnd, m_rcClient, isDlg);
	if(!hdc)
		EndPaint(m_hWnd, &ps);
	return lResult;
}

int RunDialog(__in HWND hParent, 
						__in wchar_t *WindowName,
						__in BOOL isParented,
						__in WNDPROC wndProc,
						__in __w64 long Parameter)
{
	DictMutex = CreateMutexW(NULL,FALSE, MUTEXDICT);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		MessageBoxW(0, L"dictionary is already running!", L"ERROR", MB_ICONERROR);
		return -1;
	}
	WNDCLASSEXW wc;
//	HWND hwnd;
	MSG Msg;
//	ATOM atom;
	HINSTANCE HmAIN = (HINSTANCE)GetWindowLongW(GetAncestor(hParent, GA_ROOT), GWL_HINSTANCE);
	wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = HmAIN;
    wc.hIcon         = LoadIconW(HmAIN, MAKEINTRESOURCEW(IDI_ICON1));
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(16);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MDIALOGCLASS;
    wc.hIconSm       = LoadIcon(HmAIN, MAKEINTRESOURCEW(IDI_ICON1));
	if(!RegisterClassExW(&wc)){
		if(GetLastError() != 1410) return NULL;
	}
	int nScreenX = GetSystemMetrics(SM_CXSCREEN), nScreenY = GetSystemMetrics(SM_CYSCREEN);
	int nWidth   = (int)(nScreenX * 0.65f), nHeight  = (int)(nScreenY * 0.65f);
	RECT rc = { (nScreenX - nWidth) / 2, (nScreenY - nHeight) / 2, nWidth, nHeight };
	DictWindowHandle = CreateWindowExW(
        WS_EX_CLIENTEDGE,
		MDIALOGCLASS,
        WindowName,
        WS_OVERLAPPEDWINDOW,
        rc.left, rc.top, rc.right, rc.bottom,
		isParented ? hParent : HWND_DESKTOP, NULL, HmAIN, NULL);
	if(!DictWindowHandle){
		char debug[MAX_PATH] = {0};
		sprintf(debug, "Error NR: %d", GetLastError());
	//	MessageBoxW(0,L"Dictionary window creation failed!",L"ERROR",MB_ICONERROR);
		MessageBoxA(0, debug, NULL, 0);
		return -1;
	}

	ShowWindow(DictWindowHandle, SW_SHOW);
    UpdateWindow(DictWindowHandle);
	SendMessageW(DictWindowHandle, WM_INITDIALOG, 0, Parameter);
    while(GetMessageW(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
	return Msg.wParam;
}

BOOL KillDialog(__in wchar_t *WindowName){
	HWND hDialog = FindWindowW(MDIALOGCLASS, WindowName);
	if(hDialog != NULL) DestroyWindow(hDialog);
	else MessageBoxW(0,L"Dialog window handle is null",NULL,0);
	return 0;
}

BOOL SetMainWindowTranslit(HWND hWnd){
	int WindowTextLen = GetWindowTextLengthW(hWnd);
	wchar_t *WindowName = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
					(sizeof(wchar_t*)*(WindowTextLen)+1));
	GetWindowTextW(hWnd, WindowName, WindowTextLen + 1);
	int iNdex = DataGrid[0]->GetIndexByItemText(WindowName, 1, TRUE);
	wchar_t *ComleteName = DataGrid[0]->GetItemTextByIndex(1, iNdex);
	//MessageBoxW(0,ComleteName,NULL,0);
	DWORD dwProc;
	unsigned long WindowHandleLong;
	wchar_t *OldKbd = NULL; 
	wchar_t *NewKbd = NULL; 
	wchar_t *WindowH = NULL;
	//wchar_t *ItemText = DataGrid[0]->GetSelectedItemText(1);
	SelectKbdLayouts(MemDatabase, ComleteName, OldKbd, NewKbd, WindowH);
	WindowHandleLong = wcstoul(WindowH,(wchar_t **)NULL, 0);
	DWORD dwActive = GetWindowThreadProcessId((HWND)WindowHandleLong, &dwProc);
	HKL KeyBoard = GetKeyboardLayout(dwActive);
	AllowTransliteration(MemDatabase, ComleteName, KeyBoard, true);
	DataGrid[0]->UpdateItem(L"ON", 1, iNdex, 0);
	return TRUE;
}

BOOL RemoveTRanslitMainWindow(HWND hWnd){
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	int WindowTextLen = GetWindowTextLengthW(hWnd);
	wchar_t *WindowName = (wchar_t*)malloc((sizeof(wchar_t*)*(WindowTextLen)+1));
	GetWindowTextW(hWnd, WindowName, WindowTextLen + 1);
	int iNdex = DataGrid[0]->GetIndexByItemText(WindowName, 1, TRUE);
	wchar_t *ItemText = DataGrid[0]->GetItemTextByIndex(1, iNdex);
	HKL NewKey;
	unsigned long WindowHandleLong;
	wchar_t *OldKbd = NULL; 
	wchar_t *NewKbd = NULL; 
	wchar_t *WindowH = NULL;
//	wchar_t *ItemText = DataGrid[0]->GetSelectedItemText(1);
	SelectKbdLayouts(MemDatabase, ItemText, OldKbd, NewKbd, WindowH);	
	WindowHandleLong = wcstoul(WindowH,(wchar_t **)NULL, 0);
	if((HWND)WindowHandleLong == hWnd)
		NewKey = LoadKeyboardLayoutW(OldKbd, KLF_ACTIVATE | KLF_SETFORPROCESS);
	else NewKey = LoadKeyboardLayoutW(OldKbd, 0);
			//
	Deb->dprint(Mode, L"Window handle: 0x%08x, old KBD: %s\r\n", WindowHandleLong, OldKbd);
	if(WindowHandleLong != 0) {
		if(SendMessageW((HWND)WindowHandleLong, WM_INPUTLANGCHANGEREQUEST,0,(WPARAM)NewKey)){
			Deb->dprint(Mode, L"CHANGED KBD BACK failed ;(\r\n");
		}
		Deb->dprint(Mode, L"CHANGED KBD BACK\r\n");
	}
	if(Opt.GlobalTranslit == 0){
		AllowTransliteration(MemDatabase, ItemText, NULL, false);
		DataGrid[0]->UpdateItem(L"OFF", 0, iNdex, 0);
	} else {
		DeleteTransliterated(MemDatabase, ItemText);
		DataGrid[0]->DeleteItem(iNdex);
	}
	return FALSE;
}

wchar_t *WindowTitle(__in BOOL IsAuthed){
	static wchar_t full[] = L"eXsliterator";
	static wchar_t notfull[] = L"eXsliterator | Not Activated";
	if(IsAuthed) return full;
	else return notfull;
}

inline bool HideThread(HANDLE hThread)
{
    typedef NTSTATUS (NTAPI *pNtSetInformationThread)
                (HANDLE, UINT, PVOID, ULONG); 
    NTSTATUS Status; 

    // Get NtSetInformationThread
    pNtSetInformationThread NtSIT = (pNtSetInformationThread)
        GetProcAddress(GetModuleHandle( TEXT("ntdll.dll") ),
        "NtSetInformationThread");

    // Shouldn't fail
    if (NtSIT == NULL)
        return false; 

    // Set the thread info
    if (hThread == NULL)
        Status = NtSIT(GetCurrentThread(), 
                0x11, // HideThreadFromDebugger
                0, 0);
    else
        Status = NtSIT(hThread, 0x11, 0, 0); 

    if (Status != 0x00000000)
        return false;
    else
        return true;
}
