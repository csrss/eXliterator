#ifndef _RADIOCONTROL_H
#define _RADIOCONTROL_H

class RadioControl{
protected:
	HWND hRadioControl;
	HWND pArent;
	HICON hIcon;

public: 
	DWORD dwStyle;
private:

	HFONT        m_hfArial, hfOld;

	int CreateRadio(__in HWND hParent, 
							  __in int x, 
							  __in int y, 
							  __in int nWidth, 
							  __in int nHeight, 
							  __in int CtrlID,
							  __in BOOL FontItalic,
							  __in BOOL FontUnderline,
							  __in int FontSize,
							  __in wchar_t *HeaderText)
	{
		if(!hRadioControl)
			hRadioControl = CreateWindowExW(0, L"button", HeaderText, 
											dwStyle,
											x, y, nWidth, nHeight, hParent, (HMENU)CtrlID, 
											(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
											NULL);
		else return -1;	// already here
		if(!hRadioControl) return GetLastError();
		LOGFONT lfArial = {
				-MulDiv(FontSize, GetDeviceCaps(GetDC(hParent), LOGPIXELSY), 72), 0, 0, 0,
				FW_MEDIUM, FontItalic, FontUnderline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Georgia"
			};
		m_hfArial = CreateFontIndirectW(&lfArial);
		hfOld = (HFONT)SelectObject(GetDC(hParent), (void*)(HFONT)m_hfArial);
		SendMessageW(hRadioControl, WM_SETFONT, (WPARAM)m_hfArial, MAKELPARAM(1, 0));
		return TRUE;
	}

public:

	operator HWND(){ return hRadioControl; }

	RadioControl(BOOL isLeader, BOOL isIconic, BOOL isPushLike){
		pArent = NULL;
		hRadioControl = NULL;
		hIcon = NULL;
		dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
		if(isLeader) dwStyle |= WS_GROUP;
		if(isIconic) dwStyle |= BS_ICON;
		if(isPushLike) dwStyle |= BS_PUSHLIKE;
	}

	~RadioControl(){}

	int Create(__in HWND hParent, 
						 __in int x, 
						 __in int y, 
						 __in int nWidth, 
						 __in int nHeight, 
						 __in int CtrlID, 
						 __in BOOL FontItalic, 
						 __in BOOL FontUnderline, 
						 __in int FontSize,
						 __in wchar_t *HeaderText)
	{
		if(!pArent) pArent = hParent;
		return CreateRadio(hParent, 
						   x, 
						   y, 
						   nWidth, 
						   nHeight, 
						   CtrlID, 
						   FontItalic, 
						   FontUnderline, 
						   FontSize,
						   HeaderText);
	}

	BOOL ShowHide(__in BOOL Mode){
		if(!hRadioControl) return -1;
		return ShowWindow(hRadioControl, Mode ? SW_SHOW : SW_HIDE);
	}

	int SetPosition(__in int x, __in int y){
		RECT rcWind;
		if(hRadioControl == NULL || x == 0 && y == 0) return -1;
		GetWindowRect(hRadioControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hRadioControl, x, y, rcWind.right, rcWind.bottom, TRUE);
	}

	int SetSize(__in int nWidth, __in int nHeight){
		RECT rcClient, rcWind;
		POINT ptDiff;
		if(hRadioControl == NULL || nWidth == 0 && nHeight == 0) return -1;
		GetClientRect(hRadioControl, &rcClient);
		GetWindowRect(hRadioControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hRadioControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	__w64 long SetText(__in wchar_t *Text){
		if(Text == NULL || wcslen(Text) == 0) return -1;
		return SendMessageW (hRadioControl, WM_SETTEXT, 0, (LPARAM)Text);
	}

	__w64 long SetIcon(__in int IconID){
		if(!hIcon)hIcon = (HICON)LoadImageW( GetModuleHandleW(0),
										MAKEINTRESOURCE(IconID),
										IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		else return -1;	// icon is loaded already
		if(!hIcon) return GetLastError();
		return SendMessageW(hRadioControl, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM)(DWORD)hIcon );
	}

	BOOL UnsetIcon(void){
		if(hIcon){
			DestroyIcon(hIcon);
			hIcon = NULL;
			return TRUE;
		}
		return FALSE;	// there is no icon
	}

	__w64 long ResetIcon(__in int newIconID){
		UnsetIcon();
		return SetIcon(newIconID);
	}

	__w64 long SetChecked(__in BOOL Mode){
		return SendMessageW(hRadioControl, BM_SETCHECK, Mode ? BST_CHECKED : BST_UNCHECKED, 0L);
	}

	__w64 long GetCheck(){
		return SendMessageW(hRadioControl, BM_GETCHECK, 0, 0L);
	}

	int Width(){
		RECT rcWind;
		if(hRadioControl == NULL) return -1;
		GetWindowRect(hRadioControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.right;
	}

	int Height(){
		RECT rcWind;
		if(hRadioControl == NULL) return -1;
		GetWindowRect(hRadioControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.bottom;
	}

};

#endif 