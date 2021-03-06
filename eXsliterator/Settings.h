#ifndef _SETTINGS_H
#define _SETTINGS_H

#define MODE_SET_AUTOSTART		0x00010000
#define MODE_CHECK_AUTOSTART	0x00020000
#define MODE_DEL_AUTOSTART		0x00030000
#define MAIN_REGISTRY_KEY		L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define TRANSLIT_REGKEY			L"Transliterator"
#define TRANSLIT_REGKEY_FULL	L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Transliterator"

DebugLog *Dbg;

int NtSetCheckDelAutostart(LONG SetCheckMode, HWND hWnd = NULL){
	NTSTATUS Status;
	BOOL Mode;
	UNICODE_STRING u_str;
	wchar_t ImagePath[1024] = L"\""; 
	wchar_t Myself[512];
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE hKey;
	ULONG dataLength = 0x80;
	unsigned long Disposition;
	ULONG structLength = sizeof(KEY_VALUE_BASIC_INFORMATION) + dataLength + sizeof(UNICODE_NULL);
	wchar_t debug[MAX_PATH];
	PKEY_VALUE_BASIC_INFORMATION pPartialValue = NULL;
	RtlSecureZeroMemory(debug, sizeof debug);
	Opt.DebugLogToFile == 1 ? Mode = TRUE : Mode = FALSE;

	if(SetCheckMode == MODE_CHECK_AUTOSTART){
		RtlInitUnicodeString(&u_str, MAIN_REGISTRY_KEY); 
		InitializeObjectAttributes(&ObjectAttributes, &u_str, OBJ_CASE_INSENSITIVE, NULL, NULL);
		Status = NtOpenKey(&hKey, KEY_ALL_ACCESS, &ObjectAttributes);
		if(Status != STATUS_SUCCESS){
			Dbg->dprint(Mode, L"[-] NtOpenKey Status: 0x%08x.\r\n", Status);
			return 0;
		} else {
			pPartialValue = (PKEY_VALUE_BASIC_INFORMATION) malloc(sizeof(WCHAR) * structLength);
			memset(pPartialValue, 0, structLength);
			RtlInitUnicodeString(&u_str, TRANSLIT_REGKEY); 
			Status = NtQueryValueKey(hKey, &u_str, KeyValueBasicInformation, pPartialValue, structLength, &dataLength);
			if(Status != STATUS_SUCCESS){
				Dbg->dprint(Mode, L"[+] NtQueryValueKey Status: 0x%08x.\r\n", Status);
				Dbg->dprint(Mode, L"[+] Looks like autostart is not set.\r\n");
				NtClose(hKey);
				return 0;
			} else {
				Dbg->dprint(Mode, L"[+] Autostart enabled.\r\n");
				NtClose(hKey);
				return 2;
			}
		}
	} else if(SetCheckMode == MODE_DEL_AUTOSTART){
		RtlInitUnicodeString(&u_str, MAIN_REGISTRY_KEY); 
		InitializeObjectAttributes(&ObjectAttributes, &u_str, OBJ_CASE_INSENSITIVE, NULL, NULL);
		Status = NtOpenKey(&hKey, KEY_SET_VALUE, &ObjectAttributes);
		if(Status != STATUS_SUCCESS){
			Dbg->dprint(Mode, L"[-] NtOpenKey Status: 0x%08x.\r\n", Status);
			return 0;
		} else {
			RtlInitUnicodeString(&u_str, TRANSLIT_REGKEY);
			Status = RtlDeleteRegistryValue(RTL_REGISTRY_HANDLE, (PCWSTR) hKey, TRANSLIT_REGKEY);
			if(Status != STATUS_SUCCESS){
				Dbg->dprint(Mode, L"[-] RtlDeleteRegistryValue Status: 0x%08x.\r\n", Status);
				Dbg->dprint(Mode, L"[-] Probably autostart already has been disabled.\r\n");
				return 1;
			} else {
				Dbg->dprint(Mode, L"[+] Deleted from autostart!\r\n");
				return 0;
			}
		}
	} else if(SetCheckMode == MODE_SET_AUTOSTART){
		RtlInitUnicodeString(&u_str, MAIN_REGISTRY_KEY); 
		InitializeObjectAttributes(&ObjectAttributes, &u_str, OBJ_CASE_INSENSITIVE, NULL, NULL);
		Status = NtCreateKey( &hKey, KEY_ALL_ACCESS, &ObjectAttributes, 0,  NULL, REG_OPTION_NON_VOLATILE, &Disposition);
		if(Status != STATUS_SUCCESS){
			Dbg->dprint(Mode, L"[-] NtCreateKey Status: 0x%08x.\r\n", Status);
			return 0;
		} else {
			GetModuleFileNameW(0, Myself, 512);
			wcscat(ImagePath, Myself);
			wcscat(ImagePath, L"\"");
			RtlInitUnicodeString(&u_str, TRANSLIT_REGKEY); 
			Status = NtSetValueKey(hKey, &u_str, 0, REG_SZ, ImagePath, wcslen(ImagePath) * sizeof(WCHAR) + sizeof(WCHAR));
			if(Status != STATUS_SUCCESS){
				Dbg->dprint(Mode, L"[-] NtSetValueKey Status: 0x%08x.\r\n", Status);
				return 0;
			} else {
				Dbg->dprint(Mode, L"[+] Added to autostart!\r\n");
				return 1;
			}
		}
	} else {
		Dbg->dprint(Mode, L"[-] Unknown Mode!\r\n");
	}
	return 0;
}

