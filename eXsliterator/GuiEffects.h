#ifndef _GUIFFECTS_H
#define _GUIFFECTS_H

typedef struct _MARGINS
{
             int Left;
             int Right; 
             int Top; 
             int Bottom; 
} MARGINS; 

typedef HANDLE HTHEME;
#define RECTWIDTH(lpRect) ((lpRect).right - (lpRect).left)
#define RECTHEIGHT(lpRect) ((lpRect).bottom - (lpRect).top)
typedef int (WINAPI *DTT_CALLBACK_PROC)(HDC hdc,
										LPWSTR pszText,
										int cchText,
										LPRECT prc,
										UINT dwFlags,
										LPARAM lParam);

typedef struct _DTTOPTS {
  DWORD             dwSize;
  DWORD             dwFlags;
  COLORREF          crText;
  COLORREF          crBorder;
  COLORREF          crShadow;
  int               iTextShadowType;
  POINT             ptShadowOffset;
  int               iBorderSize;
  int               iFontPropId;
  int               iColorPropId;
  int               iStateId;
  BOOL              fApplyOverlay;
  int               iGlowSize;
  DTT_CALLBACK_PROC pfnDrawTextCallback;
  LPARAM            lParam;
} DTTOPTS, *PDTTOPTS;

#define DTT_COMPOSITED (1UL << 13)
#define DTT_GLOWSIZE (1UL << 11)
#define TMT_CAPTIONFONT 0x0321

typedef HTHEME(WINAPI *OPENTD)(HWND hwnd, LPCWSTR pszClassList);	//UxTheme.dll
typedef HRESULT (WINAPI *GETTSF)(HTHEME hTheme, int iFontID, LOGFONTW *plf);
typedef HRESULT  (WINAPI *DRAWTTE)(	__in     HTHEME hTheme,
									__in     HDC hdc,
									__in     int iPartId,
									__in     int iStateId,
									__in     LPCWSTR pszText,
									__in     int iCharCount,
									__in     DWORD dwFlags,
									__inout  LPRECT pRect,
									__in     const DTTOPTS *pOptions);
typedef HRESULT (WINAPI *CLOSETD)(__in  HTHEME hTheme);

typedef long(__stdcall *DEFIC)(HWND hWnd, const MARGINS *pMarInset);
typedef long(__stdcall *DICE)(bool *pfEnabled);

DEFIC pfnDwmExtendFrameIntoClientArea; 
DICE pfnDwmIsCompositionEnabled; 
OPENTD pfnOpenThemeData;
GETTSF pfnGetThemeSysFont;
DRAWTTE pfnDrawThemeTextEx;
CLOSETD pfnCloseThemeData;

class GuiEffects {
protected:

public:
	HDC PrevDc;
	BITMAP BkgImage;
	bool glassEnabled;
public:

	static const int GLASS_ENABLED = 10;
	static const int GLASS_DISABLED = 20;
private:

	MARGINS Marg;
	int UxStatus, DwmStatus;

	int __stdcall GetDwmStyles(){
		HMODULE hDwm = LoadLibraryW(L"dwmapi.dll");
		if(!hDwm) return -1;
		pfnDwmExtendFrameIntoClientArea = 
			(DEFIC) GetProcAddress(hDwm, "DwmExtendFrameIntoClientArea");
		pfnDwmIsCompositionEnabled = 
			(DICE) GetProcAddress(hDwm, "DwmIsCompositionEnabled");
		if(pfnDwmExtendFrameIntoClientArea && pfnDwmIsCompositionEnabled) return 0;
		return -1;
	}

	int __stdcall GetUxStyles(){
		HMODULE hUx = LoadLibraryW(L"UxTheme.dll");
		if(!hUx) return -1;
		pfnOpenThemeData = (OPENTD) GetProcAddress(hUx, "OpenThemeData");
		pfnGetThemeSysFont = (GETTSF) GetProcAddress(hUx, "GetThemeSysFont");
		pfnDrawThemeTextEx = (DRAWTTE) GetProcAddress(hUx, "DrawThemeTextEx");
		pfnCloseThemeData = (CLOSETD) GetProcAddress(hUx, "CloseThemeData");
		if(pfnOpenThemeData && pfnGetThemeSysFont &&
			pfnDrawThemeTextEx && pfnCloseThemeData) return 0;
		return -1;
	}

