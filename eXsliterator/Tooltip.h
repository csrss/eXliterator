#ifndef _TOOLTIP_H
#define _TOOLTIP_H

class ToolTipWindow{

protected:

	HWND hToolTip;
	HWND hParentEx;
	int CtrlUID;

private:

	int SetTooltipWindow(__in int ItemUID, 
								   __in HWND hParent, 
								   __in wchar_t * pszText)
	{
		TOOLINFO toolInfo = { 0 };
		if (!ItemUID || !hParent || !pszText) return FALSE;
		HINSTANCE g_hInst = (HINSTANCE)GetWindowLongW(hParent, GWL_HINSTANCE);
		HWND hwndTool = GetDlgItem(hParent, ItemUID);
		if(!hwndTool){
			return GetLastError();
		}

		if(!hToolTip)
			hToolTip = CreateWindowExW(0, L"tooltips_class32", NULL,
										WS_POPUP |0x01 | 0x40,
										CW_USEDEFAULT, CW_USEDEFAULT,
										CW_USEDEFAULT, CW_USEDEFAULT,
										hParent, NULL, 
										g_hInst, NULL);
		else return -1;	// tooltip already exists

		if(!hToolTip) return GetLastError();

		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.hwnd = hParent;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (__w64 unsigned int)hwndTool;
		toolInfo.lpszText = pszText;
		SendMessageW(hToolTip, TTM_ADDTOOL, 0, (__w64 long)&toolInfo);
		SendMessageW(hToolTip, TTM_SETMAXTIPWIDTH, 0, 300);
		return TRUE;
	}

