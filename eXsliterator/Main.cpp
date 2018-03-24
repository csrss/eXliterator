#include "Headers.h"

int __stdcall TempFunction(){
	return 0;
}

__w64 long __stdcall CustomProcList(__in HWND hWnd, 
									__in unsigned int uMsg, 
									__in __w64 unsigned int wParam,
									__in __w64 long lParam, 
									__in __w64 unsigned int uIdSubclass, 
									__in __w64 unsigned long dwRefData)
{
    switch (uMsg)
    {
    	case WM_KEYUP:
			{
				if((wParam == VK_DELETE)){
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
					DataGrid[1]->DeleteItem(DataGrid[1]->GetSelectedIndex());
				}
			}
		break;
    } 
    
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

__w64 long __stdcall Cust0mPr0c(__in HWND hWnd, 
								__in unsigned int uMsg, 
								__in __w64 unsigned int wParam,
								__in __w64 long lParam, 
								__in __w64 unsigned int uIdSubclass, 
								__in __w64 unsigned long dwRefData)
{
    switch (uMsg)
    {
		case WM_KEYUP:
			{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
				if(!Opt.DictType){
					WordEntry[0]->Clear();
					MessageBoxW(MainWindowHandle, no_dict_error[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
					return -1;
				}
				if(Opt.Blocked == TRUE)while(1)NtTerminateProcess((HANDLE)-1, 0);
				wchar_t *pwstrText = NULL, *dict = NULL, *Translation = NULL;
				pwstrText =	WordEntry[0]->GetText();
				dict = GetDictTypeLong(Opt.DictType);
				Translation = SelectTranslation(PhyDataBase, pwstrText, dict);
				int iNdex = DataGrid[1]->GetIndexByItemText(pwstrText, 0, FALSE);
				if(Translation != NULL && iNdex == -1){
					int RootIndex = DataGrid[1]->InsertRootItem(pwstrText, 0);
					DataGrid[1]->InsertChildItem(Translation, RootIndex, 1);
				}

				if(wParam == VK_RETURN && iNdex == -1){
					PostMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(DGBTN_TRANSLATION_SEARCH, BN_CLICKED), 0);
				}
			}
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

__w64 long __stdcall WndProc(__in HWND hWnd, 
							 __in UINT msg, 
							 __in WPARAM wParam, 
							 __in LPARAM lParam)
{

	bool Mode;
	Opt.DebugLogToFile ? Mode = true : Mode = false;
	switch(msg){

		case WM_INITDIALOG:
			{

			}
		break;

		case WM_ACTIVATE:
			{
 
			}
		break;

		case WM_TIMER:
			{
#ifdef MONITOR
				switch(LOWORD(wParam)){
					case TIMER_DEF:
						{
							Opt.GlobalTrialTime--;
							if(Opt.GlobalTrialTime <= 0){
								KillTimer(hWnd, TIMER_DEF);
								NonRegisteredMessage(Opt.UserLanguage);
								NtTerminateProcess((HANDLE)-1, 0);
							} else {
								if(Opt.GlowText){
									wchar_t debug[MAX_PATH] = {0};
									swprintf(debug, MAX_PATH, L"Non-Registered Version - Time Left: %d s.", Opt.GlobalTrialTime);
									GuiEffectsEx->OutputGlowText(hWnd, GetDC(hWnd), 
											debug, (RECTHEIGHT(m_rcClient)-20), 0);
								}
							}
						}
					break;
				}
#endif
			}
		break;

		case WM_PRINTCLIENT:
			{
				OnInternalPaint(hWnd, (HDC)wParam, m_rcClient);
			}
		break;

		case WM_CREATE:
			{
				Tray = new TrayControl(hWnd);
				if(Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED){
					Opt.VistaGlass = GuiEffectsEx->InitializeGlass(hWnd, true);
					Deb->dprint(FALSE, L"Init glass status: %d", Opt.VistaGlass);
				}
				DataGrid[0]->Create(hWnd, 0, 0, 0, 0, TRANSLITERATED_DATAGRID, FALSE, FALSE, 10);
				GuiEffectsEx->ApplyExplorerStyle(DataGrid[0]->operator HWND());
				DataGrid[0]->AddColumn(100, grid_status[Opt.UserLanguage], 0);
				DataGrid[0]->AddColumn(400, grid_window[Opt.UserLanguage], 1);
				DataGrid[0]->SetBkgImage(Opt.TopGridBackground);
				DataGrid[0]->SetFullRowSelection();
				DataGrid[0]->InitImageList();
				DataGrid[0]->AddIconToImageList(IDI_ICON4);
				DataGrid[0]->AddIconToImageList(IDI_ICON5);
				DataGrid[0]->SetImageList();

				DataGrid[1]->Create(hWnd, 0, 0, 0, 0, TRANSLATIONS_DATAGRID, FALSE, FALSE, 10);
				GuiEffectsEx->ApplyExplorerStyle(DataGrid[1]->operator HWND());
				DataGrid[1]->AddColumn(100, grid_word[Opt.UserLanguage], 0);
				DataGrid[1]->AddColumn(400, grid_translation[Opt.UserLanguage], 1);
				DataGrid[1]->SetBkgImage(Opt.BottomGridBaclground);
				DataGrid[1]->SetFullRowSelection();
				DataGrid[1]->InitImageList();
				DataGrid[1]->AddIconToImageList(IDI_ICON6);
				DataGrid[1]->SetImageList();

				SetWindowSubclass(DataGrid[1]->operator HWND(), CustomProcList, 0, 0);

				DarkGlasButton[0]->PaintBtnControl(DarkGlasButton[0]->GlassButton, 
					hWnd, quit_button_name[Opt.UserLanguage], 0, 0, 100, 80, DGBTN_QUIT);
				DarkGlasButton[1]->PaintBtnControl(DarkGlasButton[1]->GlassButton, 
					hWnd, settings_button_name[Opt.UserLanguage], 0, 0, 100, 80, DGBTN_SETTINGS);
				DarkGlasButton[2]->PaintBtnControl(DarkGlasButton[2]->GlassButton, 
					hWnd, dictdb_button_name[Opt.UserLanguage], 0, 0, 100, 80, DGBTN_DICTDB);
				DarkGlasButton[3]->PaintBtnControl(DarkGlasButton[3]->GlassButton, 
					hWnd, fetch_button_name[Opt.UserLanguage], 0, 0, 100, 80, DGBTN_TRANSLATION_SEARCH);
				DarkGlasButton[4]->PaintBtnControl(DarkGlasButton[4]->GlassButton, 
					hWnd, translation_type[Opt.UserLanguage], 0, 0, 200, 50, GLASS_COMBO_DICT_TYPE);
				DarkGlasButton[5]->PaintBtnControl(DarkGlasButton[4]->GlassButton,
					hWnd, L"", 0, 0, 100, 100, RADIO_TRANSLIT);

				DarkGlasButton[4]->SetGlowColor(0xff7CFC00);
				DarkGlasButton[3]->SetGlowColor(0xff8DBDFF);
				DarkGlasButton[2]->SetGlowColor(0xff00CD66);
				DarkGlasButton[1]->SetGlowColor(0xffFF9912);
				DarkGlasButton[0]->SetGlowColor(0xffFF0000);

				DarkGlasButton[5]->SetBtnIcon(IDI_ICON2, DarkGlasButton[5]->ICON_POSITION_CENTER, 0, 0, 100, 100);

				if(Opt.ButtonsIcons){
					DarkGlasButton[4]->SetBtnIcon(IDI_ICON18, DarkGlasButton[4]->ICON_POSITION_RIGHT);
					DarkGlasButton[3]->SetBtnIcon(IDI_ICON10, DarkGlasButton[3]->ICON_POSITION_RIGHT_UP);
					DarkGlasButton[2]->SetBtnIcon(IDI_ICON8, DarkGlasButton[2]->ICON_POSITION_RIGHT_UP);
					DarkGlasButton[1]->SetBtnIcon(IDI_ICON9, DarkGlasButton[1]->ICON_POSITION_RIGHT_UP);
					DarkGlasButton[0]->SetBtnIcon(IDI_ICON7, DarkGlasButton[0]->ICON_POSITION_RIGHT_UP);
				}

				WordEntry[0]->Create(hWnd, 0, 0, 0, 0, ENTRY_CONTROL, FALSE, FALSE, 10);

				SetMainTooltips(hWnd, Opt.UserLanguage);
				Tray->InitTray(IDI_ICON12, L"eXsliteratorTrayInitialized");
				Tray->AddMenuItem(TRAY_TOGLEGLOBALTRANSLIT, tray_global_translit[Opt.UserLanguage]);
				Tray->AddSeparator();
				Tray->AddMenuItem(TRAY_APPRESTORE, tray_restore_window[Opt.UserLanguage]);
				Tray->AddMenuItem(TRAY_APPQUIT, tray_quit[Opt.UserLanguage]);

				SetWindowSubclass(WordEntry[0]->operator HWND(), Cust0mPr0c, 0, 0);
				m_Bkg = new ResImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDR_KEYBOARD), RT_RCDATA);
				PostMessageW(hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(680, 460));
				wchar_t *username_temp = eXoptions->GetExtendedOption(L"ClientName", L"value");
				if(username_temp == NULL || wcscmp(username_temp, L"") == 0 || wcslen(username_temp) == 0){
					Opt.Kewl = FALSE;
					username_temp = NULL;
				//	MessageBoxW(0,L"chuj dupa kurwa cipa!",NULL,0);
				} 
				DWORD threadId;
				HANDLE hThread;
				//	NTSTATUS ThSt = RtlCreateUserThread(((HANDLE)-1), NULL, FALSE, 0, 0, 0, 
				//					(LPTHREAD_START_ROUTINE)FetchStatus, 
				//					(LPVOID)username_temp, NULL, 0);
					hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)FetchStatus, 
						(LPVOID)username_temp, 0, &threadId);
				if(!hThread){
					Deb->dprint(false, L"FetchStatus ERROR\r\n");
				} else {
					HideThread(hThread);
				}
			}
		break;

		case WM_CTLCOLOREDIT:
			{
				if(Opt.wVersion >= 6){
					if ((HWND)lParam == WordEntry[0]->operator HWND()){
						SetDCBrushColor((HDC)wParam, RGB(0, 0, 0)); // RGB(205, 201, 201)
						return (BOOL)GetStockObject(DC_BRUSH);
					}
				}
			}
		break;

		case WM_GETMINMAXINFO:
			{
				MINMAXINFO *pMinMax = (MINMAXINFO*)lParam;
				pMinMax->ptMinTrackSize.x = 700;
				pMinMax->ptMinTrackSize.y = 500;
			}
		break;

		case WM_PAINT:
			{
				OnInternalPaint(hWnd, NULL, m_rcClient);
			}
		break;

		case WM_SIZE:
			{
				RECT lpRect;
				GetClientRect(hWnd, &lpRect);
				GetClientRect(hWnd, &m_rcClient);
				int ClientHeight;
				int ClientWidth = lpRect.right;
				if(Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED){
					ClientHeight = lpRect.bottom - 30;
					DataGrid[0]->SetPosition(0, 30);
				} else { 
					ClientHeight = lpRect.bottom;
					DataGrid[0]->SetPosition(0, 0);
				}

				DataGrid[0]->SetSize(ClientWidth-0, ClientHeight / 3);
				DataGrid[1]->SetPosition(0, DataGrid[0]->Height() + (ClientHeight / 3));
				DataGrid[1]->SetSize(ClientWidth-0, (ClientHeight / 3));

				int DarkGlasButton_width = DarkGlasButton[0]->Width();
				int Fuck = (ClientHeight / 2) - ((DarkGlasButton[0]->Height() / (Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED ? 3 : 2)) - 
					(DarkGlasButton[0]->Height() / 21));
				int pr0n = (ClientHeight / 2) - ((DarkGlasButton[5]->Height() / (Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED ? 3 : 2)));
				DarkGlasButton[0]->SetPosition(ClientWidth - (DarkGlasButton_width)*1 - 5*1, Fuck);
				DarkGlasButton[1]->SetPosition(ClientWidth - (DarkGlasButton_width)*2 - 5*2, Fuck);
				DarkGlasButton[2]->SetPosition(ClientWidth - (DarkGlasButton_width)*3 - 5*3, Fuck);
				DarkGlasButton[3]->SetPosition(ClientWidth - (DarkGlasButton_width)*4 - 5*4, Fuck);

				DarkGlasButton[5]->SetPosition(10, pr0n);
#ifdef FRAME
				Frame[0]->SetPosition(140, pr0n-3);
				Frame[0]->SetSize((ClientWidth - 570), 100);
#endif
				WordEntry[0]->SetPosition(120, Opt.VistaGlass == GuiEffectsEx->GLASS_ENABLED ? pr0n+67 : pr0n+70);
				WordEntry[0]->SetSize((ClientWidth - 550), 20);
				DarkGlasButton[4]->SetPosition(120-2, Fuck);
				DarkGlasButton[4]->SetSize((ClientWidth - 550)+4, 50);
		//		TranslationDir[0]->SetPosition(150, pr0n+20);
		//		TranslationDir[0]->SetSize((ClientWidth - 590), 20);
				InvalidateRect(hWnd, NULL, TRUE);
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
				switch(LOWORD(wParam)){

					case RADIO_TRANSLIT:
						{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
							if(Opt.TranslitIsON == 0 && !hHookThread){
								NTSTATUS Status;
								CLIENT_ID ClientID;
								Status = RtlCreateUserThread(((HANDLE)-1), 
															NULL, FALSE, 0, 0, 0, 
															(LPTHREAD_START_ROUTINE)hook, 
															NULL, &hHookThread, &ClientID);
								if(Status == STATUS_SUCCESS){
									Opt.TranslitIsON = 1;
									DarkGlasButton[5]->RemoveBtnIcon();
									DarkGlasButton[5]->SetBtnIcon(IDI_ICON3, DarkGlasButton[5]->ICON_POSITION_CENTER, 0, 0, 100, 100);
									Deb->dprint(Mode, L"Transliteration thread created!\r\n");
									Tray->MenuItemCheck(TRAY_TOGLEGLOBALTRANSLIT, TRUE);
									Tray->SwitchIcon(IDI_ICON11);
								} else {
									Deb->dprint(Mode, L"Error launching transliteration! Status: 0x%08x\r\n", Status);
								}
								if(Opt.GlobalTranslit == 0){
									EnumWindows((WNDENUMPROC)EnumWindowsProcEx, 0);
								}
							} else if(Opt.TranslitIsON == 1 && hHookThread){
								NTSTATUS st, ts;
								st = NtTerminateThread(hThread, 0);
								ts = NtTerminateThread(hHookThread, 0);
								if(st == STATUS_SUCCESS && ts == STATUS_SUCCESS){
									Opt.TranslitIsON = 0;
									DarkGlasButton[5]->RemoveBtnIcon();
									DarkGlasButton[5]->SetBtnIcon(IDI_ICON2, DarkGlasButton[5]->ICON_POSITION_CENTER, 0, 0, 100, 100);
									hHookThread = NULL;
									Tray->MenuItemCheck(TRAY_TOGLEGLOBALTRANSLIT, FALSE);
									Tray->SwitchIcon(IDI_ICON12);
									Deb->dprint(Mode, L"Transliteration threads are terminated!\r\n");
									RemoveAllTransliterations(hWnd);
								} else {
									Deb->dprint(Mode, L"Error terminating transliteration! Statuses: 0x%08x and 0x%08x.\r\n", st, ts);
								}
							}
						}
					break;

					case GLASS_COMBO_DICT_TYPE:
						{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
							int status = CMenu[2]->Create();
							CMenu[2]->AddItem(DICTCTX_EN_PL, dictype_en_pl[Opt.UserLanguage]);
							CMenu[2]->AddItem(DICTCTX_EN_RU, dictype_en_ru[Opt.UserLanguage]);
							CMenu[2]->AddItem(DICTCTX_PL_EN, dictype_pl_en[Opt.UserLanguage]);
							CMenu[2]->AddItem(DICTCTX_PL_RU, dictype_pl_ru[Opt.UserLanguage]);
							CMenu[2]->AddItem(DICTCTX_RU_EN, dictype_ru_en[Opt.UserLanguage]);
							CMenu[2]->AddItem(DICTCTX_RU_PL, dictype_ru_pl[Opt.UserLanguage]);
						
							CMenu[2]->SetItemBitmap(DICTCTX_EN_PL, IDB_BITMAP1, hWnd);
							CMenu[2]->SetItemBitmap(DICTCTX_EN_RU, IDB_BITMAP1, hWnd);
							CMenu[2]->SetItemBitmap(DICTCTX_PL_EN, IDB_BITMAP1, hWnd);
							CMenu[2]->SetItemBitmap(DICTCTX_PL_RU, IDB_BITMAP1, hWnd);
							CMenu[2]->SetItemBitmap(DICTCTX_RU_EN, IDB_BITMAP1, hWnd);
							CMenu[2]->SetItemBitmap(DICTCTX_RU_PL, IDB_BITMAP1, hWnd);
							unsigned int Clicked = CMenu[2]->TrackMenuHit2(hWnd, DarkGlasButton[4]->operator HWND());
							switch(Clicked){
								case DICTCTX_EN_PL:
									{
										//Opt.TempTranslit = 0;
										Opt.DictType = EN_PL;
										DarkGlasButton[4]->SetBtnText(dictype_en_pl[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_en_pl[Opt.UserLanguage]+1);
										WordEntry[0]->Clear();
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {
											if(Opt.TempTranslit) Opt.TempTranslit = RemoveTRanslitMainWindow(hWnd);
										}
									}
								break;
								case DICTCTX_EN_RU:
									{
										//Opt.TempTranslit = 0;
										Opt.DictType = EN_RU;
										DarkGlasButton[4]->SetBtnText(dictype_en_ru[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_en_ru[Opt.UserLanguage]+1);
										WordEntry[0]->Clear();
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {
											if(Opt.TempTranslit) Opt.TempTranslit = RemoveTRanslitMainWindow(hWnd);
										}
									}
								break;
								case DICTCTX_PL_EN:
									{
										//Opt.TempTranslit = 0;
										Opt.DictType = PL_EN;
										DarkGlasButton[4]->SetBtnText(dictype_pl_en[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_pl_en[Opt.UserLanguage]+1);
										WordEntry[0]->Clear();
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {
											if(Opt.TempTranslit) Opt.TempTranslit = RemoveTRanslitMainWindow(hWnd);
										}
									}
								break;
								case DICTCTX_PL_RU:
									{
										//Opt.TempTranslit = 0;
										Opt.DictType = PL_RU;
										DarkGlasButton[4]->SetBtnText(dictype_pl_ru[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_pl_ru[Opt.UserLanguage]+1);
										WordEntry[0]->Clear();
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {
											if(Opt.TempTranslit) Opt.TempTranslit = RemoveTRanslitMainWindow(hWnd);
										}
									}
								break;
								case DICTCTX_RU_EN:	// here we should run transliteration for exslit
									{
										Opt.DictType = RU_EN;
										//Opt.TempTranslit = 1;
										DarkGlasButton[4]->SetBtnText(dictype_ru_en[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_ru_en[Opt.UserLanguage]+1);
										/* so first we are checking if main transliteration is ON */
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {	// translit is ON
											if(!Opt.TempTranslit) Opt.TempTranslit = SetMainWindowTranslit(hWnd);
										}
										WordEntry[0]->Clear();
									}
								break;
								case DICTCTX_RU_PL:	// run translit for our window
									{
										//Opt.TempTranslit = 1;
										Opt.DictType = RU_PL;
										DarkGlasButton[4]->SetBtnText(dictype_ru_pl[Opt.UserLanguage]+1);
										if(DlgDGButton[5])DlgDGButton[5]->SetBtnText(dictype_ru_pl[Opt.UserLanguage]+1);
										if(Opt.TranslitIsON == 0 && !hHookThread){

										} else {
											if(!Opt.TempTranslit) Opt.TempTranslit = SetMainWindowTranslit(hWnd);
										}
										WordEntry[0]->Clear();
									}
								break;
							}
							CMenu[2]->Destroy();
						}						
					break;
 
					case DGBTN_DICTDB:
						{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
							RunDialog(	hWnd, 
										L"eXsliterator Dictionary", 
										FALSE, 
										DictionaryProc,
										(LPARAM)((DWORD)MAKELONG(0, Opt.DictType)));
						}
					break;

					case DGBTN_SETTINGS:
						{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
							DialogBoxParamW(GetModuleHandle(NULL), 
											MAKEINTRESOURCE(IDD_DIALOG1), 
											hWnd, 
											(DLGPROC)SettingsProc, 
											MAKELPARAM(0, 0));
						//	BOOL Status = EnumWindows((WNDENUMPROC)EnumWindowsProcEx, 0);
						//	if(!Status)MessageBoxW(0,L"dfvdf",NULL,0);
						}
					break;

					case DGBTN_QUIT:
						{
						//	wchar_t machine[] = L"http://machinized.com/thanks.php";
							/*
							wchar_t machine[] = { 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 
								0x6d, 0x61, 0x63, 0x68, 0x69, 0x6e, 0x69, 0x7a, 0x65, 0x64, 
								0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x74, 0x68, 0x61, 0x6e, 0x6b, 
								0x73, 0x2e, 0x70, 0x68, 0x70, 0x00};
							wchar_t cmd[] = { 0x6f, 0x70, 0x65, 0x6e, 0x00};
							SHELLEXECUTEINFO sei = {
							sizeof(SHELLEXECUTEINFO),
							0,NULL,cmd,
							machine,
							NULL,NULL,SW_SHOWNORMAL,0,0,0,0,0,0,0};
							ShellExecuteExW(&sei);
							*/ 
							RemoveAllTransliterations(hWnd);
							DeleteMainTooltips();
							DestroyWindow(hWnd);
							KillDialog(L"eXsliterator Dictionary");
							ExitProcess(0);
					//		__asm {
					//			push 0
					//			push -1
					//			call dword ptr NtTerminateProcess
					//		}
						}
					break;

					case DGBTN_TRANSLATION_SEARCH:
						{
#ifdef MONITOR
					if(!Opt.Kewl){
						if(finalcunt->lolcheng()){
							NonRegisteredMessage(Opt.UserLanguage);
							NtTerminateProcess((HANDLE)-1, 0);
							if(1){
								/* critical moment, notify author, api must be altered */
								TempFunction();
							}
						}
					}
#endif
							wchar_t *word = WordEntry[0]->GetText();
							int iNdex = DataGrid[1]->GetIndexByItemText(word, 0, FALSE);
							if(word == NULL || wcscmp(word, L"") == 0 || 
								wcslen(word) == 0 || !Opt.DictType){
								MessageBox(hWnd, L"Either word or dictionary type is not specified!", L"ERROR", MB_ICONERROR);
							} else {
								if(iNdex == -1){
									if(Opt.OnlineTranslationSearch == TRUE){
										MakeTranslationRequest(Opt.DictType, word);
									} else {
										MessageBoxW(hWnd, L"Online search is disabled in application options!", L"INFORMATION", MB_ICONINFORMATION);
									}
								} else {
									MessageBoxW(hWnd,WordIsThere[Opt.UserLanguage],L"INFORMATION",MB_ICONINFORMATION);
								}
							}
						}
					break;
				}
			}
		break;

		case WM_NOTIFY:
			{
				switch(LOWORD(wParam)){
					case TRANSLITERATED_DATAGRID:
						{
							if(((LPNMHDR)lParam)->code == NM_DBLCLK){
								NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)lParam;
								if (nmia->iItem != -1){

								}
							} else if(((LPNMHDR)lParam)->code == NM_RCLICK){
								if(Opt.TranslitIsON != 1){
									return -1;
								}
								unsigned int Clicked;
								int status = CMenu[0]->Create();
								//Deb->dprint(false, L"CMenu status: %d", status);
								NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)lParam;
								if (nmia->iItem != -1){
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_TRSET, cntx_menu_settr[Opt.UserLanguage]);
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_TRUNSET, cntx_menu_unsettr[Opt.UserLanguage]);
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_RMITEM, cntx_menu_remove[Opt.UserLanguage]);
									CMenu[0]->AddSeparator();
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_REFRESH, cntx_menu_reload[Opt.UserLanguage]);
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_CLEARALL, cntx_menu_clear[Opt.UserLanguage]);
								} else {
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_REFRESH, cntx_menu_reload[Opt.UserLanguage]);
									CMenu[0]->AddItem(TRANSLIT_CONTEXT_CLEARALL, cntx_menu_clear[Opt.UserLanguage]);	
								}
								Clicked = CMenu[0]->TrackMenuHit(hWnd);
								switch(Clicked){
									case TRANSLIT_CONTEXT_TRSET:
										{
											DWORD dwProc;
											unsigned long WindowHandleLong;
											wchar_t *OldKbd = NULL; 
											wchar_t *NewKbd = NULL; 
											wchar_t *WindowH = NULL;
											wchar_t *ItemText = DataGrid[0]->GetSelectedItemText(1);
											SelectKbdLayouts(MemDatabase, ItemText, OldKbd, NewKbd, WindowH);
											WindowHandleLong = wcstoul(WindowH,(wchar_t **)NULL, 0);
											DWORD dwActive = GetWindowThreadProcessId((HWND)WindowHandleLong, &dwProc);
											HKL KeyBoard = GetKeyboardLayout(dwActive);
											AllowTransliteration(MemDatabase, ItemText, KeyBoard, true);
											DataGrid[0]->UpdateItem(L"ON", 1, DataGrid[0]->GetSelectedIndex(), 0);
										}
									break;

									case TRANSLIT_CONTEXT_TRUNSET:
										{
											HKL NewKey;
											unsigned long WindowHandleLong;
											wchar_t *OldKbd = NULL; 
											wchar_t *NewKbd = NULL; 
											wchar_t *WindowH = NULL;
											wchar_t *ItemText = DataGrid[0]->GetSelectedItemText(1);
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
												DataGrid[0]->UpdateItem(L"OFF", 0, DataGrid[0]->GetSelectedIndex(), 0);
											} else {
												DeleteTransliterated(MemDatabase, ItemText);
												DataGrid[0]->DeleteItem(DataGrid[0]->GetSelectedIndex());
											}
										}
									break;

									case TRANSLIT_CONTEXT_RMITEM:
										{
											DataGrid[0]->DeleteItem(DataGrid[0]->GetSelectedIndex());
										}
									break;

									case TRANSLIT_CONTEXT_REFRESH:
										{
											EnumWindows((WNDENUMPROC)EnumWindowsProcEx, 0);
										}
									break;

									case TRANSLIT_CONTEXT_CLEARALL:
										{
											GlobalTransliterationsNumber = 0;
											CloseDatabaseConnection(MemDatabase);
											MemDatabase = InitInMemoryDatabase();
											DataGrid[0]->ClearDataGrid();
										}
									break;
								}
								CMenu[0]->Destroy();
							}
						}
					break;

					case TRANSLATIONS_DATAGRID:
						{
							if(((LPNMHDR)lParam)->code == NM_DBLCLK){
								NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)lParam;
								if (nmia->iItem != -1){
									int iNdex = DataGrid[1]->GetSelectedIndex();
									RunDialog(	hWnd, 
										L"eXsliterator Dictionary", 
										FALSE, 
										DictionaryProc,
										(LPARAM)((DWORD)MAKELONG(iNdex, Opt.DictType)));
								}
							} else if(((LPNMHDR)lParam)->code == NM_RCLICK){
								NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)lParam;
								if (nmia->iItem != -1){

								}
							}
						}
					break;
				}
			}
		break;

		case WM_CLOSE:
			{
			//	DestroyWindow(hWnd);
				Tray->MinimizeWindow();
			}
		break;

		case WM_TRAYICON:
			{
				unsigned int Clicked;
				if (lParam == WM_LBUTTONUP){
					Tray->RestoreWindow();
				} else if (lParam == WM_RBUTTONDOWN){
					Clicked = Tray->TrackMenuHit();
					switch(Clicked){
						case TRAY_TOGLEGLOBALTRANSLIT:
							{
								BOOL MessageStatus;
								MessageStatus = PostMessageW(hWnd, WM_COMMAND, 
								(WPARAM)MAKEWPARAM(RADIO_TRANSLIT, BN_CLICKED), 
								(LPARAM)GetDlgItem(hWnd, RADIO_TRANSLIT));
							}
						break;

						case TRAY_APPQUIT:
							{
								Tray->RemoveIcon();
								PostMessageW(hWnd, WM_COMMAND, 
									MAKELONG(DGBTN_QUIT, BN_CLICKED),
									(LPARAM)hWnd);
							}
						break;

						case TRAY_APPRESTORE:
							{
								Tray->RestoreWindow();
							}
						break;
					}
				}
			}
		break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
		break;

		default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, 
					  HINSTANCE hPrevInstance,
					  LPSTR lpCmdLine, 
					  int nCmdShow)
{
//	ProtectionClass *Prota = new ProtectionClass();
	WNDCLASSEXW wc;
    HWND hwnd;
    MSG Msg;
	__w64 unsigned long gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	MainMutex = CreateMutexW(NULL,FALSE, MUTEXRUN);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		MessageBoxW(0, L"eXsliterator is already running!", L"ERROR", MB_ICONERROR);
		ExitProcess(-1);
	}
	HideThread(NULL);
	wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(16);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"eXsliterator";
    wc.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));

	if(!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"eXsliterator has failed to start!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        ExitProcess(-1);
    }

	if(!InitializeNTAPI()){
		MessageBoxW(NULL, L"Key functions not found!", L"ERROR", MB_ICONERROR);
		ExitProcess(-1);
	}

	ResetStruct();
	InitCommonControls();
	eXslitOptions *TempOption = new eXslitOptions(L"\\Settings.ini");
	Opt.ColHeaders = TempOption->IsOptionTrue(L"ColHeaders");
	Opt.TopGridBackground = TempOption->GetExtendedOption(L"TopGridBkg", L"value");
	Opt.BottomGridBaclground = TempOption->GetExtendedOption(L"BottomGridBkg", L"value");