	int __stdcall InitGlass(HWND hWnd, bool ModeEx){
		unsigned int Status;
//		bool Mode;
	//	MARGINS Marg;
		OSVERSIONINFO VersionInfo;
		ZeroMemory(&VersionInfo, sizeof(VersionInfo));
		VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
//		bool glassEnabled = false;
	
		GetVersionEx(&VersionInfo);
		if(VersionInfo.dwMajorVersion < 6){
			MessageBoxW(hWnd, L"This system has no support for Vista Glass!", L"ERROR", MB_ICONERROR);
			return GLASS_DISABLED;
		}
		if(GetDwmStyles() == -1) return GLASS_DISABLED;
	//	TranslitOptions.debug_info == 1 ? Mode = true : Mode = false;
			if(pfnDwmIsCompositionEnabled(&glassEnabled) == S_OK){
				OutputDebugStringW(L"pfnDwmIsCompositionEnabled returned enabled");
				Marg.Left = 0;
				Marg.Right = 0;
				if(ModeEx){
					Marg.Top = 30; 
					Marg.Bottom = 27;
				} else {
					Marg.Top = 0; 
					Marg.Bottom = 0;
				}
				long shit;
				if(shit = pfnDwmExtendFrameIntoClientArea(hWnd,&Marg) != (long)0L){
					wchar_t debug[MAX_PATH];
					swprintf(debug, MAX_PATH, L"pfnDwmExtendFrameIntoClientArea statu: 0x%08x", shit);
					OutputDebugStringW(debug);
					Status = GLASS_DISABLED;
				} else {
					if(ModeEx){
						OutputDebugStringW(L"Glass effect enabled");
		//				if(hStatus)DestroyWindow(hStatus);
		//				SetMenu(hWnd, NULL);
						Status = GLASS_ENABLED;
					} else {
						OutputDebugStringW(L"Glass effect disabled");
		//				SetMenu(hWnd, hMainUpperMenu);
		//				hStatus = CreateStatusBar(hWnd, 3);
						Status = GLASS_DISABLED;
					}
				}
			} else {
	//			dbgprint("Initializing vista glass error! Composition is not enabled.\r\n");
				Status = GLASS_DISABLED;
			}
	//	}
		return Status;
	}

	int EraseTopBottom(int Top, int Bottom, HWND hWnd, HDC hdc){
			RECT lpRect;
			if(!GetClientRect(hWnd, &lpRect)) return -1;
			HBRUSH blueBrush=CreateSolidBrush(RGB(0,0,0));
			lpRect.bottom = Top;
			if(FillRect(hdc, &lpRect, blueBrush) == 0) return -1;

			RECT lpRectEx;
			if(!GetClientRect(hWnd, &lpRectEx)) return -1;
			RECT lol;
			lol.left = 0;
			lol.top =  lpRectEx.bottom - Top - (Bottom - Top);
			lol.right = lpRectEx.right - 0 - 0;
			lol.bottom = lpRectEx.bottom;
			if(FillRect(hdc, &lol, blueBrush) == 0) return -1;
		return 0;
	}

