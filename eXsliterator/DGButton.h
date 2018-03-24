#ifndef _DGBUTTON_H
#define _DGBUTTON_H
#pragma warning(disable: 4996)
HICON hDGicon, hNextIcon;

class DGButton{
protected:
	HWND m_hWnd;
	int ICON_PLACEMENT;
	BOOL IconOnly;
public:
	static const int ICON_POSITION_LEFT		= 10;
	static const int ICON_POSITION_RIGHT	= 11;
	static const int ICON_POSITION_CENTER	= 12;
	static const int ICON_POSITION_LEFT_UP	= 13;
	static const int ICON_POSITION_LEFT_BT	= 14;
	static const int ICON_POSITION_RIGHT_UP	= 15;
	static const int ICON_POSITION_RIGHT_BT	= 16;
	static const int ICON_POSITION_CUSTOM	= 20;

private:
		BOOL          m_bCacheDirty, m_bMouseTrack, m_bMouseHover, m_bMouseDown;
		CachedBitmap *m_pCachePaint;
		FLOAT         m_fGlowLevel;
		UINT          m_uGlowTimer;
		ARGB          m_argbGlow;
		WNDPROC Proc;
		BOOL		  isIconic;
	
		typedef struct _ICON_POS {
			int x;
			int y;
			int nWidth;
			int nHeight;
			int IconID;
		}ICON_POS, *PICON_POS;
		ICON_POS IconPos;

