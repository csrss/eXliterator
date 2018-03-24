#ifndef _SQLMEM_H
#define _SQLMEM_H

sqlite3 *InitInMemoryDatabase(){
	sqlite3 *DataBase;
	char *zErr; 
	int rc;
	bool Mode;
	char debug[MAX_PATH] = {0};
	char *sql_translite = "CREATE TABLE Transliterated			\
                          (										\
                          Id INTEGER PRIMARY KEY,				\
                          WINDOW_NAME longtext NOT NULL,		\
						  WINDOW_HANDLE	longtext NOT NULL,		\
						  KBD_PREV longtext NOT NULL,			\
                          KBD_NEW longtext NOT NULL,			\
						  TRANSLIT_FLAG longtext NOT NULL		\
                          );";

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

	rc = sqlite3_open16(L":memory:", &DataBase); 
        if(rc) {
			Deb->dprint(Mode, L"Cannot create In-Memory Database!\r\n");
            return NULL;
		} else {
			Deb->dprint(Mode, L"In-Memory Database created!\r\n");
		}
	rc = sqlite3_exec(DataBase, sql_translite, NULL, NULL, &zErr); 
        if(rc != SQLITE_OK) { 
			if (zErr != NULL) { 
				sprintf(debug, "SQL error: %s\n", zErr);
				OutputDebugStringA(debug);
				sqlite3_close(DataBase); 
				sqlite3_free(zErr); 
				return NULL;
			} 
		}  else {
			Deb->dprint(Mode, L"Table 'Transliterated' successfuly created in application memory!\r\n");
		}
 
	return DataBase;
}

int CloseDatabaseConnection(__in sqlite3 *db){
	int rc, Status;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

	rc = sqlite3_close(db);
	if(rc != SQLITE_OK){
		Deb->dprint(Mode, L"Closing connection failed!\r\n");
		Status = -1;
	} else {
		Deb->dprint(Mode, L"Connection with database successfuly closed!\r\n");
		Status = 0;
	}
	return Status;
}

int sqlite3_exec16(__in sqlite3 *db,
							 __in wchar_t *SqlQuery)
{
	char debug[1024] = {0};
	sqlite3_stmt *stmt;
	int rc;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(db == NULL || SqlQuery == NULL || wcslen(SqlQuery) <= 0){
		Deb->dprint(Mode, L"sqlite3_exec16: Wrong (empty) Values!\r\n");
		return -1;
	}
///	if(FuncName == NULL)FuncName = "";
	rc = sqlite3_prepare16(db, SqlQuery, -1, &stmt, 0);
    if(rc != SQLITE_OK) { 
		sprintf(debug, "sqlite3_exec16: sqlite3_prepare16 failed! Status: %s\n", sqlite3_errmsg(db));
		OutputDebugStringA(debug);
		return -1;
	}  
	rc = sqlite3_step(stmt);
	if( rc != SQLITE_DONE ){
		sprintf(debug, "sqlite3_exec16: sqlite3_step failed! Status: %s\n", sqlite3_errmsg(db));
		OutputDebugStringA(debug);
		return -1;
	} 
	int final = sqlite3_finalize(stmt);
	if(final != SQLITE_OK){
		sprintf(debug, "sqlite3_exec16: sqlite3_step failed! Status: %s\n", sqlite3_errmsg(db));
		OutputDebugStringA(debug);
		return -1;
	}
	return 0;
}

wchar_t *SearchWindowNameByHandle(__in HWND hWnd,
								  __in sqlite3 *db){
	bool Mode;
	static wchar_t *output;
	sqlite3_stmt *stmt;
	char debug[MAX_PATH];
	wchar_t WindowHandle[11] = {0};
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(hWnd == NULL){
		Deb->dprint(Mode, L"@SearchWindowNameByHandle: Wrong window handle!\r\n");
		return NULL;
	}

	swprintf(WindowHandle, 11, L"0x%08x", hWnd);
	int SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowHandle)) + MAX_PATH;
	wchar_t *SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, L"SELECT * FROM Transliterated WHERE WINDOW_HANDLE = '%s'", 
								WindowHandle);
	int rc = sqlite3_prepare16(db, SqlQuery, -1, &stmt, 0);
        if(rc != SQLITE_OK) { 
			sprintf(debug, "sqlite3_prepare16 failed! Status: %s", sqlite3_errmsg(db));
			OutputDebugStringA(debug); 
			return NULL;
		} 
	rc = sqlite3_step(stmt);
	switch(rc){
		case SQLITE_DONE:
			{
				output = NULL;
			}
		break;

		case SQLITE_ROW:
			{
				int output_len = sizeof(wchar_t*) * wcslen((LPWSTR)sqlite3_column_text16(stmt, 1)) + 1;
				output = (wchar_t*)malloc(output_len);
				swprintf(output, output_len, L"%s", (LPWSTR)sqlite3_column_text16(stmt, 1));
			}
		break;

		default:
			{
				output = NULL;
			}
		break;
	}
	sqlite3_finalize(stmt);
	free(SqlQuery);
	return output;
}

