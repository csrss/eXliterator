#ifndef _CONTEXTMENU_H
#define _CONTEXTMENU_H

class ContextMenu {
protected:
	HMENU hContextMenu;
private:

public:
	operator HMENU(){ return hContextMenu; }
	ContextMenu(){
		hContextMenu = NULL;
	}
	~ContextMenu(){
		hContextMenu = NULL;
	}

	int Create(void){
		if(!hContextMenu) hContextMenu = CreatePopupMenu();
		if(!hContextMenu) return GetLastError();
		return 0;
	}

	int Destroy(void){
		BOOL Status = DestroyMenu(hContextMenu);
		hContextMenu = NULL;
		if(!Status) return GetLastError();
		return Status;
	}

	int AddItem(__in int ControlID, 
						  __in wchar_t *Text)
	{
		BOOL Status;
		Status = AppendMenuW(hContextMenu, MF_STRING, ControlID, Text);
		if(!Status) return GetLastError();
		return Status;
	}

	BOOL AddSeparator(void){
		return AppendMenuW(hContextMenu, MF_SEPARATOR, 0, L"-");
	}

	unsigned int TrackMenuHit(__in HWND hParent){
		POINT curPoint;
		GetCursorPos(&curPoint);
		SetForegroundWindow(hParent); 
		return TrackPopupMenu(hContextMenu,
							  TPM_RETURNCMD | TPM_NONOTIFY,
							  curPoint.x,
							  curPoint.y,
							  0,
							  hParent,
							  NULL);
	}

	unsigned int TrackMenuHit2(__in HWND hParent, 
										 __in HWND hControl)
	{
		POINT curPoint;
		GetCursorPos(&curPoint);
		SetForegroundWindow(hParent);
		BOOL bAnim = TRUE;
		int rc = SystemParametersInfoW(0x1003/*SPI_SETMENUANIMATION*/, 0, (PVOID)bAnim, 0);
		RECT rectangle;
		GetWindowRect(hControl, &rectangle);
		TPMPARAMS params = { sizeof(TPMPARAMS) };
		params.rcExclude = rectangle;
		return TrackPopupMenu(hContextMenu,
							  TPM_RETURNCMD | 
							  /*TPM_NONOTIFY |*/ 
							  TPM_VERPOSANIMATION,
							  rectangle.left+2, rectangle.bottom-2,
							  0,
							  hParent,
							  NULL);
	}

	BOOL SetItemBitmap(__in unsigned int MenuID,
								 __in int BitmapID, 
								 __in HWND hRoot)
	{
		if(!hContextMenu || !hRoot || !BitmapID || !MenuID) return FALSE;
		MENUITEMINFO mii;
		mii.cbSize = sizeof mii;
		mii.fMask = MIIM_BITMAP;
		HINSTANCE hInst = (HINSTANCE)GetWindowLong(hRoot, GWL_HINSTANCE);
		HBITMAP Bmp = LoadBitmapW(hInst, MAKEINTRESOURCE(BitmapID));
		mii.hbmpItem = Bmp;
		return SetMenuItemInfo(hContextMenu, MenuID, FALSE, &mii);
	}

};

#endif