	int OutputGlowingText(HWND hWnd, 
						  HDC hdc, 
						  wchar_t *szTitle, 
						  int FromTopBorder,
						  int FromLeftBorder)
	{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
	
	if(UxStatus == -1) return -1;
    HTHEME hTheme = pfnOpenThemeData(NULL, L"Button");
    if (hTheme){
        HDC hdcPaint = CreateCompatibleDC(hdc);
        if (hdcPaint){
            int cx = RECTWIDTH(rcClient);
            int cy = RECTHEIGHT(rcClient);
			int BIT_COUNT = 32;
            BITMAPINFO dib = { 0 };
            dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
            dib.bmiHeader.biWidth           = cx;
            dib.bmiHeader.biHeight          = -cy;
            dib.bmiHeader.biPlanes          = 1;
            dib.bmiHeader.biBitCount        = BIT_COUNT;
            dib.bmiHeader.biCompression     = BI_RGB;

            HBITMAP hbm = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
            if (hbm){
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);

                // Setup the theme drawing options.
                DTTOPTS DttOpts = {sizeof(DTTOPTS)};
                DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
                DttOpts.iGlowSize = 10;
				DttOpts.crText   = RGB(255, 255, 255);
                // Select a font.
                LOGFONT lgFont;
                HFONT hFontOld = NULL;
                if (SUCCEEDED(pfnGetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont))){
                    HFONT hFont = CreateFontIndirect(&lgFont);
                    hFontOld = (HFONT) SelectObject(hdcPaint, hFont);
                }

                // Draw the title.
                RECT rcPaint = rcClient;
				rcPaint.top = FromTopBorder-(RECTHEIGHT(rcClient)-Marg.Bottom);
				rcPaint.left = FromLeftBorder;
                pfnDrawThemeTextEx(hTheme, 
                                hdcPaint, 
                                0, 0, 
                                szTitle, 
                                -1, 
                                DT_CENTER | DT_WORD_ELLIPSIS | DT_SINGLELINE, 
                                &rcPaint, 
                                &DttOpts);

                // Blit text to the frame.
//                BitBlt(hdc, 0, 5, cx, cy, hdcPaint, 0, 0, SRCCOPY);
				BitBlt(hdc, 0, RECTHEIGHT(rcClient)-Marg.Bottom, cx, cy, hdcPaint, 0, 0, SRCCOPY);
                SelectObject(hdcPaint, hbmOld);
                if (hFontOld) SelectObject(hdcPaint, hFontOld);
                DeleteObject(hbm);
            } else return -2; // CreateDIBSection failed
			ReleaseDC(hWnd, hdcPaint);
            DeleteDC(hdcPaint);
        } else return -1; // CreateCompatibleDC failed
        pfnCloseThemeData(hTheme);
    } else return -3; // pfnOpenThemeData failed
	return 0;
	}

	int PrintUIText(HDC hdc,
							  HWND hWnd,		// main windows handle
							  int FontSize,		// font size, like 12, or 15
							  int TextWidth,	// just a text width
							  int TxtRotation,	// first letter goes down
							  int Italic,		// 1 or 0
							  int Underline,	// 1 or 0
							  wchar_t *Text,	// text we want to be printed
							  long rgbColor,	// RGB color value
							  int FromLeft,		// from left border of main window
							  int FromTop)		// from top border of main window
	{
		HFONT m_hfArial = NULL;
		HFONT hfOld;
		LOGFONT lfArial = {
			-MulDiv(FontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), 
			TextWidth, TxtRotation, 0,
			FW_MEDIUM, Italic, Underline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, TEXT("Georgia")
		};
		m_hfArial = CreateFontIndirect(&lfArial);
		hfOld = SelectFont(hdc, m_hfArial);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, rgbColor);
		TextOut(hdc,FromLeft,FromTop,Text,wcslen(Text));
	return 0;
	}

	int SetControlPosition(__in HWND hControl, __in int x, __in int y){
		RECT rcWind;
		if(hControl == NULL || x == 0 && y == 0) return -1;
		GetWindowRect(hControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hControl, x, y, rcWind.right, rcWind.bottom, TRUE);
	}

	int SetControlSize(__in HWND hControl, __in int nWidth, __in int nHeight){
		RECT rcClient, rcWind;
		POINT ptDiff;
		if(hControl == NULL || nWidth == 0 && nHeight == 0) return -1;
		GetClientRect(hControl, &rcClient);
		GetWindowRect(hControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	int SetExplorerStyle(HWND hItem){
		wchar_t *Explo = L"Explorer";
		DWORD SetWindowTheme = 
			(DWORD)GetProcAddress(LoadLibraryW(L"UxTheme.dll"), "SetWindowTheme");
		if(!SetWindowTheme) return -1;
		__asm {
			push 0
			push Explo
			push hItem
			call dword ptr SetWindowTheme
		}
	}

	BITMAP LoadImageBkg(__in HINSTANCE hInst, 
						__in LPTSTR lpszResName, 
						__in LPTSTR lpszResType, 
						__in HDC m_hdcSurface)
	{
		BITMAP  m_bmSurface = {0};
		HRSRC   hResInfo   = FindResource(hInst, lpszResName, lpszResType);
		HGLOBAL hResGlobal = LoadResource(hInst, hResInfo);
		DWORD   dwSize     = SizeofResource(hInst, hResInfo);
		HGLOBAL hgImage    = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		CopyMemory(GlobalLock(hgImage), LockResource(hResGlobal), dwSize);
		GlobalUnlock(hgImage); 
		UnlockResource(hResGlobal);
		
		IStream *isImage = NULL;
		IPicture* pImage = NULL;
		CreateStreamOnHGlobal(hgImage, TRUE, &isImage);
		if(isImage == NULL){ 
			GlobalFree(hgImage); 
		} else {	
			OleLoadPicture(isImage, dwSize, FALSE, IID_IPicture, (LPVOID *)&pImage);
			isImage->Release();
			if(pImage != NULL){
				HBITMAP hImage = NULL;
				pImage->get_Handle((OLE_HANDLE *)&hImage);
				HBITMAP hResult = (HBITMAP)CopyImage(hImage, IMAGE_BITMAP, 0, 0, 0);
				pImage->Release();
				INT m_ndcSurface = SaveDC(m_hdcSurface);
				SelectBitmap(m_hdcSurface, hResult);
				GetObject(hResult, sizeof(BITMAP), &m_bmSurface);
				return m_bmSurface;
			}
		}
		return m_bmSurface;
	}

public:
	GuiEffects(){
		DwmStatus = GetDwmStyles();
		UxStatus = GetUxStyles();
		glassEnabled = false;
	}
	~GuiEffects(){}

	int InitializeGlass(__in HWND hWnd, __in bool ModeEx){
		return InitGlass(hWnd, ModeEx);
	}

	int PaintMargins(int Top, int Bottom, HWND hWnd, HDC hdc){
		return EraseTopBottom(Top, Bottom, hWnd, hdc);
	}

	int OutputGlowText(__in HWND hWnd, 
					   __in HDC hdc, 
					   __in wchar_t *szTitle, 
					   __in int FromTopBorder,
					   __in int FromLeftBorder)
	{
		return OutputGlowingText(hWnd, hdc, szTitle, FromTopBorder, FromLeftBorder);
	}

	int PrintUiText(HDC hdc,
							  HWND hWnd,		// main windows handle
							  int FontSize,		// font size, like 12, or 15
							  int TextWidth,	// just a text width
							  int TxtRotation,	// first letter goes down
							  int Italic,		// 1 or 0
							  int Underline,	// 1 or 0
							  wchar_t *Text,	// text we want to be printed
							  long rgbColor,	// RGB color value
							  int FromLeft,		// from left border of main window
							  int FromTop)
	{
		return PrintUIText(hdc, hWnd, FontSize, TextWidth, TxtRotation, Italic,
			Underline, Text, rgbColor, FromLeft, FromTop);
	}

	int NewControlPosition(__in HWND hControl, __in int x, __in int y){
		return SetControlPosition(hControl, x, y);
	}

	int NewControlSize(__in HWND hControl, __in int nWidth, __in int nHeight){
		return SetControlSize(hControl, nWidth, nHeight);
	}

	int ApplyExplorerStyle(HWND hItem){
		return SetExplorerStyle(hItem);
	}

	int GdiPaintBkg(	__in Gdiplus::Color color,
								__in_opt HWND hWnd = NULL,
								__in_opt HDC hdc = NULL	)
	{	
		if(hWnd == NULL && hdc == NULL) return -2;
		Gdiplus::Graphics gr(hdc ? hdc : GetDC(hWnd));
		gr.Clear(Color(color));
		if(hWnd) InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	}

	BITMAP InitBkgImage(__in HWND hWnd, __in int ImageId, __out HDC *PrevDC){
		HDC m_hdcSurface = CreateCompatibleDC(NULL);
		*PrevDC = m_hdcSurface;
		return LoadImageBkg((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
											MAKEINTRESOURCE(ImageId), 
											RT_RCDATA, m_hdcSurface);
	}

	void PaintBkgImage(__in HWND hWnd, 
								 __in HDC PaintDC, 
								 __in BITMAP bmp, 
								 __in HDC prevDC)
	{
		RECT m_rcClient;
		GetClientRect(hWnd, &m_rcClient);
		SetStretchBltMode(PaintDC, HALFTONE);
		StretchBlt(PaintDC, 0, 0, m_rcClient.right, m_rcClient.bottom,
			   prevDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}

};

#endif 