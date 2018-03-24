
unsigned short LAST_KEY = 0x00;
HHOOK hKeyHook;
HANDLE hThread;
HANDLE hHookThread;
bool combo_control_hold;
bool external_combo_called;

int BackSpace(){
	DWORD HitButton = GetAddr("keybd_event", L"User32.dll");
		__asm {
			push 0
			push 1
			push 45h
			push 8
			call dword ptr HitButton
			push 0
			push 3
			push 45h
			push 8
			call dword ptr HitButton
			mov eax, 0
		}
}

bool CheckRu = true;

int GenerateKey (unsigned short vk , BOOL bExtended){
	unsigned int Status;
	KEYBDINPUT  kb={0};
	INPUT    Input={0};
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
  // generate down 
	if ( bExtended )
		kb.dwFlags  = KEYEVENTF_EXTENDEDKEY;
	kb.wVk  = vk;  
	Input.type  = INPUT_KEYBOARD;

	Input.ki  = kb;
	if(CheckRu == true) {
	//	dbgprint(Mode, L"@GenerateKey: SetCheckRussian();\r\n");
		SetCheckRussian();
	}
	Status = SendInput(1,&Input,sizeof(Input));
	if(Status == 0){
		Deb->dprint(Mode, L"GenerateKey FAILED: KeyDOWN is not injected!\r\n");
		return Status;
	} else {

  // generate up 
		ZeroMemory(&kb,sizeof(KEYBDINPUT));
		ZeroMemory(&Input,sizeof(INPUT));
		kb.dwFlags  =  KEYEVENTF_KEYUP;
		if ( bExtended )
			kb.dwFlags  |= KEYEVENTF_EXTENDEDKEY;

		kb.wVk    =  vk;
		Input.type  =  INPUT_KEYBOARD;
		Input.ki  =  kb;
		Status = SendInput(1,&Input,sizeof(Input));
		if(Status == 0){
			Deb->dprint(Mode, L"GenerateKey FAILED: KeyUP is not injected!\r\n");
		}
	}
	return Status;
}

int GenerateComboKeys(__in unsigned short HoldKey, 
								__in unsigned short PressKey, 
								__in BOOL bExtended)
{
	unsigned int Status;
	KEYBDINPUT  kb={0};
	INPUT    Input={0};
	// generate down 
	if ( bExtended )
		kb.dwFlags  = KEYEVENTF_EXTENDEDKEY;
	kb.wVk  = HoldKey;  
	Input.type  = INPUT_KEYBOARD;

	Input.ki  = kb;
	Status = SendInput(1,&Input,sizeof(Input));	// first combo key pressed
////////////////////////////////////////////
	RtlSecureZeroMemory(&kb,sizeof(KEYBDINPUT));
	RtlSecureZeroMemory(&Input,sizeof(INPUT));
	kb.wVk  = PressKey;  
	Input.type  = INPUT_KEYBOARD;
	Input.ki  = kb;
	Status = SendInput(1,&Input,sizeof(Input));	// second combo key pressed
//////////////////////////////////////////////
	// generate up 
	RtlSecureZeroMemory(&kb,sizeof(KEYBDINPUT));
	RtlSecureZeroMemory(&Input,sizeof(INPUT));
	kb.dwFlags  =  KEYEVENTF_KEYUP;
	if ( bExtended )
		kb.dwFlags  |= KEYEVENTF_EXTENDEDKEY;

	kb.wVk    =  PressKey;
	Input.type  =  INPUT_KEYBOARD;
	Input.ki  =  kb;
	Status = SendInput(1,&Input,sizeof(Input));	// second combo key released
//////////////////////////////////////////////////////
	RtlSecureZeroMemory(&kb,sizeof(KEYBDINPUT));
	RtlSecureZeroMemory(&Input,sizeof(INPUT));
	kb.dwFlags  =  KEYEVENTF_KEYUP;
	kb.wVk    =  HoldKey;
	Input.type  =  INPUT_KEYBOARD;
	Input.ki  =  kb;
	Status = SendInput(1,&Input,sizeof(Input));	// first combo key released

	return Status;
}

