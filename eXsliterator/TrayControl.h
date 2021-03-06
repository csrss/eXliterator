#ifndef _TRAYCONTROL_H
#define _TRAYCONTROL_H

#define ID_TRAY_APP_ICON                5000
#define WM_TRAYICON						(WM_USER + 1)

class TrayControl {
protected:
	unsigned int WM_TASKBARCREATED;
	NOTIFYICONDATA g_notifyIconData;
	HMENU g_menu;
	int CurrentIconID;
	HWND hWindow;

private:

	BOOL Restore(HWND hWnd){
		Shell_NotifyIconW(NIM_DELETE, &g_notifyIconData);
		return ShowWindow(hWnd, SW_SHOW);
	}

	BOOL Minimize(HWND hWnd){
		Shell_NotifyIconW(NIM_ADD, &g_notifyIconData);
		return ShowWindow(hWnd, SW_HIDE);
	}

	void InitNotifyIconData(HWND hWnd, int IconID){
		HINSTANCE hMainIn;
		RtlSecureZeroMemory( &g_notifyIconData, sizeof( NOTIFYICONDATA ) ) ;
		hMainIn = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
		g_notifyIconData.hWnd = hWnd;
		g_notifyIconData.uID = ID_TRAY_APP_ICON;
		g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
		g_notifyIconData.uCallbackMessage = WM_TRAYICON;
		g_notifyIconData.hIcon = (HICON)LoadImage( hMainIn, MAKEINTRESOURCE(IconID), IMAGE_ICON, 0, 0, 0  ) ;
		wcscpy(g_notifyIconData.szTip, L"eXsliteratoЯ™");
	}

public:
	operator HMENU(){ return g_menu; }
	operator HWND(){ return hWindow; }
	operator int(){ return CurrentIconID; }
	TrayControl(HWND hWnd){
		WM_TASKBARCREATED = 0;
		hWindow = hWnd;
	}
	~TrayControl(){
		hWindow = NULL;
		RemoveIcon();
		CurrentIconID = 0;
	}

	int InitTray(__in int IconID, 
						   __in wchar_t *CustomMessageName)
	{
		WM_TASKBARCREATED = RegisterWindowMessageW(CustomMessageName);
		if(WM_TASKBARCREATED == 0){
			return GetLastError();
		}
		InitNotifyIconData(hWindow, IconID);
		if(!IsWindowVisible(hWindow)){
			Shell_NotifyIconW(NIM_DELETE, &g_notifyIconData);
		}
		g_menu = CreatePopupMenu();
		if(!g_menu) return GetLastError();
		CurrentIconID = IconID;
		return 0;
	}

	BOOL SwitchIcon(__in int newIconID){
		InitNotifyIconData(hWindow, newIconID);
		CurrentIconID = newIconID;
		return Shell_NotifyIconW(NIM_MODIFY, &g_notifyIconData);
	}

	BOOL RemoveIcon(void){
		InitNotifyIconData(hWindow, CurrentIconID);
		return Shell_NotifyIconW(NIM_DELETE, &g_notifyIconData);
	}

	BOOL MinimizeWindow(void){
		return Minimize(hWindow);
	}

	BOOL RestoreWindow(void){
		return Restore(hWindow);
	}

	BOOL AddMenuItem(__in int ControlID, 
							   __in wchar_t *Text)
	{
		return AppendMenuW(g_menu, MF_STRING, ControlID, Text);
	}

	DWORD MenuItemCheck(__in int ControlID, 
								  __in BOOL Mode)
	{
		return CheckMenuItem(g_menu, ControlID, Mode ? MF_CHECKED : MF_UNCHECKED);
	}

	BOOL MenuItemEnable(__in int ControlID, 
								  __in BOOL Mode)
	{
		return EnableMenuItem(g_menu, ControlID, Mode ? MF_ENABLED : MF_DISABLED);
	}

	BOOL AddSeparator(void){
		return AppendMenuW(g_menu, MF_SEPARATOR, 0, L"-");
	}

	unsigned int TrackMenuHit(void){
		POINT curPoint;
		GetCursorPos(&curPoint);
		SetForegroundWindow(hWindow); 
		return TrackPopupMenu(g_menu,
							  TPM_RETURNCMD | TPM_NONOTIFY,
							  curPoint.x,
							  curPoint.y,
							  0,
							  hWindow,
							  NULL);
	}
};

#endif