		int InitCustButton(__in long ControlStyle, // button custom style
									  __in HWND hParent,	// parent window
									  __in wchar_t *Text,	// text to print on button
									  __in int x,			// from left side
									  __in int y,			// from top border
									  __in int nWidth,
									  __in int nHeight,	
									  __in int iControl)	// button ID
		{
			WNDCLASSEXW wc;
			ATOM atom;
			LPWSTR pwstr;
			int wcsChars;
			static const long Glass_Button = 0x0000000a;
			static const long Iconi_Button = 0x0000000b;
			static const wchar_t DarkGlassControl[] = L"DarkGlassControl";
			static const wchar_t IconicControl[] = L"IconicControl";

			if(ControlStyle == Glass_Button) wcsChars = (int)wcslen(DarkGlassControl);
			else if(ControlStyle == Iconi_Button) wcsChars = (int)wcslen(IconicControl);
			else return NULL;

			if (GetClassInfoExW(NULL, DarkGlassControl, &wc) ||
				GetClassInfoExW(NULL, IconicControl, &wc)){
				OutputDebugStringW(L"Class already here!\n");
			} else {    
			    wc.cbClsExtra = 0;
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.cbWndExtra = 0;
				wc.hbrBackground = NULL;
			    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			    wc.hIcon = NULL;
			    wc.hIconSm = NULL;
			    wc.hInstance = NULL;  
				if(ControlStyle == Glass_Button)
					wc.lpfnWndProc = (WNDPROC)DGButton::s_WndProc;
				else if(ControlStyle == Iconi_Button)
					wc.lpfnWndProc = (WNDPROC)NULL;
				if(ControlStyle == Glass_Button)
				    wc.lpszClassName = DarkGlassControl;
				else if(ControlStyle == Iconi_Button)
					wc.lpszClassName = IconicControl;
			    wc.lpszMenuName = NULL;
			    wc.style = CS_PARENTDC | CS_VREDRAW | CS_HREDRAW;
			    atom = RegisterClassExW(&wc);
				if(!atom){
					if(GetLastError() != 1410) return -1;
				}
			}

			pwstr = (wchar_t *)malloc(wcsChars * sizeof (wchar_t*) + 1);
			if(ControlStyle == Glass_Button) wcscpy(pwstr, DarkGlassControl);
			else if(ControlStyle == Iconi_Button) wcscpy(pwstr, IconicControl);

			m_hWnd = CreateWindowExW(0,
									pwstr, 
									Text,
									WS_VISIBLE | WS_CHILD | WS_TABSTOP,
									x, // position and dimensions
									y,
									nWidth,
									nHeight,
									hParent, // parent window
									(HMENU)iControl,
									(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
									(LPVOID)this);
			free(pwstr);
			if(!m_hWnd) return -1;
			return 0;
		}

		BOOL Redraw(HWND m_hWnd, BOOL bUpdateNow){
			m_bCacheDirty = TRUE;
			if(bUpdateNow)
				return RedrawWindow(m_hWnd, NULL, NULL, RDW_NOERASE | 
					RDW_INVALIDATE | RDW_UPDATENOW);
			else
				return InvalidateRect(m_hWnd, NULL, FALSE);
		}

		GraphicsPath *CreateRoundRectPath(Rect r, int d){
			GraphicsPath *gp = new GraphicsPath();
			
			if(d > r.Width)  d = r.Width;
			if(d > r.Height) d = r.Height;
			
			Rect Corner(r.X, r.Y, d, d);
			
			gp->Reset();
			gp->AddArc(Corner, 180, 90);
			Corner.X += (r.Width - d - 1);
			gp->AddArc(Corner, 270, 90);
			Corner.Y += (r.Height - d - 1);
			gp->AddArc(Corner, 0, 90);
			Corner.X -= (r.Width - d - 1);
			gp->AddArc(Corner, 90, 90);
			gp->CloseFigure();
			
			return gp;
		}

		LRESULT OnSize(HWND m_hWnd, INT nWidth, INT nHeight){
			RECT m_rcClient;
			GetClientRect(m_hWnd, &m_rcClient);
			Graphics graphics(m_hWnd);
			Rect rect(0, 0, m_rcClient.right, m_rcClient.bottom);
			GraphicsPath *gp = CreateRoundRectPath(rect, 24);
			SetWindowRgn(m_hWnd, Region(gp).GetHRGN(&graphics), FALSE);
			delete gp;
			Redraw(m_hWnd, FALSE);
			return 0;
		}

		HDC PaintParentBackground(HWND m_hWnd, HDC hdc, RECT m_rcClient)
		{

			POINT ptOrg;
			HRGN  hRgn;
			HWND  hWndParent  = GetParent(m_hWnd);
			RECT  rcTranslate = m_rcClient;
			
			// Map our client area to its position relative to the client area of the parent
			MapWindowPoints(m_hWnd, hWndParent, (LPPOINT)&rcTranslate, 2);
			SetWindowOrgEx(hdc, rcTranslate.left, rcTranslate.top, &ptOrg);
			
			// Clip painting to our client area's width and height
			hRgn = CreateRectRgnIndirect(&m_rcClient);
			SelectClipRgn(hdc, hRgn);
			DeleteObject(hRgn);
			
			// Tell the parent to paint to our HDC
			SendMessage(hWndParent, WM_PRINTCLIENT, (WPARAM)hdc, PRF_CLIENT);
			
			// Turn off the clipping limit
			SelectClipRgn(hdc, NULL);
			
			// Reset the coordinate translation
			SetWindowOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
			
			return hdc;
		}

		LRESULT OnDestroy()
		{
			// Clean up allocated resources
			if(m_pCachePaint){
				delete m_pCachePaint;
				m_pCachePaint = NULL;
			}

			return 0;
		}

		void DrawRoundRect(Graphics *g, Rect rect, Color color, INT nDiameter)
		{
			GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
			Pen pen(color, 1);    
			pen.SetAlignment(PenAlignmentCenter);
			int oldPageUnit = g->SetPageUnit(UnitPixel);
			g->DrawPath(&pen, gp);
			g->SetPageUnit((Unit)oldPageUnit);
			delete gp;
		}

		LRESULT OnPaint(PAINTSTRUCT *ps, HWND m_hWnd)
		{
			RECT m_rcClient;
			GetClientRect(m_hWnd, &m_rcClient);
			Gdiplus::Graphics graphics(ps->hdc);
			Gdiplus::GraphicsPath *innerPath;
			// Do we need to redraw the window contents?
			if(m_bCacheDirty){
				// Create some GDI rect's
				Gdiplus::Rect rect(m_rcClient.left, m_rcClient.top,
					m_rcClient.right, m_rcClient.bottom);
				Gdiplus::RectF rectf ((float)m_rcClient.left, (float)m_rcClient.top,
					(float)m_rcClient.right, (float)m_rcClient.bottom);
				
				// Setup a double buffer for painting
				Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap(m_rcClient.right, m_rcClient.bottom);
				Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(bmp);

				// Setup nicer graphics and copy in the background
				g->SetSmoothingMode(SmoothingModeAntiAlias);
				g->ReleaseHDC(PaintParentBackground(m_hWnd, g->GetHDC(), m_rcClient));

				if(!IconOnly){
					// Draw double border of white and black
					Gdiplus::Rect border = rect;
					border.Inflate(-2, -2);
					DrawRoundRect(g, border, Gdiplus::Color::White, 5);
					border.Inflate(-1, -1);
					DrawRoundRect(g, border, Gdiplus::Color::Black, 5);
				
					// Create a path for us to work inside the borders
					innerPath = CreateRoundRectPath(border, 5);
				
					// Fill in the semi-transparent background depending on button state
					Gdiplus::SolidBrush background(Gdiplus::Color((m_bMouseDown) ? 0xCC000000 : 0x7F000000)); //0x7F000000
					g->FillPath(&background, innerPath);
				
					// Clip further painting operations to inside the border
					g->SetClip(innerPath);
				
					// Choose the shine colors depending on button state
					Gdiplus::Color colorShine1, colorShine2;
					if(m_bMouseDown){
						colorShine1.SetValue(0x3DFFFFFF), colorShine2.SetValue(0x14FFFFFF);
					}else{
						colorShine1.SetValue(0x99FFFFFF), colorShine2.SetValue(0x33FFFFFF);
					}

					// Add the button shine effect to the button
					Gdiplus::Rect shine = border;
					shine.Height /= 2;
					Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
						colorShine2, LinearGradientModeVertical);
					g->FillRectangle(&shineBrush, shine);
				}
				// If the button is not pushed down?
				if(!m_bMouseDown){
					if(!IconOnly){
						// Calculate the alpha level for the glow color
						Gdiplus::ARGB argbGlowStart, argbGlowEnd;
						argbGlowStart  = argbGlowEnd = (m_argbGlow & ~Gdiplus::Color::AlphaMask);
						argbGlowStart |= ((ARGB)(BYTE)(((m_argbGlow &  Gdiplus::Color::AlphaMask) >> 
							Gdiplus::Color::AlphaShift) * m_fGlowLevel) << Gdiplus::Color::AlphaShift);
					
						// Paint the glow starting from bottom center
						Gdiplus::PathGradientBrush pgb(innerPath);
						Gdiplus::Color colorGlowEnd(argbGlowEnd);
						int colorCount = 1;
						pgb.SetCenterColor(Color(argbGlowStart));
						pgb.SetSurroundColors(&colorGlowEnd, &colorCount);
						pgb.SetCenterPoint(PointF(rectf.Width * 0.50f, rectf.Height * 1.00f));
						g->FillPath(&pgb, innerPath);
					}
				}

				if(!IconOnly){
				// Get the buttons text length
					int nTextLen = GetWindowTextLength(m_hWnd);
					if(nTextLen > 0){
					// Get the buttons text
					LPWSTR lpwsText = //new WCHAR[++nTextLen];
						(wchar_t*)malloc(sizeof(wchar_t*) * nTextLen + 1);
					if(lpwsText){
						GetWindowTextW(m_hWnd, lpwsText, nTextLen+1);

						// Position the text in the center of the button
						StringFormat strFormat;
						strFormat.SetAlignment(StringAlignmentCenter);
						strFormat.SetLineAlignment(StringAlignmentCenter);
						
						// Setup the style and draw the text
						SolidBrush textColor(Color::White);
						Font buttonFont(L"Georgia", 12, FontStyleRegular, UnitPixel);
						g->DrawString(lpwsText, -1, &buttonFont, rectf, &strFormat, &textColor);

						// Free the button text
					//	delete lpwsText;
						free(lpwsText);
						}
					}
				}

				// Reset the clipping area and delete the clip path
				g->ResetClip();
				if(!IconOnly)delete innerPath;

				// Update our cached bitmap
				if(m_pCachePaint) delete m_pCachePaint;
				m_pCachePaint = new CachedBitmap(bmp, &graphics);
				
				// Free resources and reset the state
				delete bmp;
				delete g;
				m_bCacheDirty = FALSE;
			}

			// Draw the final cached image to the screen
			graphics.DrawCachedBitmap(m_pCachePaint, 0, 0);

			return 0;
		}