wchar_t * GetFileSelected(HWND hWnd){
	static wchar_t *FileOutput = NULL;
	OPENFILENAME ofn; 
	wchar_t szFile[10000]; 
	RtlSecureZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)==TRUE) {
		if(wcslen(ofn.lpstrFile) > 0){
			FileOutput = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(ofn.lpstrFile)+1);
			wcscpy(FileOutput, ofn.lpstrFile);
		}
	}
	return FileOutput;
}

eXslitOptions *eXop;

static BOOL SettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
		case WM_INITDIALOG:
			{
				Dbg = new DebugLog(FALSE);
				EntryHelper = new EntryControl(FALSE, FALSE);
				eXslitOptions *op = new eXslitOptions(L"\\Settings.ini");
				eXop = new eXslitOptions(L"\\Settings.ini");
				HWND hCombo1 = GetDlgItem(hWnd, IDC_COMBO1);
				HWND hCombo2 = GetDlgItem(hWnd, IDC_COMBO2);
				SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"English");
				SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"Polski");
				SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"Русский");
				SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"Default Look");
				SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"Vista Glass");
				if(Opt.LocalTranslationSearch)CheckDlgButton(hWnd, IDC_CHECK3, BST_CHECKED);
				if(Opt.DebugLogToFile)CheckDlgButton(hWnd, IDC_CHECK1, BST_CHECKED); 
				if(Opt.OnlineTranslationSearch)CheckDlgButton(hWnd, IDC_CHECK4, BST_CHECKED);
				if(Opt.GlobalTranslit)CheckDlgButton(hWnd, IDC_CHECK2, BST_CHECKED);
				if(Opt.GlowText)CheckDlgButton(hWnd, IDC_CHECK5, BST_CHECKED);
				if(Opt.ButtonsIcons)CheckDlgButton(hWnd, IDC_CHECK6, BST_CHECKED);
				if(Opt.Tooltips)CheckDlgButton(hWnd, IDC_CHECK7, BST_CHECKED);
				if(Opt.ColHeaders)CheckDlgButton(hWnd, IDC_CHECK8, BST_CHECKED);
				if(Opt.AutoUpdates) CheckDlgButton(hWnd, IDC_RADIO2, BST_CHECKED);

				wchar_t *TopGridBack = eXop->GetExtendedOption(L"TopGridBkg", L"value");
				wchar_t *BotGridBack = eXop->GetExtendedOption(L"BottomGridBkg", L"value");

				if(TopGridBack && wcslen(TopGridBack) > 0){
					SetWindowTextW(GetDlgItem(hWnd, IDC_BUTTON3), PathFindFileName(TopGridBack));
				} else {
					SetWindowTextW(GetDlgItem(hWnd, IDC_BUTTON3), L"Select...");
				}
				if(BotGridBack && wcslen(BotGridBack) > 0){
					SetWindowTextW(GetDlgItem(hWnd, IDC_BUTTON4), PathFindFileName(BotGridBack));
				} else {
					SetWindowTextW(GetDlgItem(hWnd, IDC_BUTTON4), L"Select...");
				}

				if(NtSetCheckDelAutostart(MODE_CHECK_AUTOSTART, hWnd) == 1){
					CheckDlgButton(hWnd, IDC_RADIO1, BST_CHECKED);
				}

				HWND hSlider1 = GetDlgItem(hWnd, IDC_SLIDER1);
				SendMessage(hSlider1, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(0, 10)); 
				SendMessage(hSlider1, TBM_SETPAGESIZE, 0, (LPARAM) 10);  
				SendMessage(hSlider1, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) Opt.ConnectionTimeOut / 10);  

				switch(Opt.UserLanguage){
					case ENGLISH:
						{
							SendMessage(hCombo1, CB_SETCURSEL, 0, 0);
						}
					break;

					case POLISH:
						{
							SendMessage(hCombo1, CB_SETCURSEL, 1, 0);
						}
					break;

					case RUSSIAN:
						{
							SendMessage(hCombo1, CB_SETCURSEL, 2, 0);
						}
					break;
				}
				if(Opt.VistaGlass == 10){
					SendMessage(hCombo2, CB_SETCURSEL, 1, 0);
				} else {
					SendMessage(hCombo2, CB_SETCURSEL, 0, 0);
				}
				wchar_t* CliName = op->GetExtendedOption(L"ClientName", L"value");
				if(!CliName || wcslen(CliName) == 0 || wcscmp(CliName, L"") == 0){
					SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
				} else {
					if(Opt.Kewl == TRUE){
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is activated.");
					} else {
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
					}
					EntryHelper->SetText(CliName, GetDlgItem(hWnd, IDC_EDIT2));
					delete op;
				}
				SetOptionsText(hWnd, Opt.UserLanguage);
				SetOptionsTooltips(hWnd, Opt.UserLanguage);
			}
		break;

		case WM_PAINT:
			{
			
			}
		break;

		case WM_SIZE:
			{
			
			}
		break;

		case WM_NOTIFY:
			{
			
			}
		break;

		case WM_HSCROLL:
			{
				HWND hSlider1 = GetDlgItem(hWnd, IDC_SLIDER1);
				wchar_t temp[MAX_PATH] = {0};
				int position = SendMessageW(hSlider1, TBM_GETPOS, (WPARAM) 0, (LPARAM)0);
				Opt.ConnectionTimeOut = position * 10;
				swprintf(temp, sizeof position, L"%d", position * 10);
				eXslitOptions *op = new eXslitOptions(L"\\Settings.ini");
				op->SetExtendedOption(L"ConnectionTimeOut", L"Value", temp);
				delete op;
			}
		break;

		case WM_LBUTTONDOWN:
			{
				__asm {
					push 0
					push HTCAPTION
					push WM_NCLBUTTONDOWN
					push hWnd
					call dword ptr SendMessageW
				}
			}
		break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) {
					case IDCANCEL:
						{
							wchar_t *CliName = EntryHelper->GetText(GetDlgItem(hWnd, IDC_EDIT2));
							if(CliName && wcslen(CliName) > 0){
								eXop->SetExtendedOption(L"ClientName", L"value", CliName);
							}
							DeleteOptionsTooltips();
							delete eXop;
							__asm {
								push 0
								push hWnd
								call dword ptr EndDialog
							}
						}
					break;

					case IDOK:
						{
							wchar_t *CliName = EntryHelper->GetText(GetDlgItem(hWnd, IDC_EDIT2));
							if(CliName && wcslen(CliName) > 0){
								eXop->SetExtendedOption(L"ClientName", L"value", CliName);
							}
							DeleteOptionsTooltips();
							delete eXop;
							__asm {
								push 0
								push hWnd
								call dword ptr EndDialog
							}
						}
					break;

					case IDC_BUTTON1:
						{
				//			wchar_t machine[] = { 0x68, 0x74, 0x74, 0x70, 0x3a, 
				//				0x2f, 0x2f, 0x6d, 0x61, 0x63, 0x68, 0x69, 0x6e, 
				//				0x69, 0x7a, 0x65, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 
				//				0x2f, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x73, 
				//				0x2e, 0x70, 0x68, 0x70, 0x3f, 0x70, 0x61, 0x67, 
				//				0x65, 0x3d, 0x72, 0x65, 0x67, 0x69, 0x73, 0x74, 
				//				0x65, 0x72, 0x00};
				//			wchar_t cmd[] = { 0x6f, 0x70, 0x65, 0x6e, 0x00};
				//			SHELLEXECUTEINFO sei = {
				//			sizeof(SHELLEXECUTEINFO),
				//			0,NULL,cmd,
				//			machine,
				//			NULL,NULL,SW_SHOWNORMAL,0,0,0,0,0,0,0};
				//			ShellExecuteExW(&sei);
						}
					break;

					case IDC_BUTTON2:	// checking for auth
						{	
							wchar_t *CliName = EntryHelper->GetText(GetDlgItem(hWnd, IDC_EDIT2));
							if(!CliName || wcslen(CliName) == 0 || wcscmp(CliName, L"") == 0){
								MessageBoxW(MainWindowHandle, ClientNameEmpty[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
							} else {
								char *shitnah = CheckStatusEx(TRUE, TRUE, CliName, hWnd, TRUE);
							}
						}
					break;

					case IDC_RADIO1:	// check / uncheck autostart
						{
							if(Opt.AutoStart == 0)
								Opt.AutoStart = NtSetCheckDelAutostart(MODE_SET_AUTOSTART);
							else if(Opt.AutoStart == 1)
								Opt.AutoStart = NtSetCheckDelAutostart(MODE_DEL_AUTOSTART);
						}
					break;

					case IDC_RADIO2:	// automatic updates
						{
							if(Opt.AutoStart){
								eXop->SetOption(FALSE, L"AutoCheckUpdates");
								Opt.AutoStart = 0;
							} else {
								eXop->SetOption(TRUE, L"AutoCheckUpdates");
								Opt.AutoStart = 1;
							}
						//	Opt.AutoStart = NtSetCheckDelAutostart(MODE_DEL_AUTOSTART);
						}
					break;

					case IDC_CHECK1:
						{
							if(Opt.DebugLogToFile){
								eXop->SetOption(FALSE, L"DebugLog");
								Opt.DebugLogToFile = 0;
							} else {
								eXop->SetOption(TRUE, L"DebugLog");
								Opt.DebugLogToFile = 1;
							}
						}
					break;

					case IDC_CHECK2:
						{
							if(Opt.GlobalTranslit){
								eXop->SetOption(FALSE, L"GlobalTranslit");
								Opt.GlobalTranslit = 0;
							} else {
								eXop->SetOption(TRUE, L"GlobalTranslit");
								Opt.GlobalTranslit = 1;
							}
						}
					break;

					case IDC_CHECK3:
						{
							if(Opt.LocalTranslationSearch){
								Opt.LocalTranslationSearch = 0;
								eXop->SetOption(FALSE, L"LocalTranslationSearch");
							} else {
								eXop->SetOption(TRUE, L"LocalTranslationSearch");
								Opt.LocalTranslationSearch = 1;
							}
						}
					break;

					case IDC_CHECK4:
						{
							if(Opt.OnlineTranslationSearch){
								Opt.OnlineTranslationSearch = 0;
								eXop->SetOption(FALSE, L"OnlineTranslationSearch");
							} else {
								Opt.OnlineTranslationSearch = 1;
								eXop->SetOption(TRUE, L"OnlineTranslationSearch");
							}
						}
					break;

					case IDC_CHECK5:
						{
							if(Opt.GlowText){
								Opt.GlowText = 0;
								eXop->SetOption(FALSE, L"BottomGlowText");
							} else {
								Opt.GlowText = 1;
								eXop->SetOption(TRUE, L"BottomGlowText");
							}
						}
					break;

					case IDC_CHECK6:
						{
							if(Opt.ButtonsIcons){
								Opt.ButtonsIcons = 0;
								eXop->SetOption(FALSE, L"ButtonsIcons");
							} else {
								Opt.ButtonsIcons = 1;
								eXop->SetOption(TRUE, L"ButtonsIcons");
							}
						}
					break;

					case IDC_CHECK7:	// tooltips
						{
							if(Opt.Tooltips){
								Opt.Tooltips = 0;
								eXop->SetOption(FALSE, L"ShowTooltips");
							} else {
								Opt.Tooltips = 1;
								eXop->SetOption(TRUE, L"ShowTooltips");
							}
						}
					break;

					case IDC_CHECK8:	// columns headers
						{
							if(Opt.ColHeaders){
								Opt.ColHeaders = 0;
								eXop->SetOption(FALSE, L"ColHeaders");
							} else {
								Opt.ColHeaders = 1;
								eXop->SetOption(TRUE, L"ColHeaders");
							}
						}
					break;

					case IDC_BUTTON3:
						{
							wchar_t *File = GetFileSelected(hWnd);
				//			LPWSTR *szArglist;
				//			int nArgs;
				//			szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
				//			MessageBoxW(0,GetCommandLineW()+10,GetCommandLineW(),0);
							if(File){
								SetWindowText(GetDlgItem(hWnd, IDC_BUTTON3), PathFindFileName(File));
								eXop->SetExtendedOption(L"TopGridBkg", L"value", File);
							}
						}
					break;

					case IDC_BUTTON4:
						{
							wchar_t *File = GetFileSelected(hWnd);
							if(File){
								SetWindowText(GetDlgItem(hWnd, IDC_BUTTON4), PathFindFileName(File));
								eXop->SetExtendedOption(L"BottomGridBkg", L"value", File);
							}						
						}
					break;

					case IDC_COMBO2:
						{
							GuiEffects *GUI = new GuiEffects();
							if ( HIWORD(wParam) == CBN_SELCHANGE ){
								int nCurSel = SendDlgItemMessage(hWnd, IDC_COMBO2, CB_GETCURSEL, 0, 0);
								if (nCurSel != -1){
									switch(nCurSel){
										case 0:
										{
									//		GuiEffects *GUI = new GuiEffects();
									//		Opt.VistaGlass = GUI->InitializeGlass(GetParent(hWnd), false);
											RedrawWindow(GetParent(hWnd), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
											eXop->SetOption(FALSE, L"VistaGlass");
									//		delete GUI;
										}
										break;

										case 1:
										{
									//		GuiEffects *GUI = new GuiEffects();
									//		Opt.VistaGlass = GUI->InitializeGlass(GetParent(hWnd), true);
											RedrawWindow(GetParent(hWnd), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
											eXop->SetOption(TRUE, L"VistaGlass");
									//		delete GUI;
										}
										break;
									}
								}
							}
							delete GUI;
						}
					break;

					case IDC_COMBO1:
						{
							if ( HIWORD(wParam) == CBN_SELCHANGE ){
								int nCurSel = SendDlgItemMessage(hWnd, IDC_COMBO1, CB_GETCURSEL, 0, 0);
								if (nCurSel != -1){
									switch(nCurSel){
										case 0:
										{
											ResetMainTooltips(ENGLISH);
											ResetOptionsTooltips(ENGLISH);
											eXop->SetExtendedOption(L"UserLanguage", L"value", L"EN");
											SetOptionsText(hWnd, ENGLISH);
										}
										break;

										case 1:
										{
											ResetMainTooltips(POLISH);
											ResetOptionsTooltips(POLISH);
											eXop->SetExtendedOption(L"UserLanguage", L"value", L"PL");
											SetOptionsText(hWnd, POLISH);
										}
										break;

										case 2:
										{
											ResetMainTooltips(RUSSIAN);
											ResetOptionsTooltips(RUSSIAN);
											eXop->SetExtendedOption(L"UserLanguage", L"value", L"RU");
											SetOptionsText(hWnd, RUSSIAN);
										}
										break;
									}
								}
							}
						}
					break;
				}
			}
		break;
	}
	return FALSE;
}

#endif 