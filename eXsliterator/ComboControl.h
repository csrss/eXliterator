#ifndef _COMBOCONTROL_H
#define _COMBOCONTROL_H

class ComboControl {
protected:
	HWND hComboControl;
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
	HFONT        m_hfArial, hfOld;

	int CreateCombo(__in HWND hParent, 
							  __in int x, 
							  __in int y, 
							  __in int nWidth, 
							  __in int nHeight, 
							  __in int CtrlID,
							  __in BOOL FontItalic,
							  __in BOOL FontUnderline,
							  __in int FontSize)
	{
		if(!hComboControl)
			hComboControl = CreateWindowExW(0, L"ComboBox", L"", 
											WS_CHILD | WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
											x, y, nWidth, nHeight, hParent, (HMENU)CtrlID, 
											(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
											NULL);
		else return -1;	// already here
		if(!hComboControl) return GetLastError();
		LOGFONT lfArial = {
				-MulDiv(FontSize, GetDeviceCaps(GetDC(hParent), LOGPIXELSY), 72), 0, 0, 0,
				FW_MEDIUM, FontItalic, FontUnderline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Georgia"
			};
		m_hfArial = CreateFontIndirectW(&lfArial);
		hfOld = (HFONT)SelectObject(GetDC(hParent), (void*)(HFONT)m_hfArial);
		SendMessageW(hComboControl, WM_SETFONT, (WPARAM)m_hfArial, MAKELPARAM(1, 0));
		return TRUE;
	}

public:
	operator HWND(){ return hComboControl; }
	ComboControl(){
		hComboControl = NULL;
		m_hfArial = NULL;	
	}
	~ComboControl(){
		hComboControl = NULL;
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
		return CreateCombo(hParent, 
						   x, 
						   y, 
						   nWidth, 
						   nHeight, 
						   CtrlID, 
						   FontItalic, 
						   FontUnderline, 
						   FontSize);
	}

	int SetPosition(__in int x, __in int y){
		RECT rcWind;
		if(hComboControl == NULL || x == 0 && y == 0) return -1;
		GetWindowRect(hComboControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hComboControl, x, y, rcWind.right, rcWind.bottom, TRUE);
	}

	int SetSize(__in int nWidth, __in int nHeight){
		RECT rcClient, rcWind;
		POINT ptDiff;
		if(hComboControl == NULL || nWidth == 0 && nHeight == 0) return -1;
		GetClientRect(hComboControl, &rcClient);
		GetWindowRect(hComboControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hComboControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	__w64 long InsertString(__in wchar_t *Text){
		if(Text == NULL || wcslen(Text) == 0) return -1;
		return SendMessageW(hComboControl, CB_ADDSTRING, 0, (__w64 long)Text);
	}

	__w64 long RemoveStringByIndex(__in int Index){
		return SendMessageW(hComboControl, CB_DELETESTRING, Index, 0);
	}

	__w64 long SetActiveStringByIndex(__in int Index){
		return SendMessageW(hComboControl, CB_SETCURSEL, Index, 0);
	}

	int GetActiveIndex(void){
		return SendMessageW(hComboControl, CB_GETCURSEL, 0, 0);
	}

	wchar_t * GetActiveText(void){
		wchar_t * lpszString;
		int TextLength;
		int nCurSel = GetActiveIndex();
		if (nCurSel != -1){
			TextLength = SendMessageW(hComboControl, CB_GETLBTEXTLEN, nCurSel, 0);
			lpszString = (wchar_t *)malloc(sizeof(wchar_t*) * TextLength + 1);
			if (lpszString != NULL){
				SendMessageW(hComboControl, CB_GETLBTEXT, nCurSel, (LPARAM)lpszString);
				return lpszString;
			}
		}
		return NULL;
	}

	__w64 long ClearList(void){
		return SendMessageW(hComboControl, CB_RESETCONTENT, 0, 0);
	}

	BOOL ShowHide(__in BOOL Mode){
		if(!hComboControl) return -1;
		return ShowWindow(hComboControl, Mode ? SW_SHOW : SW_HIDE);
	}

	// SendMessageW(hComboControl, CB_SETDROPPEDWIDTH, 240, 0);

};

#endif