int InsertTransliterated(__in sqlite3 *db,
								   __in wchar_t *WindowName,
								   __in HWND WindowH,
								   __in HKL KbdPrev,
								   __in HKL KbdNew,
								   __in bool TranslitFlag)
{
	wchar_t *SqlQuery = NULL;
	wchar_t *Test;
	wchar_t FlagTrue[] = L"TRUE";
	wchar_t FlagFalse[] = L"FALSE";
	int SqlQuery_len;
	int rc;
	int iNdex;
	NTSTATUS Status;
	bool Mode;
	wchar_t KbdPrevLaout[10] = {0};
	wchar_t KbdNewLayout[10] = {0};
	wchar_t WindowHandle[11] = {0};
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(db == NULL || WindowName == NULL || wcslen(WindowName) <= 0){
		Deb->dprint(Mode, L"@InsertTransliterated: Wrong values!\r\n");
		return -1;
	}
	Test = SearchWindowNameByHandle(WindowH, db);
	if(Test != NULL){
		Deb->dprint(Mode, L"@InsertTransliterated: Window is in database already!\r\n");
		return 1;
	}
	swprintf(KbdPrevLaout, 10, L"0000%04x", (unsigned short)KbdPrev);
	swprintf(KbdNewLayout, 10, L"0000%04x", (unsigned short)KbdNew);
	swprintf(WindowHandle, 11, L"0x%08x", WindowH);
	SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowName)+wcslen(KbdPrevLaout)+wcslen(KbdNewLayout))+1024;
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, L"INSERT INTO Transliterated (Id, WINDOW_NAME, WINDOW_HANDLE, KBD_PREV, KBD_NEW, TRANSLIT_FLAG) VALUES (NULL, '%s', '%s', '%s', '%s', '%s')", 
		WindowName, WindowHandle, KbdPrevLaout, KbdNewLayout, (TranslitFlag ? FlagTrue : FlagFalse));
	rc = sqlite3_exec16(db, SqlQuery);
	free(SqlQuery);
	iNdex = InsertWindow(MainWindowHandle, IDC_LIST1, (TranslitFlag ? true : false), WindowName);

	Status = RtlCreateUserThread(((HANDLE)-1), NULL, FALSE, 0, 0, 0, 
									(LPTHREAD_START_ROUTINE)ProcMonitorThread, 
									(LPVOID)WindowH, NULL, 0);
	if(Status != STATUS_SUCCESS){
		Deb->dprint(Mode, L"@InsertTransliterated: Creating monitor thread error!\r\n");
	} else {
		Deb->dprint(Mode, L"@InsertTransliterated: Monitor thread created.\r\n");
	}
	return 0;
}

int DeleteTransliterated(__in sqlite3 *db,
								   __in wchar_t *WindowName)
{
	wchar_t *SqlQuery = NULL;
	bool Mode;
	int SqlQuery_len, rc;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(WindowName == NULL || wcslen(WindowName) <= 0){
		Deb->dprint(Mode, L"WindowName is nul.\r\n");
		return -1;
	}
	SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowName)) + 1024;
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, L"DELETE FROM Transliterated WHERE WINDOW_NAME = '%s'", WindowName);
	rc = sqlite3_exec16(db, SqlQuery);
	free(SqlQuery);
	return 0;
}

bool IsTranslitAllowed(__in sqlite3 *db,
								 __in HWND hWnd){
	bool Mode, Retval = false;
	static wchar_t *output;
	sqlite3_stmt *stmt;
	char debug[MAX_PATH];
	wchar_t WindowHandle[11] = {0};

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(hWnd == NULL){
		Deb->dprint(Mode, L"@IsTranslitAllowed: Wrong window handle!\r\n");
		return NULL;
	}

	swprintf(WindowHandle, 11, L"0x%08x", hWnd);
	int SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowHandle)) + 1024;
	wchar_t *SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, L"SELECT * FROM Transliterated WHERE WINDOW_HANDLE = '%s'", 
								WindowHandle);
	int rc = sqlite3_prepare16(db, SqlQuery, -1, &stmt, 0);
        if(rc != SQLITE_OK) { 
			sprintf(debug, "sqlite3_prepare16 failed! Status: %s", sqlite3_errmsg(db));
			OutputDebugStringA(debug); 
			return NULL;
		} 
	rc = sqlite3_step(stmt);
	switch(rc){
		case SQLITE_DONE:
			{
				Retval = false;
			}
		break;

		case SQLITE_ROW:
			{
				int output_len = sizeof(wchar_t*) * wcslen((LPWSTR)sqlite3_column_text16(stmt, 5)) + 1;
				output = (wchar_t*)malloc(output_len);
				swprintf(output, output_len, L"%s", (LPWSTR)sqlite3_column_text16(stmt, 5));
				if(wcscmp(output, L"TRUE") == 0) Retval = true;
				else Retval = false;
			}
		break;

		default:
			{
				Retval = false;
			}
		break;
	}
	sqlite3_finalize(stmt);
	free(SqlQuery);
	free(output);
	return Retval;
}