	int DestroyTooltipWindow(){
		if(hToolTip){
			if(!DestroyWindow(hToolTip)){
				return GetLastError();
			} else {
				hToolTip = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}

public:

	ToolTipWindow(void){
		hToolTip = NULL;
		hParentEx = NULL;
		CtrlUID = 0;
	}

	~ToolTipWindow(void){
		DestroyTooltipWindow();
		hToolTip = NULL;
		hParentEx = NULL;
		CtrlUID = 0;
	}

	HWND GetHWND(void){
		return hToolTip;
	}

	int SetTooltip(__in int ItemUID, 
							 __in HWND hParent, 
							 __in wchar_t * pszText)
	{
		if(!hParentEx) hParentEx = hParent;
		if(!CtrlUID)CtrlUID = ItemUID;
		return SetTooltipWindow(ItemUID, hParent, pszText);
	}

	int DelTooltip(void){
		return DestroyTooltipWindow();
	}

	int ResetText(__in wchar_t * NewText){
		DestroyTooltipWindow();
		return SetTooltipWindow(CtrlUID, hParentEx, NewText);
	}
};

ToolTipWindow *ToolTip[9];
ToolTipWindow *OptionsTooltip[19];
ToolTipWindow *DictTooltips[9];

int SetMainTooltips(HWND hWnd, int LangID){
	if(!Opt.Tooltips) return -1;
	int Status[9];
	Status[0] = ToolTip[0]->SetTooltip(TRANSLITERATED_DATAGRID, hWnd, translit_area_tool[LangID]);
	Status[1] = ToolTip[1]->SetTooltip(TRANSLATIONS_DATAGRID, hWnd, translation_area_tool[LangID]);
	Status[2] = ToolTip[2]->SetTooltip(RADIO_TRANSLIT, hWnd, main_radio_tool[LangID]);
	Status[3] = ToolTip[3]->SetTooltip(GLASS_COMBO_DICT_TYPE, hWnd, translation_dirsel_tool[LangID]);
	Status[4] = ToolTip[4]->SetTooltip(ENTRY_CONTROL, hWnd, word_entry_tool[LangID]);
	Status[5] = ToolTip[5]->SetTooltip(DGBTN_TRANSLATION_SEARCH, hWnd, fetch_button_tool[LangID]);
	Status[6] = ToolTip[6]->SetTooltip(DGBTN_DICTDB, hWnd, dictdb_button_tool[LangID]);
	Status[7] = ToolTip[7]->SetTooltip(DGBTN_SETTINGS, hWnd, settings_button_tool[LangID]);
	Status[8] = ToolTip[8]->SetTooltip(DGBTN_QUIT, hWnd, quit_button_tool[LangID]);
	return *Status;
}

int ResetMainTooltips(int LangID){
	if(!Opt.Tooltips) return -1;
	else if(!ToolTip[0]) return -1;
	int Status[8+1];
	Status[0] = ToolTip[0]->ResetText(translit_area_tool[LangID]);
	Status[1] = ToolTip[1]->ResetText(translation_area_tool[LangID]);
	Status[2] = ToolTip[2]->ResetText(main_radio_tool[LangID]);
	Status[3] = ToolTip[3]->ResetText(translation_dirsel_tool[LangID]);
	Status[4] = ToolTip[4]->ResetText(word_entry_tool[LangID]);
	Status[5] = ToolTip[5]->ResetText(fetch_button_tool[LangID]);
	Status[6] = ToolTip[6]->ResetText(dictdb_button_tool[LangID]);
	Status[7] = ToolTip[7]->ResetText(settings_button_tool[LangID]);
	Status[8] = ToolTip[8]->ResetText(quit_button_tool[LangID]);
	return *Status;
}

int DeleteMainTooltips(){
	if(!Opt.Tooltips) return -1;
	else if(!ToolTip[0]) return -1;
	int i = 8; while(i >= 0){ delete ToolTip[i]; i--; } return 0;
}

int SetOptionsTooltips(HWND hWnd, int LangID){
	if(!Opt.Tooltips) return -1;
	int i = 19;
	int Status[19+1];
	while(i >=0){
		OptionsTooltip[i] = new ToolTipWindow();
		i--;
	} 
	Status[0] = OptionsTooltip[0]->SetTooltip(IDC_COMBO1, hWnd, tool_lang_selection[LangID]);
	Status[1] = OptionsTooltip[1]->SetTooltip(IDC_COMBO2, hWnd, tool_visual_style[LangID]);
	Status[2] = OptionsTooltip[2]->SetTooltip(IDC_CHECK5, hWnd, bottom_text_tool[LangID]);
	Status[3] = OptionsTooltip[3]->SetTooltip(IDC_CHECK6, hWnd, buttons_icons_tool[LangID]);
	Status[4] = OptionsTooltip[4]->SetTooltip(IDC_RADIO1, hWnd, tool_autostart[LangID]);
	Status[5] = OptionsTooltip[5]->SetTooltip(IDC_RADIO2, hWnd, tool_manual_start[LangID]);
	Status[6] = OptionsTooltip[6]->SetTooltip(IDC_CHECK1, hWnd, tool_debug_log[LangID]);
	Status[7] = OptionsTooltip[7]->SetTooltip(IDC_CHECK2, hWnd, tool_global_ru[LangID]);
	Status[8] = OptionsTooltip[8]->SetTooltip(IDC_CHECK7, hWnd, show_tootips_tool[LangID]);
	Status[9] = OptionsTooltip[9]->SetTooltip(IDC_SLIDER1, hWnd, tool_conn_timeout[LangID]);
	Status[10] = OptionsTooltip[10]->SetTooltip(IDC_CHECK3, hWnd, tool_local_search[LangID]);
	Status[11] = OptionsTooltip[11]->SetTooltip(IDC_CHECK4, hWnd, tool_net_search[LangID]);
	Status[12] = OptionsTooltip[12]->SetTooltip(IDC_EDIT1, hWnd, app_activate_tool[LangID]);
	Status[13] = OptionsTooltip[13]->SetTooltip(IDC_EDIT2, hWnd, mflogin_tool[LangID]);
	Status[14] = OptionsTooltip[14]->SetTooltip(IDC_BUTTON2, hWnd, activate_button_tool[LangID]);
	Status[15] = OptionsTooltip[15]->SetTooltip(IDC_BUTTON1, hWnd, register_button_tool[LangID]);
	Status[16] = OptionsTooltip[16]->SetTooltip(IDOK, hWnd, close_button_tool[LangID]);
	Status[17] = OptionsTooltip[17]->SetTooltip(IDC_CHECK8, hWnd, show_col_headers_tool[LangID]);
	Status[18] = OptionsTooltip[18]->SetTooltip(IDC_BUTTON3, hWnd, top_grid_bkg_tool[LangID]);
	Status[19] = OptionsTooltip[19]->SetTooltip(IDC_BUTTON4, hWnd, bot_grid_bkg_tool[LangID]);
	return *Status;
}

int ResetOptionsTooltips(int LangID){
	if(!Opt.Tooltips) return -1;
	else if(!OptionsTooltip[0]) return -1;
	OptionsTooltip[0]->ResetText(tool_lang_selection[LangID]);
	OptionsTooltip[1]->ResetText(tool_visual_style[LangID]);
	OptionsTooltip[2]->ResetText(bottom_text_tool[LangID]);
	OptionsTooltip[3]->ResetText(buttons_icons_tool[LangID]);
	OptionsTooltip[4]->ResetText(tool_autostart[LangID]);
	OptionsTooltip[5]->ResetText(tool_manual_start[LangID]);
	OptionsTooltip[6]->ResetText(tool_debug_log[LangID]);
	OptionsTooltip[7]->ResetText(tool_global_ru[LangID]);
	OptionsTooltip[8]->ResetText(show_tootips_tool[LangID]);
	OptionsTooltip[9]->ResetText(tool_conn_timeout[LangID]);
	OptionsTooltip[10]->ResetText(tool_local_search[LangID]);
	OptionsTooltip[11]->ResetText(tool_net_search[LangID]);
	OptionsTooltip[12]->ResetText(app_activate_tool[LangID]);
	OptionsTooltip[13]->ResetText(mflogin_tool[LangID]);
	OptionsTooltip[14]->ResetText(activate_button_tool[LangID]);
	OptionsTooltip[15]->ResetText(register_button_tool[LangID]);
	OptionsTooltip[16]->ResetText(close_button_tool[LangID]);
	OptionsTooltip[17]->ResetText(show_col_headers_tool[LangID]);
	OptionsTooltip[18]->ResetText(top_grid_bkg_tool[LangID]);
	OptionsTooltip[19]->ResetText(bot_grid_bkg_tool[LangID]);
	return 0;
}

int DeleteOptionsTooltips(){
	if(!Opt.Tooltips) return -1;
	else if(!OptionsTooltip[0]) return -1;
	int i = 19; while(i >= 0){ delete OptionsTooltip[i]; i--; } return 0;
}

int SetDictTooltips(HWND hWnd, int LangID){
	if(!Opt.Tooltips) return -1;
	int i = 8;
	int Status[8+1];
	while(i >=0){
		DictTooltips[i] = new ToolTipWindow();
		i--;
	} 
	Status[0] = DictTooltips[0]->SetTooltip(IDC_EDIT1, hWnd, dict_tool_search[LangID]);
	Status[1] = DictTooltips[1]->SetTooltip(IDC_COMBO1, hWnd, dict_tool_dicttype[LangID]);
	Status[2] = DictTooltips[2]->SetTooltip(IDC_BUTTON3, hWnd, dict_tool_searchbtn[LangID]);
	Status[3] = DictTooltips[3]->SetTooltip(IDC_LIST1, hWnd, dict_tool_wordslist[LangID]);
	Status[4] = DictTooltips[4]->SetTooltip(IDC_EDIT2, hWnd, dict_tool_translation[LangID]);
	Status[5] = DictTooltips[5]->SetTooltip(IDOK, hWnd, dict_tool_insert[LangID]);
	Status[6] = DictTooltips[6]->SetTooltip(IDC_BUTTON1, hWnd, dict_tool_save[LangID]);
	Status[7] = DictTooltips[7]->SetTooltip(IDC_BUTTON2, hWnd, dict_tool_delete[LangID]);
	Status[8] = DictTooltips[8]->SetTooltip(IDCANCEL, hWnd, dict_tool_close[LangID]);

	return *Status;
}
int ResetDictTooltips(int LangID){
	if(!Opt.Tooltips) return -1;
	else if(!DictTooltips[0]) return -1;
	int Status[8+1];
	Status[0] = DictTooltips[0]->ResetText(dict_tool_search[LangID]);
	Status[1] = DictTooltips[1]->ResetText(dict_tool_dicttype[LangID]);
	Status[2] = DictTooltips[2]->ResetText(dict_tool_searchbtn[LangID]);
	Status[3] = DictTooltips[3]->ResetText(dict_tool_wordslist[LangID]);
	Status[4] = DictTooltips[4]->ResetText(dict_tool_translation[LangID]);
	Status[5] = DictTooltips[5]->ResetText(dict_tool_insert[LangID]);
	Status[6] = DictTooltips[6]->ResetText(dict_tool_save[LangID]);
	Status[7] = DictTooltips[7]->ResetText(dict_tool_delete[LangID]);
	Status[8] = DictTooltips[8]->ResetText(dict_tool_close[LangID]);

	return *Status;
}

int DeleteDictTooltips(){
	if(!Opt.Tooltips) return -1;
	else if(!DictTooltips[0]) return -1;
	int i = 8; while(i >= 0){ delete DictTooltips[i]; i--; } return 0;
}

#endif