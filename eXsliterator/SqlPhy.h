#ifndef _SQLPHY_H
#define _SQLPHY_H

sqlite3 *InitPhysicalDatabase(){
	sqlite3 *DataBase;
	char *zErr; 
	int rc;
	bool Mode, IfExists;
	char debug[MAX_PATH] = {0};

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	char *sql_translate = "CREATE TABLE Translations			\
                          (										\
                          Id INTEGER PRIMARY KEY,				\
                          WORD_NAME longtext NOT NULL,			\
						  WORD_TRANSLATION longtext NOT NULL,	\
						  TRANSLATION_TYPE longtext NOT NULL	\
                          );";

	if(VerifyFilePresence(DICT_DB) != 1){	// file does not exists
		Deb->dprint(false, L"Database doesnot exist!");
		IfExists = false;
	} else {	// file is there
		IfExists = true;
	}

	rc = sqlite3_open16(dict_db(), &DataBase); 
        if(rc) {
			Deb->dprint(Mode, L"Cannot open connection to Physical Database!\r\n");
            return NULL;
		} else {
			Deb->dprint(Mode, L"Connected with Physical Database!\r\n");
		}

	if(!IfExists){
	rc = sqlite3_exec(DataBase, sql_translate, NULL, NULL, &zErr); 
        if(rc != SQLITE_OK) { 
			if (zErr != NULL) { 
				sprintf(debug, "SQL error: %s\n", zErr);
				OutputDebugStringA(debug);
				sqlite3_close(DataBase); 
				sqlite3_free(zErr); 
				return NULL;
			} 
		}  else {
			Deb->dprint(Mode, L"Table 'Translations' successfuly created!\r\n");
		}
	}

	return DataBase;
}

int DBInsertTranslation(	__in sqlite3 *db,
									__in wchar_t *Word,
									__in wchar_t *Translation,
									__in wchar_t *Type)
{
	wchar_t *SqlQuery = NULL;
	int SqlQuery_len;
	int rc;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(db == NULL || Word == NULL || wcslen(Word) <= 0 || 
		Translation == NULL || wcslen(Translation) <= 0 || 
		Type == NULL || wcslen(Type) <= 0){
			Deb->dprint(Mode, L"@InsertTranslation: Wrong values!\r\n");
			return -1;
	}
	Network *Net = new Network();
	wchar_t *escaped = Net->ReplaceWString(L"'", L"''", Translation);
//	MessageBoxW(0,Translation,L"escaped",0);
	SqlQuery_len = sizeof(wchar_t*) * (wcslen(Word)+wcslen(escaped) + wcslen(Type) + MAX_PATH);
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
//	SqlQuery = (wchar_t *)NtMalloc(0, 0, SqlQuery_len, 0);
	swprintf(SqlQuery, SqlQuery_len, L"INSERT INTO Translations (Id, WORD_NAME, WORD_TRANSLATION, TRANSLATION_TYPE) VALUES (NULL, '%s', '%s', '%s')", 
							Word, escaped, Type);
	rc = sqlite3_exec16(db, SqlQuery);
//	NtMalloc(0, 1, 0, (LPVOID)SqlQuery);
	free(SqlQuery);
	
	delete Net;
	return 0;
}

int DeleteTranslation(__in sqlite3 *db,
								__in wchar_t *Word,
								__in wchar_t *Type)
{
	wchar_t *SqlQuery;
	int SqlQuery_len, rc;
	bool Mode;
	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;
	if(Word == NULL || wcslen(Word) <= 0 || Type == NULL || wcslen(Type) <= 0){
		Deb->dprint(Mode, L"Word is nul.\r\n");
		return -1;
	}
	SqlQuery_len = sizeof(wchar_t*) * (wcslen(Word) + wcslen(Type)) + 1024;
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, L"DELETE FROM Translations WHERE WORD_NAME = '%s' AND TRANSLATION_TYPE = '%s'", 
		Word, Type);
	rc = sqlite3_exec16(db, SqlQuery);
	free(SqlQuery);
	return 0;
}

int UpdateTranslation(__in sqlite3 *db,
								__in wchar_t *Word,
								__in wchar_t *Translation,
								__in wchar_t *Type)
{
	Network *Net = new Network();
//	MessageBoxW(0,Translation,NULL,0);
	wchar_t *escaped = Net->ReplaceWString(L"'", L"''", Translation);
//	MessageBoxW(0,escaped,NULL,0);
	int SqlQuery_len = sizeof(wchar_t*) * (wcslen(Word) + wcslen(escaped) + wcslen(Type)) + 1024;
	wchar_t *SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, 
		L"UPDATE Translations SET WORD_TRANSLATION = '%s' WHERE WORD_NAME = '%s' AND TRANSLATION_TYPE = '%s'",
		escaped, Word, Type);
	int rc = sqlite3_exec16(db, SqlQuery);
	free(SqlQuery);
	delete Net;
	return rc;
}

wchar_t *SelectTranslation(__in sqlite3 *db,
						   __in wchar_t *Word,
						   __in wchar_t *TranslationType)
{
	wchar_t *SqlQuery = NULL;
	int SqlQuery_len;
	char debug[1024] = {0};
	sqlite3_stmt *stmt;
	int rc; 
	static wchar_t *output;
	bool Mode;

	Opt.DebugLogToFile == 1 ? Mode = true : Mode = false;

	if(Word == NULL || wcslen(Word) <= 0 || db == NULL || 
		TranslationType == NULL || wcslen(TranslationType) == 0 || 
		wcscmp(TranslationType, L"") == 0){
		Deb->dprint(Mode, L"@SelectTranslation: Wrong values!\r\n");
		return NULL;
	} else {
		Deb->dprint(Mode, L"Searching for %s in local database..\r\n", Word);
	}

	SqlQuery_len = sizeof(wchar_t *) * (wcslen(Word) + wcslen(TranslationType)) + 1024;
//	SqlQuery = (wchar_t *)NtMalloc(0, 0, SqlQuery_len, 0);
	SqlQuery = (wchar_t *)malloc(SqlQuery_len);
	swprintf(SqlQuery, SqlQuery_len, L"SELECT * FROM Translations WHERE WORD_NAME = '%s' AND TRANSLATION_TYPE = '%s'", 
		Word, TranslationType);
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
				Deb->dprint(Mode, L"@SelectTranslation: %s not found.\r\n", Word);
				output = NULL;
			}
		break;

		case SQLITE_ROW:
			{
				int output_len = sizeof(wchar_t *) * wcslen((LPWSTR)sqlite3_column_text16(stmt, 2)) + MAX_PATH;
				output = (wchar_t*)malloc(output_len);
				swprintf(output, output_len, L"%s", (LPWSTR)sqlite3_column_text16(stmt, 2));
			}
		break;

		default:
			{
				Deb->dprint(Mode, L"@SelectTranslation: sqlite3_step error!\r\n");
				output = NULL;
			}
		break;
	}
	sqlite3_finalize(stmt);
	free(SqlQuery);
//	NtMalloc(0, 1, 0, (LPVOID)SqlQuery);
	return output;
}

#endif