int SelectKbdLayouts(__in sqlite3 *db,
							   __in wchar_t *WindowName,
							   __out wchar_t *&OldKbd,
							   __out wchar_t *&NewKbd,
							   __out wchar_t *&WindowH)
{
	wchar_t *SqlQuery = NULL;
	int SqlQuery_len;
	char debug[MAX_PATH] = {0};
	sqlite3_stmt *stmt;
	int rc, Status; 
	bool Mode;

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

	if(db == NULL || WindowName == NULL || wcslen(WindowName) <= 0){
		Deb->dprint(Mode, L"@SelectKbdLayouts: Wrong Values!\r\n");
		*OldKbd = NULL;
		*NewKbd = NULL;
		return -1;
	}

	SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowName)) + 1024;
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, L"SELECT * FROM Transliterated WHERE WINDOW_NAME = '%s'", WindowName);
	rc = sqlite3_prepare16(db, SqlQuery, -1, &stmt, 0);
        if(rc != SQLITE_OK) { 
			sprintf(debug, "sqlite3_prepare16 failed! Status: %s", sqlite3_errmsg(db));
			OutputDebugStringA(debug); 
			return NULL;
		} 
	rc = sqlite3_step(stmt);
	switch(rc){
		case SQLITE_DONE:
			{
	//			MessageBoxA(0,"SQLITE_DONE","SQLITE_DONE",0);
				Status = 1;	// not found simply, if found this part is never called
			}
		break;

		case SQLITE_ROW:
			{
	//			MessageBoxA(0,"SQLITE_ROW","SQLITE_ROW",0);
				wchar_t* pTemp = (wchar_t *)malloc(sizeof(wchar_t*) * MAX_PATH + 1);
				wchar_t* cTemp = (wchar_t *)malloc(sizeof(wchar_t*) * MAX_PATH + 1);
				wchar_t* wTemp = (wchar_t *)malloc(sizeof(wchar_t*) * MAX_PATH + 1);
				wcscpy(wTemp, (wchar_t*)sqlite3_column_text16(stmt, 2));
				wcscpy(pTemp, (wchar_t*)sqlite3_column_text16(stmt, 3));
				wcscpy(cTemp, (wchar_t*)sqlite3_column_text16(stmt, 4));
				OldKbd = pTemp;
				NewKbd = cTemp;
				WindowH = wTemp;
				Status = 0;
			}
		break;

		default:
			{
		//		MessageBoxA(0,"default","default",0);
				Status = -1;	// sql query failed!
			}
		break;
	}
	sqlite3_finalize(stmt);
	free(SqlQuery);
	return Status;
}

int AllowTransliteration(__in sqlite3 *db,
								   __in wchar_t *WindowName,
								   __in HKL KbdPrev,
								   __in bool Allowed)
{
	wchar_t KbdPrevLaout[10] = {0};
	if(KbdPrev != NULL){
		swprintf(KbdPrevLaout, 10, L"0000%04x", (unsigned short)KbdPrev);
	}
	int SqlQuery_len = sizeof(wchar_t*) * (wcslen(WindowName)) + 1024;
	wchar_t *SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	if(KbdPrev != NULL){
		swprintf(SqlQuery, SqlQuery_len, 
		L"UPDATE Transliterated SET TRANSLIT_FLAG = '%s', KBD_PREV = '%s' WHERE WINDOW_NAME = '%s'", 
						(Allowed ? L"TRUE" : L"FALSE"), KbdPrevLaout, WindowName);
	} else {
		swprintf(SqlQuery, SqlQuery_len, 
		L"UPDATE Transliterated SET TRANSLIT_FLAG = '%s' WHERE WINDOW_NAME = '%s'", 
						(Allowed ? L"TRUE" : L"FALSE"), WindowName);
	}

	int rc = sqlite3_exec16(db, SqlQuery);
	free(SqlQuery);
	return rc;
}


int RemoveAllTransliterations(HWND hWnd){
	unsigned long WindowHandleLong;
	wchar_t *WindowH = NULL;
	wchar_t *OldKbd = NULL; 
	wchar_t *NewKbd = NULL; 
	HKL NewKey;
	int iTems = DataGrid[0]->GetItemsNr();
	while(iTems > 0){
		iTems--;
		wchar_t *ItemText = DataGrid[0]->GetItemTextByIndex(1, iTems);
		SelectKbdLayouts(MemDatabase, ItemText, OldKbd, NewKbd, WindowH);
		NewKey = LoadKeyboardLayoutW(OldKbd, 0); // // KLF_ACTIVATE | KLF_SETFORPROCESS
		WindowHandleLong = wcstoul(WindowH,(wchar_t **)NULL, 0);
		if(WindowHandleLong != 0) SendMessage((HWND)WindowHandleLong, WM_INPUTLANGCHANGEREQUEST,0,(WPARAM)NewKey);
//		delete OldKbd;
//		delete NewKbd;
//		delete WindowH;
	} 
	GlobalTransliterationsNumber = 0;
	CloseDatabaseConnection(MemDatabase);
	MemDatabase = InitInMemoryDatabase();
	DataGrid[0]->ClearDataGrid();
	return 0;
}

#endif 