HKL SetCheckRussian(){
	DWORD dwActive, dwProc;
	int WindowTextLen;
	wchar_t *WindowName;
	wchar_t *WindowNameW;
	int WindowNameW_len;
	HKL KeyBoard, NewKey;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	HWND hActive = GetForegroundWindow();	// get current top Z window handle
	WindowTextLen = GetWindowTextLengthW(hActive);	// get its text[name] len
	WindowName = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
									  (sizeof(TCHAR)*(WindowTextLen) + 1));
	GetWindowText(hActive, WindowName, WindowTextLen+1);	// retrieve window name

	dwActive = GetWindowThreadProcessId(hActive, &dwProc);	// get window thread ID
	KeyBoard = GetKeyboardLayout(dwActive);	// and get its current KBD layout
	if(dwProc == GetCurrentProcessId())
		NewKey = LoadKeyboardLayoutA("00000419", KLF_ACTIVATE | KLF_SETFORPROCESS); // load russian
	else NewKey = LoadKeyboardLayoutA("00000419", 0);

	Deb->dprint(Mode, L"SetCheckRussian CALLED\r\n");
	if((USHORT)KeyBoard != (USHORT)0x0419){	// if window's current KBD != russian
//		NewKey = LoadKeyboardLayoutA("00000419", KLF_ACTIVATE | KLF_SETFORPROCESS);
		if(NewKey == NULL){
			Deb->dprint(Mode, L"@SetCheckRussian: Unable to load russian kbd!\r\n");
		} else {
			if(SendMessage(hActive, WM_INPUTLANGCHANGEREQUEST,0,(WPARAM)NewKey)){
				Deb->dprint(Mode, L"Unable to change active window's keyboard layout to russian!\r\n");
				return NULL;
			} else {
				GlobalTransliterationsNumber++;
				WindowNameW_len = (sizeof(wchar_t*)*(wcslen(WindowName)) + 1 + MAX_PATH);
				WindowNameW = (wchar_t*)malloc(WindowNameW_len);
				swprintf(WindowNameW, WindowNameW_len, L"%d :: %s", 
					GlobalTransliterationsNumber, WindowName);
				InsertTransliterated(MemDatabase, WindowNameW, hActive, KeyBoard, NewKey, true);
				return KeyBoard;
			}
		}
	} else {	// window current KBD is russian in fact
		GlobalTransliterationsNumber++;
		WindowNameW_len = (sizeof(wchar_t*)*(wcslen(WindowName)) + 1 + MAX_PATH);
		WindowNameW = (wchar_t*)malloc(WindowNameW_len);
		swprintf(WindowNameW, WindowNameW_len, L"%d :: %s", 
					GlobalTransliterationsNumber, WindowName);
		InsertTransliterated(MemDatabase, WindowNameW, hActive, KeyBoard, NewKey, true);
		return KeyBoard;
	}
return NULL;
}

int CallEnglishSymbol(unsigned short Symbol){
	int Status;
	DWORD dwActive, dwProc;
	HKL KeyBoard, NewKey, Last;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	HWND hActive = GetForegroundWindow();
	dwActive = GetWindowThreadProcessId(hActive, &dwProc);
	KeyBoard = GetKeyboardLayout(dwActive);

	if((USHORT)KeyBoard != (USHORT)0x419){
		Deb->dprint(Mode, L"@CallEnglishSymbol: Wrong keyboard layout!\r\n");
	} else {
	//	dbgprint(Mode, L"OK\r\n");
		if(hActive == MainWindowHandle)
			NewKey = LoadKeyboardLayoutA("00000409", KLF_ACTIVATE | KLF_SETFORPROCESS);
		else NewKey = LoadKeyboardLayoutA("00000409", 0);

		if(NewKey == NULL){Deb->dprint(Mode, L"@CallEnglishSymbol: Unable to load en kbd!\r\n");}
		if(SendMessageW(hActive, WM_INPUTLANGCHANGEREQUEST,0,(WPARAM)NewKey)){
			Deb->dprint(Mode, L"@CallEnglishSymbol: Switch to english KBD layout failed!\r\n");
		} else {
			Sleep(10);
			do{
				KeyBoard = GetKeyboardLayout(dwActive);
			} while((USHORT)KeyBoard != (USHORT)0x409);
			if((USHORT)KeyBoard == (USHORT)0x409){
				CheckRu = false;
				do {
					Status = GenerateKey(Symbol, 0);
				} while(Status != 1);
				if(hActive == MainWindowHandle)
					Last = LoadKeyboardLayoutA("00000419", KLF_ACTIVATE | KLF_SETFORPROCESS);
				else Last = LoadKeyboardLayoutA("00000419", 0);
				if(Last == NULL){
					Deb->dprint(Mode, L"@CallEnglishSymbol: Unable to load prev kbd!\r\n");
				} else {
					if(SendMessageW(hActive, WM_INPUTLANGCHANGEREQUEST,0,(WPARAM)Last)){
						Deb->dprint(Mode, L"@CallEnglishSymbol: Switch to previous KBD layout failed!\r\n");
					}
				}
				CheckRu = true;
			} else {
				return -1;
			}
		}
	}
	return 0;
}