		LRESULT OnMouseButton(HWND m_hWnd, BOOL bDown, INT nX, INT nY){
			// Has the button push state changed?
			if(m_bMouseDown != bDown){
				//MessageBoxA(0,"button push state changed","",0);
				// Was the mouse released over the button?
				if(m_bMouseDown && m_bMouseHover){
					//MessageBoxA(0,"on button press","",0);
					PostMessage(GetParent(m_hWnd), WM_COMMAND,
						MAKELONG(GetWindowLong(m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_hWnd);
				}
				// Set new state and redraw
				m_bMouseDown  = bDown;
				Redraw(m_hWnd, TRUE);
			}

			return 0;
		}

		LRESULT OnKey(HWND m_hWnd, BOOL bDown, INT nVirtKey, INT nRepeat){
			// Forward the message on so that pressing space-bar will click the button
			if(nVirtKey == VK_SPACE)
				return OnMouseButton(m_hWnd, bDown, 0, 0);

			return 0;
		}

		LRESULT OnMouseMove(HWND m_hWnd, INT nKey, INT nX, INT nY){
			// Track the mouse leaving our window
			if(!m_bMouseTrack){
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_hWnd, 1 };
				m_bMouseTrack = TrackMouseEvent(&tme);
			}
			// Update the hover
			if(!m_bMouseHover){
				// Reset the down state if needed
				if(nKey == MK_LBUTTON)
					m_bMouseDown = TRUE;
				m_bMouseHover = TRUE;
				if(!IconOnly)Redraw(m_hWnd, TRUE);
			}

			return 0;
		}

		LRESULT OnMouseLeave(HWND m_hWnd){
			m_bMouseHover = FALSE;
			m_bMouseTrack = FALSE;
			m_bMouseDown  = FALSE;
			if(!IconOnly)Redraw(m_hWnd, TRUE);
			
			// Update glow timer
			if(!IconOnly){
				if(!m_uGlowTimer)m_uGlowTimer = SetTimer(m_hWnd, 1, 10, NULL);
			}
			return 0;
		}

		LRESULT OnMouseHover(HWND m_hWnd)
		{
			m_bMouseHover = TRUE;
			//MessageBoxA(0,"mouse hovers over the window","",0);
			if(!IconOnly)Redraw(m_hWnd, TRUE);
			
			// Update glow timer
			if(!IconOnly){
				if(!m_uGlowTimer)m_uGlowTimer = SetTimer(m_hWnd, 1, 10, NULL);
			}
			return 0;
		}

		LRESULT OnFocus(HWND m_hWnd, BOOL bHasFocus, HWND hWndLastFocus)
		{
			if(!bHasFocus)
				m_bMouseDown = FALSE;
			m_bMouseHover = bHasFocus;
			if(!IconOnly)Redraw(m_hWnd, FALSE);

			// Update glow timer
			if(!IconOnly){
				if(!m_uGlowTimer)m_uGlowTimer = SetTimer(m_hWnd, 1, 10, NULL);
			}		
			return 1;
		}

		LRESULT OnTimer(HWND m_hWnd, UINT uTimerID)
		{
			// Did we receive this timer message as a result of glow change?
			if(uTimerID == 1){
				// Increment the glow if mouse is over our button
				if(m_bMouseHover){
					m_fGlowLevel += 0.033f;
					if(m_fGlowLevel >= 1.0f){
						// When we reach the upper limit cancel the timer
						m_fGlowLevel = 1.0f;
						m_uGlowTimer = 0;
						KillTimer(m_hWnd, 1);
					}
					if(!IconOnly)Redraw(m_hWnd, FALSE);

				// Decrement the glow if the mouse is not over the button
				}else{
					m_fGlowLevel -= 0.066f;
					if(m_fGlowLevel <= 0.0f){
						// When we reach the bottom limit cancel the timer
						m_fGlowLevel = 0.0f;
						m_uGlowTimer = 0;
						KillTimer(m_hWnd, 1);
					}
					if(!IconOnly)Redraw(m_hWnd, FALSE);
				}
			}

			return 0;
		}


LRESULT OnInternalPaint(HWND m_hWnd, HDC hdc){
			LRESULT lResult;
			PAINTSTRUCT ps;
			RECT m_rcClient;
			GetClientRect(m_hWnd, &m_rcClient);
			ZeroMemory(&ps, sizeof(ps));
			if(!hdc)
				BeginPaint(m_hWnd, &ps);
			else{
				CopyRect(&ps.rcPaint, &m_rcClient);
				ps.hdc = hdc;
			}
			lResult = OnPaint(&ps, m_hWnd);
			if(!hdc)
				EndPaint(m_hWnd, &ps);

			return lResult;
}
//////////////////////////////////////////////////////////////////////
	static LRESULT CALLBACK DGButton::s_WndProc(__in HWND hwnd, 
												__in UINT uMsg, 
												__in WPARAM wParam, 
												__in LPARAM lParam)
	{
		DGButton *self;
		if (uMsg == WM_NCCREATE) {
		//	LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		//	self = reinterpret_cast<DGButton *>(lpcs->lpCreateParams);
			self = (DGButton *)lpcs->lpCreateParams;
			//  self->m_hwnd = hwnd;
//			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(self));
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LPARAM)self);
		} else {
//			self = reinterpret_cast<DGButton *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			self = (DGButton *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (self) {
			return self->GlassButtonProc(hwnd, uMsg, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

//////////////////////////////////////////////////////////////////////
		LRESULT CALLBACK GlassButtonProc(__in HWND hWnd, 
										 __in UINT uMsg, 
										 __in WPARAM wParam, 
										 __in LPARAM lParam)
		{

			switch (uMsg)  {

				case WM_ACTIVATE:
				break;

			case WM_CREATE:
			{
				SendMessage(hWnd, WM_SIZE, 0, 0);
				Redraw(hWnd, TRUE);
			}
			break;

			case WM_DESTROY:
			{
				OnDestroy();
			}
			break;

			case WM_SIZE:
			{
				OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));
			}
			break;

			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				int lPos = 0, // position from left glass button border
					tPos = 0; // position from top glass button border
				OnInternalPaint(hWnd, hdc);
				if(isIconic == TRUE){
						if(ICON_PLACEMENT == ICON_POSITION_RIGHT){
							lPos = (Width()-(IconPos.nWidth)-(IconPos.nWidth / 4));
							tPos = ((Height() - (IconPos.nHeight))/2);
						} else if(ICON_PLACEMENT == ICON_POSITION_LEFT){ 
							lPos = (IconPos.nWidth / 4);
							tPos = ((Height() - (IconPos.nHeight))/2);
						} else if(ICON_PLACEMENT == ICON_POSITION_CENTER){
							lPos = (Width() / 2) - (IconPos.nWidth / 2);
							tPos = ((Height() - (IconPos.nHeight))/2);
						} else if(ICON_PLACEMENT == ICON_POSITION_LEFT_UP){
							lPos = (IconPos.nWidth / 8);
							tPos = 5;
						} else if(ICON_PLACEMENT == ICON_POSITION_LEFT_BT){
							lPos = (IconPos.nWidth / 8);
							tPos = (Height() - IconPos.nHeight - 5);
						} else if(ICON_PLACEMENT == ICON_POSITION_RIGHT_UP){
							lPos = (Width()-(IconPos.nWidth)-(IconPos.nWidth / 8));
							tPos = 5;
						} else if(ICON_PLACEMENT == ICON_POSITION_RIGHT_BT){
							lPos = (Width()-(IconPos.nWidth)-(IconPos.nWidth / 8));
							tPos = (Height() - IconPos.nHeight - 5);
						} else {
							lPos = IconPos.x;
							tPos = IconPos.y;
						}
						if(!DrawIconEx(hdc, 
							lPos, 
							tPos, 
							(HICON)LoadImageW((HINSTANCE)GetWindowLongW(
							GetAncestor(m_hWnd, GA_ROOT), GWL_HINSTANCE), 
							MAKEINTRESOURCEW(IconPos.IconID), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR), 
							IconPos.nWidth, IconPos.nHeight, 0, NULL, DI_NORMAL)){
								OutputDebugStringW(L"failed to draw icon!");
						}
				}
				EndPaint(hWnd, &ps);
			}
			break;

			case WM_LBUTTONDOWN:
			{
				OnMouseButton(hWnd, TRUE,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			break;

			case WM_LBUTTONUP:
			{
				OnMouseButton(hWnd, FALSE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			break;

			case WM_MOUSEMOVE:
			{
				OnMouseMove(hWnd, (INT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			break;

			case WM_MOUSELEAVE:
			{
				OnMouseLeave(hWnd);
			}
			break;

			case WM_MOUSEHOVER:
			{
				OnMouseHover(hWnd);
			}
			break;

			case WM_SETFOCUS:
			{
				OnFocus(hWnd, TRUE, (HWND)wParam);
			}
			break;

			case WM_KILLFOCUS:
			{
				OnFocus(hWnd, FALSE, (HWND)wParam);
			}
			break;

			case WM_TIMER:
			{
				if(!IconOnly)OnTimer(hWnd, (UINT)wParam);
			}
			break;

			case WM_KEYDOWN:
			{
				OnKey(hWnd, TRUE,  (INT)wParam, (INT)LOWORD(lParam));
			}
			break;

			case WM_KEYUP:
			{
				OnKey(hWnd, FALSE,  (INT)wParam, (INT)LOWORD(lParam));
			}
			break;

	    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	public:
		operator HWND(){ return m_hWnd; }

		DGButton(BOOL IconOnlyButton){
			m_pCachePaint = NULL;
			m_bCacheDirty = TRUE;
			m_bMouseTrack = m_bMouseHover = m_bMouseDown  = FALSE;
			m_fGlowLevel  = 0;
			m_uGlowTimer  = 0;
			m_argbGlow    = 0xB28DBDFF;
			isIconic	  = FALSE;
			IconOnly = IconOnlyButton;
			/*green color: 0xB200CD66,
			orange : 0xB2FF9912,
			red: 0xB2FF0000*/
		}
		~DGButton(){}

		static const long GlassButton = 0x0000000a;
		static const long IconiButton = 0x0000000b;

		int PaintBtnControl(__in long CtrlStyle, // button custom procedure
									   __in HWND hParent,	// parent window
									   __in wchar_t *Text,	// text to print on button
									   __in int x,			// from left side
									   __in int y,			// from top border
									   __in int nWidth,
									   __in int nHeight,	
									   __in int iControl)
		{
			return InitCustButton(CtrlStyle, hParent, 
						Text, x, y, nWidth, nHeight, iControl);
		}

		ARGB SetGlowColor(ARGB argbColor)
		{
			ARGB argbOld = m_argbGlow;
			m_argbGlow = argbColor;
			return argbOld;
		}

		BOOL SetBtnText(wchar_t *LText){
			BOOL Status;
			Status = SetWindowTextW(m_hWnd, LText);
			if(!Status) return FALSE;
			Status = PostMessageW(m_hWnd, WM_MOUSEMOVE, 0, 0);
			return Status;
		}

		/*
			Last 4 params are optional.
			If there is a default icon (32px) then there is no need to 
			specify an icon size.
		*/
		int SetBtnIcon(__in int IconID, 
								 __in int IconPositionID,
								 __in_opt int x = 0, 
								 __in_opt int y = 0, 
								 __in_opt int nWidth = 32, 
								 __in_opt int nHeight = 32)
		{
			ICON_PLACEMENT = IconPositionID;	// ICON_POSITION_CUSTOM
			if(ICON_PLACEMENT == ICON_POSITION_CUSTOM){
				if(x == 0 || y == 0)return -2;	// coordinates should be provided
			}
			IconPos.x = x;
			IconPos.y = y;
			IconPos.nWidth = nWidth;
			IconPos.nHeight = nHeight;
			IconPos.IconID = IconID;
			isIconic = TRUE;
			return Redraw(m_hWnd, TRUE);
		}

		int RemoveBtnIcon(){
			isIconic = FALSE;
			return Redraw(m_hWnd, TRUE);
		}

		int SetPosition(__in int x, __in int y){
			RECT rcWind;
			if(x == 0 && y == 0) return -1;
			GetWindowRect(m_hWnd, &rcWind);
			OffsetRect(&rcWind, -rcWind.left, -rcWind.top); 
			return MoveWindow(m_hWnd, x, y, rcWind.right, rcWind.bottom, TRUE);
		}

		int SetSize(__in int nWidth, __in int nHeight){
			RECT rcClient, rcWind;
			POINT ptDiff;
			GetClientRect(m_hWnd, &rcClient);
			GetWindowRect(m_hWnd, &rcWind);
			ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
			ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
			return SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 
				nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
		}

	int Width(){
		RECT rcWind;
		if(m_hWnd == NULL) return -1;
		GetWindowRect(m_hWnd, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.right;
	}

	int Height(){
		RECT rcWind;
		if(m_hWnd == NULL) return -1;
		GetWindowRect(m_hWnd, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.bottom;
	}

};

#endif 