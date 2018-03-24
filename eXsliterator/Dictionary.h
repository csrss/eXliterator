#include "Dict.h"

__w64 long __stdcall CustomProc(__in HWND hWnd, 
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
				wchar_t *word = HelperEC->GetText(GetDlgItem(GetParent(hWnd), IDC_EDIT1));
				wchar_t *dict = GetDictTypeLong(Opt.DictType);
				if(dict == NULL || wcslen(dict) == 0 || wcscmp(dict, L"") == 0){
					HelperEC->Clear(GetDlgItem(GetParent(hWnd), IDC_EDIT1));
					MessageBoxW(GetParent(hWnd), NoDict[Opt.UserLanguage], L"INFORMATION", MB_ICONINFORMATION);
					return DefSubclassProc(hWnd, uMsg, wParam, lParam);
				}
				wchar_t *Translation = SelectTranslation(PhyDataBase, word, dict);
				if(Translation != NULL){
					SetDlgItemTextW(GetParent(hWnd), IDC_EDIT2, Translation);
					HelperDG->ScrollToItem(-1, 0, word, FALSE, GetDlgItem(GetParent(hWnd), IDC_LIST1));
				} else {
					SetDlgItemTextW(GetParent(hWnd), IDC_EDIT2, L"");
				}

				if((wParam == VK_RETURN)){
					PostMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), 0);
				}
			}
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

int __stdcall SetButtonIcon(__in HWND hWnd, 
							__in int ButtonID, 
							__in int IconID)
{
	HWND hUpCtl;
	HICON hU;
	hUpCtl = GetDlgItem( hWnd, ButtonID );
	hU = (HICON)LoadImage( GetModuleHandleW(0),
			MAKEINTRESOURCE( IconID ),
			IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
	SendMessage( hUpCtl, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM) (DWORD) hU );
	return 0;
}

DataGridControl *WordsListDG[1];
ResImage *ResourceImage;
DGButton *DlgDGButton[6];
ContextMenu *CTDiaMenu[1];