//	wchar_t *tempSe = TempOption->GetExtendedOption(L"AuthCode", L"value");
	delete TempOption;
	ActivateClasses();
	OleInitialize(NULL);

//	if(Prota->VirtCheck() || Prota->DebCheck()){
//		MessageBoxW(0, L"Cannot run under debugger!", L"ERROR", MB_ICONERROR);
//		while(1)NtTerminateProcess((HANDLE)-1, 0);
//	}
//	delete Prota;

	/* Initialize options */
	wchar_t *UserSelectedLang = eXoptions->GetExtendedOption(L"UserLanguage", L"value");
	if(wcscmp(UserSelectedLang, L"EN") == 0)Opt.UserLanguage = ENGLISH;
	else if(wcscmp(UserSelectedLang, L"PL") == 0)Opt.UserLanguage = POLISH;
	else if(wcscmp(UserSelectedLang, L"RU") == 0)Opt.UserLanguage = RUSSIAN;
	else Opt.UserLanguage = ENGLISH;
	Opt.DebugLogToFile = eXoptions->IsOptionTrue(L"DebugLog");
	Opt.GlobalTranslit = eXoptions->IsOptionTrue(L"GlobalTranslit");
	Opt.LocalTranslationSearch = eXoptions->IsOptionTrue(L"LocalTranslationSearch");
	Opt.OnlineTranslationSearch = eXoptions->IsOptionTrue(L"OnlineTranslationSearch");
	BOOL GlassEffect = eXoptions->IsOptionTrue(L"VistaGlass");
	if(GlassEffect) Opt.VistaGlass = 10;
	Opt.ButtonsIcons = eXoptions->IsOptionTrue(L"ButtonsIcons");
	Opt.GlowText = eXoptions->IsOptionTrue(L"BottomGlowText");
	Opt.Tooltips = eXoptions->IsOptionTrue(L"ShowTooltips");
	Opt.ColHeaders = eXoptions->IsOptionTrue(L"ColHeaders");
	Opt.AutoUpdates = eXoptions->IsOptionTrue(L"AutoCheckUpdates");
	wchar_t *someshit = eXoptions->GetExtendedOption(L"ConnectionTimeOut", L"Value");
	Opt.ConnectionTimeOut = wtoi(someshit);
	GlobalTransliterationsNumber = 0;
	MemDatabase = InitInMemoryDatabase();
	PhyDataBase = InitPhysicalDatabase();
	hHookThread = NULL;
	MonitorAllWindowsThreadHandle = NULL;
	OSVERSIONINFO VersionInfo;
	RtlSecureZeroMemory(&VersionInfo, sizeof(VersionInfo));
	VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
	GetVersionEx(&VersionInfo);
	Opt.wVersion = VersionInfo.dwMajorVersion;
