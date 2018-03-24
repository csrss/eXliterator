#ifndef _FRAMECONTROL_H
#define _FRAMECONTROL_H

class FrameControl {
protected:
	HWND hFrameControl;
	HWND pArent;
private:

	HFONT        m_hfArial, hfOld;

	int CreateFrame(__in HWND hParent, 
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
		if(!hFrameControl)
			hFrameControl = CreateWindowExW(0, L"button", HeaderText, 
											WS_VISIBLE | WS_CHILD | BS_GROUPBOX ,
											x, y, nWidth, nHeight, hParent, (HMENU)CtrlID, 
											(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
											NULL);
		else return -1;	// already here
		if(!hFrameControl) return GetLastError();
		LOGFONT lfArial = {
				-MulDiv(FontSize, GetDeviceCaps(GetDC(hParent), LOGPIXELSY), 72), 0, 0, 0,
				FW_MEDIUM, FontItalic, FontUnderline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Georgia"
			};
		m_hfArial = CreateFontIndirectW(&lfArial);
		hfOld = (HFONT)SelectObject(GetDC(hParent), (void*)(HFONT)m_hfArial);
		SendMessageW(hFrameControl, WM_SETFONT, (WPARAM)m_hfArial, MAKELPARAM(1, 0));
		return TRUE;
	}

public:
	operator HWND(){ return hFrameControl; }
	FrameControl(){
		hFrameControl = NULL;
		m_hfArial = NULL;
	}
	~FrameControl(){
		hFrameControl = NULL;
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
						 __in int FontSize,
						 __in wchar_t *HeaderText)
	{
		if(!pArent) pArent = hParent;
		return CreateFrame(hParent, 
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

	int SetPosition(__in int x, __in int y){
		RECT rcWind;
		if(hFrameControl == NULL || x == 0 && y == 0) return -1;
		GetWindowRect(hFrameControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hFrameControl, x, y, rcWind.right, rcWind.bottom, TRUE);
	}

	int SetSize(__in int nWidth, __in int nHeight){
		RECT rcClient, rcWind;
		POINT ptDiff;
		if(hFrameControl == NULL || nWidth == 0 && nHeight == 0) return -1;
		GetClientRect(hFrameControl, &rcClient);
		GetWindowRect(hFrameControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hFrameControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	BOOL ShowHide(__in BOOL Mode){
		if(!hFrameControl) return -1;
		return ShowWindow(hFrameControl, Mode ? SW_SHOW : SW_HIDE);
	}

	__w64 long SetText(wchar_t *Text){
		if(Text == NULL || wcslen(Text) == 0) return -1;
		return SendMessageW (hFrameControl, WM_SETTEXT, 0, (LPARAM)Text);
	}

};

#endif 