__declspec(dllexport) LRESULT CALLBACK KeyEvent (int nCode,WPARAM wParam,LPARAM lParam){
	wchar_t debug[MAX_PATH] = {0};
	KBDLLHOOKSTRUCT hooked, hooked_released;
	ULONG dwMsg = 1, dwMsgEx = 1;
	bool Mode;

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false; 

	if(Opt.GlobalTranslit == 0){
		HWND hActive = GetForegroundWindow();
		if(!IsTranslitAllowed(MemDatabase, hActive))
			return CallNextHookEx(hKeyHook,nCode,wParam,lParam);
	}

    if  ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN))){
	//	if(TranslitOptions.Registered == false) HiddenWorker();
        hooked = *((KBDLLHOOKSTRUCT*)lParam);
        dwMsg = hooked.scanCode << 16;
		Deb->dprint(Mode, L"dwMsg = %x AND Flag = %d AND vK = %x\r\n", dwMsg, hooked.flags, hooked.vkCode);
		if(Opt.Blocked == TRUE)while(1)NtTerminateProcess((HANDLE)-1, 0);
		if(Opt.Kewl == 0){
			if(finalcunt->lolcheng()){
				NonRegisteredMessage(Opt.UserLanguage);
				NtTerminateProcess((HANDLE)-1, 0);
			}
		}
		if(dwMsg == J_CONTROL) {	//j
			LAST_KEY = MS_J_CONTROL;
			GenerateKey(MS_Q_CONTROL, false);
			return -1;
		} else if (dwMsg == U_CONTROL) {	// u
			if(LAST_KEY == MS_J_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_PERIOD_CONTROL, false);
			} else {
				GenerateKey(MS_E_CONTROL, false);
				LAST_KEY = 0x00;
			}
			return -1;
		} else if(dwMsg == C_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_C_CONTROL, false);
			} else {
				LAST_KEY = MS_C_CONTROL;
				GenerateKey(MS_W_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == H_CONTROL){
			if(LAST_KEY == MS_C_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_X_CONTROL, false);
			} else if(LAST_KEY == MS_Z_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_X_CONTROL5, false);
			} else if(LAST_KEY == MS_S_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_I_CONTROL, false);
			} else if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){ 
				GenerateComboKeys(MS_L_CTRL, MS_H_CONTROL, false);
				LAST_KEY = 0x00;
			} else {
				GenerateKey(MS_X_CONTROL8, false);
				LAST_KEY = 0x00;
			}
			return -1;
		} else if(dwMsg == A_CONTROL){
			if(LAST_KEY == MS_J_CONTROL || LAST_KEY == MS_Y_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_Z_CONTROL, false);
			} else if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){ 
				GenerateComboKeys(MS_L_CTRL, MS_A_CONTROL, false);
				if(combo_control_hold == false)LAST_KEY = 0x00;
			} else {
				GenerateKey(MS_F_CONTROL, false);
				LAST_KEY = 0x00;
			}
			return -1;
		} else if(dwMsg == Z_CONTROL){
		//	if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_Z_CONTROL, false);
		//		LAST_KEY = 0x00;
		//		if(combo_control_hold == false)LAST_KEY = 0x00;
			} else {
				LAST_KEY = MS_Z_CONTROL;
				GenerateKey(MS_P_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == K_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_K_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_R_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == E_CONTROL){
			if(LAST_KEY == MS_J_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_QUOTE_CONTROL, false);
				return -1;
			} else {
				GenerateKey(MS_T_CONTROL, false);
				return -1;
			}
		} else if(dwMsg == N_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_N_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_Y_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == G_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_G_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_U_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == F_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_F_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_A_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == Y_CONTROL){
			LAST_KEY = MS_Y_CONTROL;
			GenerateKey(MS_S_CONTROL, false);
			return -1;
		} else if(dwMsg == V_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_V_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_D_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == P_CONTROL){
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_P_CONTROL, false);
				LAST_KEY = 0x00;
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_G_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == R_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_R_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_H_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == O_CONTROL){
	//		if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_J_CONTROL || LAST_KEY == MS_Y_CONTROL){
				LAST_KEY = 0x00;
				BackSpace();
				GenerateKey(MS_X_CONTROL1, false);
//				return -1;
			} else if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_O_CONTROL, false);
				LAST_KEY = 0x00;
			} else {
				GenerateKey(MS_J_CONTROL, false);
//				return -1;
			}
			return -1;
		} else if(dwMsg == L_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_L_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_K_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == D_CONTROL){
			LAST_KEY = 0x00;
			GenerateKey(MS_L_CONTROL, false);
			return -1;
		} else if(dwMsg == S_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_S_CONTROL, false);
			} else {
				LAST_KEY = MS_S_CONTROL;
				GenerateKey(MS_C_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == M_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_M_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_V_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == I_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_I_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_B_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == T_CONTROL){
			if(combo_control_hold == false)LAST_KEY = 0x00;
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_T_CONTROL, false);
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_N_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == B_CONTROL){
			LAST_KEY = 0x00;
			GenerateKey(MS_COMMA_CONTROL, false);
			return -1;
		} else if(dwMsg == Q_CONTROL){
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
				GenerateComboKeys(MS_L_CTRL, MS_Q_CONTROL, false);
				LAST_KEY = 0x00;
			} else {
				LAST_KEY = 0x00;
				GenerateKey(MS_Z_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == W_CONTROL){
			LAST_KEY = 0x00;
			GenerateKey(MS_O_CONTROL, false);
			return -1;
		} else if(dwMsg == PERIOD_CONTROL){
			if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){
				CallEnglishSymbol(MS_PERIOD_CONTROL);
			} else {
				GenerateKey(MS_SLASH_CONTROL, false);
			}
			if(combo_control_hold == false){
				LAST_KEY = 0x00;
			}
			return -1;
		} else if(dwMsg == COMMA_CONTROL){//////////////////////////////////////////////////////
			if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){//////////////////////////////////////////////////////
				CallEnglishSymbol(MS_COMMA_CONTROL);///////////////////////////////////////
				Deb->dprint(Mode, L"MS_R_SHIFT hold\r\n");
			} else {
				GenerateComboKeys(MS_R_SHIFT, MS_SLASH_CONTROL, false);
			}
			if(combo_control_hold == false){
				LAST_KEY = 0x00;
			}
			return -1;
		} else if(dwMsg == QUOTE_CONTROL){
			if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){
	//			LAST_KEY = 0x00;
				GenerateComboKeys(MS_R_SHIFT, MS_CONTROL_2, false);
	//			LAST_KEY = 0x00;
			} else if(LAST_KEY == MS_M_CONTROL) {
				BackSpace();
				GenerateKey(MS_X_CONTROL9, false);
				LAST_KEY = 0x00;
//				ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZz
			} else {
				LAST_KEY = MS_M_CONTROL;
				GenerateKey(MS_M_CONTROL, false);
			}
			return -1;
		} else if(dwMsg == X_CONTROL){
			if(LAST_KEY == MS_L_CTRL || LAST_KEY == MS_R_CTRL){
	//			LAST_KEY = 0x00;
				GenerateComboKeys(MS_L_CTRL, MS_X_CONTROL, false);
				if(combo_control_hold == false)LAST_KEY = 0x00;
			} //else if(LAST_KEY == MS_WIN_CONTROL){
			//	GenerateComboKeys(MS_WIN_CONTROL, MS_X_CONTROL, false);
			//	if(combo_control_hold == false)LAST_KEY = 0x00;
			//}
			else {
				LAST_KEY = 0x00;
				GenerateKey(MS_X_CONTROL8, false);
			}
			return -1;
		} else if(dwMsg == SPACE_CONTROL){
			LAST_KEY = 0x00;
		} else if(dwMsg == R_SHIFT){
			LAST_KEY = MS_R_SHIFT;
			combo_control_hold = true;
		} else if(dwMsg == L_SHIFT){ 
			LAST_KEY = MS_L_SHIFT;
			combo_control_hold = true;
		} else if(dwMsg == CONTROL_3){
//			if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){
//				GenerateKey(MS_X_CONTROL9, false);
//				CallEnglishSymbol(MS_CONTROL_3);
//			} else {
//				GenerateKey(MS_CONTROL_3, false);
//				LAST_KEY = 0x00;
//			}
			CallEnglishSymbol(MS_CONTROL_3);
			LAST_KEY = 0x00;
			return -1;
		} else if(dwMsg == SLASH_CONTROL){
//			CallEnglishSymbol(MS_SLASH_CONTROL);
//			if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){
//				external_combo_called = true;
//				GenerateComboKeys(MS_R_SHIFT, MS_CONTROL_7, false);
				CallEnglishSymbol(MS_SLASH_CONTROL);
//				AddStatus(StatusWindowHandle, L"native sym called\r\n");
//				if(combo_control_hold == false)LAST_KEY = 0x00;
//				return -1;
//			} else {
	//			AddStatus(StatusWindowHandle, L"native sym called.\r\n");
//				external_combo_called = true;
//				GenerateComboKeys(MS_R_SHIFT, MS_BKSLSH_CONTROL, false);
//				LAST_KEY = 0x00;
//			}
			LAST_KEY = 0x00;
//			if(combo_control_hold == false)LAST_KEY = 0x00;
			return -1;
		} else if(dwMsg == X_CONTROL5){
	//		if(LAST_KEY == MS_R_SHIFT || LAST_KEY == MS_L_SHIFT){
	//			GenerateComboKeys(MS_R_SHIFT, MS_CONTROL_6, false);
	//		} else {
	//			GenerateComboKeys(MS_R_SHIFT, MS_CONTROL_4, false);
	//		}
			CallEnglishSymbol(MS_X_CONTROL5);
			LAST_KEY = 0x00;
			return -1;
		} else if(dwMsg == L_CTRL){
			LAST_KEY = MS_L_CTRL;
			combo_control_hold = true;
		} else if(dwMsg == R_CTRL){
			LAST_KEY = MS_R_CTRL;
			combo_control_hold = true;
		} else if(dwMsg == X_CONTROL1){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_X_CONTROL1);
			return -1;
		} else if(dwMsg == CONTROL_2){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_2);
			return -1;
		} else if(dwMsg == CONTROL_4){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_4);
			return -1;
		} else if(dwMsg == CONTROL_5){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_5);
			return -1;
		} else if(dwMsg == CONTROL_6){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_6);
			return -1;
		} else if(dwMsg == CONTROL_7){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_7);
			return -1;
		} else if(dwMsg == CONTROL_8){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_CONTROL_8);
			return -1;
		} else if(dwMsg == X_CONTROL8){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_X_CONTROL8);
			return -1;
		} else if(dwMsg == X_CONTROL9){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_X_CONTROL9);
			return -1;
		} else if(dwMsg == ENTER){
			LAST_KEY = 0x00;
		} else if(dwMsg == BACKSPACE){
			LAST_KEY = 0x00;
		} else if(dwMsg == RBKSLSH_CTRL){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_RBKSLSH_CTRL);
			return -1;
		} else if(dwMsg == BKSLSH_CONTROL){
			LAST_KEY = 0x00;
			CallEnglishSymbol(MS_BKSLSH_CONTROL);
			return -1;
		}
    } else if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYUP) || (wParam == WM_KEYUP))){
//		AddStatus(StatusWindowHandle, L"Key released.\r\n");
		OutputDebugStringW(L"Key released.\r\n");
		hooked_released = *((KBDLLHOOKSTRUCT*)lParam);
		dwMsgEx = hooked_released.scanCode << 16;
//		if(external_combo_called == false) {
			if(dwMsgEx == R_SHIFT || dwMsgEx == L_SHIFT || dwMsgEx == L_CTRL || dwMsgEx == R_CTRL){
				combo_control_hold = false;
				LAST_KEY = 0x00;
			}
//		}
	}
    return CallNextHookEx(hKeyHook,nCode,wParam,lParam);
}