//	System *Sys = new System();
//	Network *Net = new Network();
//	char *lolz = Sys->HardGemEx();
//	wchar_t *lolleng = Net->AnsiToUnicode(lolz);
//	delete Sys;
//	delete Net;

//	if(wcscmp(lolleng, tempSe) == 0){
//		Opt.Kewl = TRUE;
	//	MessageBoxA(0, "registered!", "Server", 0);
//	} else {
//		Opt.Kewl = FALSE;
	//	MessageBoxA(0, "not registered", "Server", 0);
//	}
	/* Option initialization end */

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	int nScreenX = GetSystemMetrics(SM_CXSCREEN), nScreenY = GetSystemMetrics(SM_CYSCREEN);
	int nWidth   = (int)(nScreenX * 0.65f), nHeight  = (int)(nScreenY * 0.65f);
	RECT rc = { (nScreenX - nWidth) / 2, (nScreenY - nHeight) / 2, nWidth, nHeight };
	hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
		L"eXsliterator",
		L"eXsliterator",
        WS_OVERLAPPEDWINDOW,
        rc.left, rc.top, rc.right, rc.bottom,
        NULL, NULL, hInstance, NULL);
//	finalcunt->lolcheng();
//	SetTimer(hwnd, TIMER_DEF, 10000, NULL);

    if(hwnd == NULL){
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
	} else {
		MainWindowHandle = hwnd;
	}


	ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
	GdiplusShutdown(gdiplusToken);
    return Msg.wParam;
}