DataGridControl *HelperDG;
EntryControl *HelperEC;
EntryControl *EntryHelper;

int InitHelpers(){
	if(!HelperDG)HelperDG = new DataGridControl(TRUE);
	if(!HelperEC)HelperEC = new EntryControl(FALSE, FALSE);
	return 0;
}

int DissmissHelpers(){
	if(HelperDG)delete HelperDG;
	if(HelperEC)delete HelperEC;
	return 0;
}

wchar_t * GetDictTypeLong(__in long lType){
	switch(lType){
		case EN_PL: return cEN_PL; break;
		case EN_RU: return cEN_RU; break;
		case PL_EN: return cPL_EN; break;
		case PL_RU: return cPL_RU; break;
		case RU_EN: return cRU_EN; break;
		case RU_PL: return cRU_PL; break;
		default: return NULL; break;
	}
	return NULL;
}

wchar_t * GetDictLangType(__in long lType, 
									__in int UserLanguage)
{
	switch(lType){
		case EN_PL: return dictype_en_pl[UserLanguage]; break;
		case EN_RU: return dictype_en_ru[UserLanguage]; break;
		case PL_EN: return dictype_pl_en[UserLanguage]; break;
		case PL_RU: return dictype_pl_ru[UserLanguage]; break;
		case RU_EN: return dictype_ru_en[UserLanguage]; break;
		case RU_PL: return dictype_ru_pl[UserLanguage]; break;
		default: return NULL; break;
	}
	return NULL;
}

/*
wchar_t * __stdcall GetDictType(__in HWND hCombo, __out long * Type){
	wchar_t * lpszString;
	static wchar_t *Dict = NULL;
	int TextLength;
	int nCurSel = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
	if (nCurSel != -1){
		TextLength = SendMessageW(hCombo, CB_GETLBTEXTLEN, nCurSel, 0);
		lpszString = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
			sizeof(wchar_t*)*TextLength+1);
		if (lpszString != NULL){
			SendMessageW(hCombo, CB_GETLBTEXT, nCurSel, (LPARAM)lpszString);
			if(!lpszString) return NULL;
			if(wcscmp(lpszString, dictype_en_pl[Opt.UserLanguage]) == 0){
				Dict = cEN_PL;
				*Type = EN_PL;
			} else if(wcscmp(lpszString, dictype_en_ru[Opt.UserLanguage]) == 0){
				Dict = cEN_RU;
				*Type = EN_RU;
			} else if(wcscmp(lpszString, dictype_pl_en[Opt.UserLanguage]) == 0){
				Dict = cPL_EN;
				*Type = PL_EN;
			} else if(wcscmp(lpszString, dictype_pl_ru[Opt.UserLanguage]) == 0){
				Dict = cPL_RU;
				*Type = PL_RU;
			} else if(wcscmp(lpszString, dictype_ru_en[Opt.UserLanguage]) == 0){
				Dict = cRU_EN;
				*Type = RU_EN;
			} else if(wcscmp(lpszString, dictype_ru_pl[Opt.UserLanguage]) == 0){
				Dict = cRU_PL;
				*Type = RU_PL;
			}
		}
		HeapFree(GetProcessHeap(), 0, lpszString);
	}
	return Dict;
}
*/ 
int BuildWordsList(HWND hDictCombo, HWND hList){
	bool Mode;
//long lType;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	HelperDG->ClearDataGrid(hList);
	Network *Net = new Network();
	wchar_t *Dict = GetDictTypeLong(Opt.DictType);
	wchar_t SqlQuery[1024] = {0};// = L"SELECT * FROM Translations WHERE TRANSLATION_TYPE = ";
	swprintf(SqlQuery, MAX_PATH, L"SELECT * FROM Translations WHERE TRANSLATION_TYPE = '%s'", Dict);
	//	wcscat(SqlQuery, Dict);
	sqlite3_stmt *stmt;
	int rc;
	rc = sqlite3_prepare16(PhyDataBase, SqlQuery, -1, &stmt, 0);
	if(rc != SQLITE_OK) { 
		Deb->dprint(Mode, L"sqlite3_prepare16 failed! Status: %s", 
			Net->AnsiToUnicode((char *)sqlite3_errmsg(PhyDataBase)));
		return -1;
	}
	do {
		rc = sqlite3_step(stmt);
		switch(rc){
			case SQLITE_DONE:
			break;
			case SQLITE_ROW:
				{
					HelperDG->InsertRootItem((LPWSTR)sqlite3_column_text16(stmt, 1), -1, hList);
				}
			break;
			default:
				{
					Deb->dprint(Mode, L"@SelectTranslation: sqlite3_step error!\r\n");
				}
			break;
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize(stmt);
	delete Net;
	return 0;
}