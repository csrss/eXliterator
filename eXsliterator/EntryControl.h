#ifndef _ENTRYCONTROL_H
#define _ENTRYCONTROL_H

class EntryControl {

protected:

	HWND hEntryControl;
	HWND pArent;

private:
/*
#ifndef walloc
	#define walloc(Size) (wchar_t*)HeapAlloc(GetProcessHeap(), \
						HEAP_ZERO_MEMORY, sizeof (wchar_t*) * Size + 1)
#endif
#ifndef ufree
	#define ufree(Addr)	HeapFree(GetProcessHeap(), 0, (LPVOID)Addr)
#endif
*/ 
	HFONT m_hfArial, hfOld;
	DWORD dwStyle, dwExStyle;

	int CreateEntry(__in HWND hParent, 
							  __in int x, 
							  __in int y, 
							  __in int nWidth, 
							  __in int nHeight, 
							  __in int CtrlID,
							  __in BOOL FontItalic,
							  __in BOOL FontUnderline,
							  __in int FontSize)
	{
		if(!hEntryControl)
			hEntryControl = CreateWindowExW(dwExStyle, L"edit", L"", 
											dwStyle,
											x, y, nWidth, nHeight, hParent, (HMENU)CtrlID, 
											(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
											NULL);
		else return -1;	// already here
		if(!hEntryControl) return GetLastError();
		LOGFONT lfArial = {
				-MulDiv(FontSize, GetDeviceCaps(GetDC(hParent), LOGPIXELSY), 72), 0, 0, 0,
				FW_MEDIUM, FontItalic, FontUnderline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Georgia"
			};
		m_hfArial = CreateFontIndirectW(&lfArial);
		hfOld = (HFONT)SelectObject(GetDC(hParent), (void*)(HFONT)m_hfArial);
		SendMessageW(hEntryControl, WM_SETFONT, (WPARAM)m_hfArial, MAKELPARAM(1, 0));
		return TRUE;
	}

public:
	
	operator HWND(){ return hEntryControl; }

	EntryControl(__in BOOL isMultiline, __in BOOL StaticEdge){
		dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER;
		dwExStyle = 0;
		hEntryControl = NULL;
		m_hfArial = NULL;

		if(isMultiline)dwStyle |= ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL;
		else dwStyle |= ES_AUTOHSCROLL;

		if(StaticEdge)dwExStyle |= WS_EX_STATICEDGE;
	}
	
	~EntryControl(void){
		hEntryControl = NULL;
		SelectObject(GetDC(pArent), hfOld);
		if(m_hfArial) DeleteObject(m_hfArial);
	}
	
	int Create(__in HWND hParent, 
						 __in int x, 
						 __in int y, 
						 __in int nWidth, 
						 __in int nHeight, 
						 __in int CtrlID, 
						 __in BOOL FontItalic, 
						 __in BOOL FontUnderline, 
						 __in int FontSize)
	{
		if(!pArent) pArent = hParent;
		return CreateEntry(hParent, 
						   x, 
						   y, 
						   nWidth, 
						   nHeight, 
						   CtrlID, 
						   FontItalic, 
						   FontUnderline, 
						   FontSize);
	}

	int SetPosition(__in int x, 
							  __in int y,
							  __in_opt HWND hControl = NULL)
	{
		RECT rcWind;
		if((!hEntryControl && !hControl) || (x == 0 && y == 0)) return -1;
		GetWindowRect(hControl ? hControl : hEntryControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hControl ? hControl : hEntryControl, x, y, 
			rcWind.right, rcWind.bottom, TRUE);
	}

	int SetSize(__in int nWidth, 
						  __in int nHeight,
						  __in_opt HWND hControl = NULL)
	{
		RECT rcClient, rcWind;
		POINT ptDiff;
		if((!hEntryControl && !hControl) || (nWidth == 0 && nHeight == 0)) return -1;
		GetClientRect(hControl ? hControl : hEntryControl, &rcClient);
		GetWindowRect(hControl ? hControl : hEntryControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hControl ? hControl : hEntryControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	wchar_t * GetText(__in_opt HWND hControl = NULL){
		if(!hEntryControl && !hControl) return NULL;
		int Length = (int)SendMessageW(hControl ? hControl : hEntryControl, 
									   WM_GETTEXTLENGTH, 
									   0, 
									   0L);
		static wchar_t *output = NULL;
			
		output = (wchar_t*)malloc(sizeof(wchar_t*)*Length+1);
		SendMessageW(hControl ? hControl : hEntryControl, 
					 WM_GETTEXT, 
					 (WPARAM)(int)Length+1, 
					 (LPARAM)output);
		return output;
	}

	__w64 long SetText(__in wchar_t *Text,
								 __in_opt HWND hControl = NULL)
	{
		if((!hEntryControl && !hControl) || !Text || wcslen(Text) == 0) return -1;
		return SendMessageW(hControl ? hControl : hEntryControl, 
							WM_SETTEXT, 
							0, 
							(LPARAM)Text);
	}

	__w64 long Clear(__in_opt HWND hControl = NULL){
		if(!hEntryControl && !hControl) return -1;
		return SendMessageW(hControl ? hControl : hEntryControl, 
							WM_SETTEXT, 
							0, 
							(LPARAM)L"");
	}

};

#endif