__w64 long __stdcall DictionaryProc(__in HWND hWnd, 
									__in unsigned int uMsg, 
									__in __w64 unsigned int wParam, 
									__in __w64 long lParam)
{
	bool Mode;
	RECT m_rcClient;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	switch(uMsg){
		case WM_CREATE:
			{
				InitHelpers();
				WordsListDG[0] = new DataGridControl(FALSE);
				WordsListDG[0]->Create(hWnd, 0, 0, 0, 0, DLG_DATAGRID, FALSE, FALSE, 10);
				WordsListDG[0]->AddColumn(200, dict_words_header[Opt.UserLanguage], 0);
				WordsListDG[0]->SetFullRowSelection();

				DlgDGButton[0] = new DGButton(FALSE);
				DlgDGButton[1] = new DGButton(FALSE);
				DlgDGButton[2] = new DGButton(FALSE);
				DlgDGButton[3] = new DGButton(FALSE);
				DlgDGButton[4] = new DGButton(FALSE);
				DlgDGButton[5] = new DGButton(FALSE);

				DlgDGButton[0]->PaintBtnControl(10, hWnd, L"Search", 0, 0, 0, 0, IDC_BUTTON3);
				DlgDGButton[0]->SetBtnIcon(IDI_ICON17, DlgDGButton[0]->ICON_POSITION_LEFT);
				DlgDGButton[0]->SetGlowColor(0xff8DBDFF);

				DlgDGButton[1]->PaintBtnControl(10, hWnd, L"Insert", 0, 0, 0, 0, IDOK);
				DlgDGButton[1]->SetBtnIcon(IDI_ICON16, DlgDGButton[1]->ICON_POSITION_LEFT);
				DlgDGButton[1]->SetGlowColor(0xff7CFC00);

				DlgDGButton[2]->PaintBtnControl(10, hWnd, L"Save", 0, 0, 0, 0, IDC_BUTTON1);
				DlgDGButton[2]->SetBtnIcon(IDI_ICON15, DlgDGButton[2]->ICON_POSITION_LEFT);
				DlgDGButton[2]->SetGlowColor(0xffFF9912);

				DlgDGButton[3]->PaintBtnControl(10, hWnd, L"Delete", 0, 0, 0, 0, IDC_BUTTON2);
				DlgDGButton[3]->SetBtnIcon(IDI_ICON14, DlgDGButton[3]->ICON_POSITION_LEFT);
				DlgDGButton[3]->SetGlowColor(0xffFF0000);

				DlgDGButton[4]->PaintBtnControl(10, hWnd, L"Close", 0, 0, 0, 0, IDCANCEL);
				DlgDGButton[4]->SetBtnIcon(IDI_ICON13, DlgDGButton[4]->ICON_POSITION_LEFT);
				DlgDGButton[4]->SetGlowColor(0xffFF0000);

				DlgDGButton[5]->PaintBtnControl(10, hWnd, translation_type[Opt.UserLanguage],
					0, 0, 0, 0, IDC_COMBO1);
				DlgDGButton[5]->SetBtnIcon(IDI_ICON18, DlgDGButton[5]->ICON_POSITION_RIGHT);
				DlgDGButton[5]->SetGlowColor(0xff7CFC00);

				CTDiaMenu[0] = new ContextMenu();

				Entry[0] = new EntryControl(FALSE, FALSE);
				Entry[0]->Create(hWnd, 0, 0, 0, 0, IDC_EDIT1, FALSE, FALSE, 10);

				Entry[1] = new EntryControl(TRUE, TRUE);
				Entry[1]->Create(hWnd, 0, 0, 0, 0, IDC_EDIT2, FALSE, FALSE, 10);

				SetDictionaryText(hWnd, Opt.UserLanguage);
				SetDictTooltips(hWnd, Opt.UserLanguage);
				SetWindowSubclass(Entry[0]->operator HWND(), CustomProc, 0, 0);
				HINSTANCE hIn = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				SetClassLong(hWnd, GCL_HICON, (LONG)LoadIcon(hIn, (LPCTSTR)IDI_ICON1));
				/******************************************************************/
				GuiEffectsEx->ApplyExplorerStyle(WordsListDG[0]->operator HWND());
				BuildWordsList(GetDlgItem(hWnd, IDC_COMBO1), WordsListDG[0]->operator HWND());
				Deb->dprint(false, L"loword: %d, highword: 0x%08x", (int)LOWORD(lParam), (long)HIWORD(lParam));
				WordsListDG[0]->SetBkgImage(L"C:\\Users\\Administrator\\Documents\\Visual Studio 2008\\Projects\\Translit\\Translit\\Img\\kbd2.jpg");
				ResourceImage = new ResImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDR_KEYBOARD), RT_RCDATA);
				PostMessageW(hWnd, WM_SIZE, 0, 0);
			}
		break;

		case WM_INITDIALOG:
			{
				if((int)LOWORD(lParam) != 0 || (long)HIWORD(lParam) != 0){
					int Index = (int)LOWORD(lParam);
					long Dict = (long)HIWORD(lParam);
					wchar_t *WordEx = DataGrid[1]->GetItemTextByIndex(0, Index);
					wchar_t *DictEx = GetDictTypeLong(Dict);
					wchar_t *LangDict = GetDictLangType(Dict, Opt.UserLanguage);
					wchar_t *Translation = SelectTranslation(PhyDataBase, WordEx, DictEx);
					DlgDGButton[5]->SetBtnText(LangDict);
					SetDlgItemTextW(hWnd, IDC_EDIT2, Translation);
				}
			}
		break;

		case WM_PRINTCLIENT:
			{
				OnInternalPaint(hWnd, (HDC)wParam, m_rcDialog, TRUE);
			}
		break;

		case WM_PAINT:
			{
				OnInternalPaint(hWnd, NULL, m_rcDialog, TRUE);
			}
		break;

		case WM_CTLCOLOREDIT:
			{
				if(Opt.wVersion >= 6){
					if ((HWND)lParam == Entry[0]->operator HWND()){
						SetDCBrushColor((HDC)wParam, RGB(0, 0, 0)); // RGB(205, 201, 201)
						return (BOOL)GetStockObject(DC_BRUSH);
					} else if((HWND)lParam == Entry[1]->operator HWND()){
						SetDCBrushColor((HDC)wParam, RGB(255, 255, 255));
						return (BOOL)GetStockObject(DC_BRUSH);
					}
				}
			}
		break;

		case WM_SIZE:
			{
				RECT lpRect;
				GetClientRect(hWnd, &lpRect);
				GetClientRect(hWnd, &m_rcDialog);
				int ClientHeight = lpRect.bottom;
				int ClientWidth = lpRect.right;
				int ButtonWidth = DlgDGButton[0]->Width();
				int ButtonHeight = DlgDGButton[0]->Height();
				HWND Edit2 = GetDlgItem(hWnd, IDC_EDIT2);
				HWND List1 = GetDlgItem(hWnd, IDC_LIST1);

				WordsListDG[0]->SetSize(150, ClientHeight);

				// search button
				DlgDGButton[0]->SetPosition(ClientWidth-(DlgDGButton[0]->Width()+10), 
									  10);
				DlgDGButton[0]->SetSize(120, 80);

				// close button
				DlgDGButton[4]->SetPosition(ClientWidth-(ButtonWidth + 10),
					ClientHeight - (50) - 10);
				DlgDGButton[4]->SetSize(120, 50);

				// delete button
				DlgDGButton[3]->SetPosition(ClientWidth-((ButtonWidth*2) + (10*2)),
					ClientHeight - (50) - 10);
				DlgDGButton[3]->SetSize(120, 50);

				// save button
				DlgDGButton[2]->SetPosition(ClientWidth-((ButtonWidth*3) + (10*3)),
					ClientHeight - (50) - 10);
				DlgDGButton[2]->SetSize(120, 50);

				// insert button
				DlgDGButton[1]->SetPosition(ClientWidth-((ButtonWidth*4) + (10*4)),
					ClientHeight - (50) - 10);
				DlgDGButton[1]->SetSize(120, 50);

				// translation result
				Entry[1]->SetPosition((ClientWidth / 4)+5,
					100);
				Entry[1]->SetSize((ClientWidth -(ClientWidth/3))+30, 
								  (ClientHeight - 170));

				DlgDGButton[5]->SetPosition((ClientWidth / 4)+3, 40);
				Entry[0]->SetPosition((ClientWidth / 4)+5, 10);

				DlgDGButton[5]->SetSize((ClientWidth / 2), 50);
				Entry[0]->SetSize((ClientWidth / 2)-5, 20);
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

		case WM_GETMINMAXINFO:
			{
				MINMAXINFO *pMinMax = (MINMAXINFO*)lParam;
				pMinMax->ptMinTrackSize.x = 715;
				pMinMax->ptMinTrackSize.y = 300;
			}
		break;

		case WM_NOTIFY:
			{
				switch(LOWORD(wParam)){
					case DLG_DATAGRID:
						{
							if(((LPNMHDR)lParam)->code == NM_DBLCLK){
								NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)lParam;
								if (nmia->iItem != -1){
	//								long lType;
									wchar_t *Type = NULL, *ItemText = NULL, *Translation = NULL;
									Type = GetDictTypeLong(Opt.DictType);
									ItemText = WordsListDG[0]->GetSelectedItemText(0);
									Translation = SelectTranslation(PhyDataBase, ItemText, Type);
									SetDlgItemTextW(hWnd, IDC_EDIT2, Translation);
								}
							}
						}
					break;
				}
			}
		break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) {
					case IDCANCEL:
						{
							DissmissHelpers();
							DeleteDictTooltips();
	//						KillDialog(L"Dictionary");
							DestroyWindow(hWnd);
							NtClose(DictMutex);
							DictMutex = NULL;
							DictWindowHandle = NULL;
						}
					break;
					
					case IDC_BUTTON3:	// search button
						{
						//	long lType;
							wchar_t *word = HelperEC->GetText(GetDlgItem((hWnd), IDC_EDIT1));
							wchar_t *dict = GetDictTypeLong(Opt.DictType);
							wchar_t *Translation = SelectTranslation(PhyDataBase, word, dict);
							if(Translation != NULL){
								SetDlgItemTextW(GetParent(hWnd), IDC_EDIT2, Translation);
								WordsListDG[0]->ScrollToItem(-1, 0, word, FALSE);
							} else {
								if(Opt.OnlineTranslationSearch == TRUE){
									MakeTranslationRequest(Opt.DictType, word);
								} else {
									MessageBoxW(hWnd, L"Online search is disabled in application options!", L"INFORMATION", MB_ICONINFORMATION);
								}
							//	Deb->dprint(false, L"lType: 0x%08x\r\n", lType);
							}
						}
					break;

					case IDC_BUTTON2:	// delete button
						{
						//	long lType;
							int iNdex = WordsListDG[0]->GetSelectedIndex();
							wchar_t *word = WordsListDG[0]->GetSelectedItemText(0);
							wchar_t *dict = GetDictTypeLong(Opt.DictType);
							DeleteTranslation(PhyDataBase, word, dict);
							WordsListDG[0]->DeleteItem(iNdex);
							HelperEC->Clear(GetDlgItem(hWnd, IDC_EDIT2));
						}
					break;

					case IDC_BUTTON1:	// save / update button
						{
						//	long lType;
							wchar_t *word = WordsListDG[0]->GetSelectedItemText(0);
							wchar_t *dict = GetDictTypeLong(Opt.DictType);
							wchar_t *translation = HelperEC->GetText(GetDlgItem(hWnd, IDC_EDIT2));
							UpdateTranslation(PhyDataBase, word, translation, dict);
						}
					break;

					case IDOK:	// insert new translation button
						{
					//		long lType;
							wchar_t *word = HelperEC->GetText(GetDlgItem(hWnd, IDC_EDIT1));
							wchar_t *translation = HelperEC->GetText(GetDlgItem(hWnd, IDC_EDIT2));
							wchar_t *dict = GetDictTypeLong(Opt.DictType);
						//	MessageBoxW(0,word,translation,0);
							DBInsertTranslation(PhyDataBase, word, translation, dict);
						}
					break;

					case IDC_COMBO1:
						{
							int status = CTDiaMenu[0]->Create();
							CTDiaMenu[0]->AddItem(DICTCTX_EN_PL, dictype_en_pl[Opt.UserLanguage]);
							CTDiaMenu[0]->AddItem(DICTCTX_EN_RU, dictype_en_ru[Opt.UserLanguage]);
							CTDiaMenu[0]->AddItem(DICTCTX_PL_EN, dictype_pl_en[Opt.UserLanguage]);
							CTDiaMenu[0]->AddItem(DICTCTX_PL_RU, dictype_pl_ru[Opt.UserLanguage]);
							CTDiaMenu[0]->AddItem(DICTCTX_RU_EN, dictype_ru_en[Opt.UserLanguage]);
							CTDiaMenu[0]->AddItem(DICTCTX_RU_PL, dictype_ru_pl[Opt.UserLanguage]);
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_EN_PL, IDB_BITMAP1, (hWnd));
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_EN_RU, IDB_BITMAP1, (hWnd));
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_PL_EN, IDB_BITMAP1, (hWnd));
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_PL_RU, IDB_BITMAP1, (hWnd));
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_RU_EN, IDB_BITMAP1, (hWnd));
							CTDiaMenu[0]->SetItemBitmap(DICTCTX_RU_PL, IDB_BITMAP1, (hWnd));
							unsigned int Clicked = CTDiaMenu[0]->TrackMenuHit2(hWnd, DlgDGButton[5]->operator HWND());
							switch(Clicked){
								case DICTCTX_EN_PL:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_en_pl[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_en_pl[Opt.UserLanguage]+1);
										Opt.DictType = EN_PL;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;

								case DICTCTX_EN_RU:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_en_ru[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_en_ru[Opt.UserLanguage]+1);
										Opt.DictType = EN_RU;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;

								case DICTCTX_PL_EN:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_pl_en[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_pl_en[Opt.UserLanguage]+1);
										Opt.DictType = PL_EN;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;

								case DICTCTX_PL_RU:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_pl_ru[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_pl_ru[Opt.UserLanguage]+1);
										Opt.DictType = PL_RU;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;

								case DICTCTX_RU_EN:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_ru_en[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_ru_en[Opt.UserLanguage]+1);
										Opt.DictType = RU_EN;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;

								case DICTCTX_RU_PL:
									{
										Entry[1]->Clear();
										DlgDGButton[5]->SetBtnText(dictype_ru_pl[Opt.UserLanguage]+1);
										DarkGlasButton[4]->SetBtnText(dictype_ru_pl[Opt.UserLanguage]+1);
										Opt.DictType = RU_PL;
										BuildWordsList(DlgDGButton[5]->operator HWND(), WordsListDG[0]->operator HWND());
									}
								break;
							}
							CTDiaMenu[0]->Destroy();
						}
					break;
				}
			}
		break;

		case WM_CLOSE:
			{
				DissmissHelpers();
				DeleteDictTooltips();
				KillDialog(L"Dictionary");
				NtClose(DictMutex);
				DictMutex = NULL;
				DictWindowHandle = NULL;
			//	DestroyWindow(hWnd);
			}
		break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
		break;

		default:
			return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}