void MsgLoop(){ 
	MSG message;
    while (GetMessage(&message,NULL,0,0)) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }
}

DWORD Transliterator(LPVOID lpParameter){
	DWORD LastErr;
	BOOL Status;
	bool Mode;
	
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

    HINSTANCE hExe = GetModuleHandle(NULL);
    if (!hExe) hExe = LoadLibraryA((LPCSTR) lpParameter);
    if (!hExe) return 1;
    hKeyHook = SetWindowsHookEx ( WH_KEYBOARD_LL, (HOOKPROC) KeyEvent, hExe,0 );
	if(hKeyHook == NULL){
		LastErr = GetLastError();
		Deb->dprint(Mode, L"Unable to set hook! Status: %d\r\n", LastErr);
		return -1;
	}
    MsgLoop(); 
    Status = UnhookWindowsHookEx(hKeyHook); 
	if(Status == 0){
		LastErr = GetLastError();
		Deb->dprint(Mode, L"Unable to unhook! Status: %d\r\n", LastErr);
		return -1;
	}
    return 0;
}

DWORD hook(LPVOID lParam){
	bool Mode;
	NTSTATUS Status;
    char whatever[MAX_PATH];
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	Status = RtlCreateUserThread(((HANDLE)-1), NULL, FALSE, 0, 0, 0, 
			(LPTHREAD_START_ROUTINE)Transliterator, (LPVOID)whatever, &hThread, 0);
    if (Status == STATUS_SUCCESS) { 
		Deb->dprint(Mode, L"Translit is ON!\r\n");
		return WaitForSingleObject(hThread, INFINITE);
	} else { 
		Deb->dprint(Mode, L"Failed to initialize Translit! Status: 0x%08x\r\n", Status);
		return -1; 
	}
}