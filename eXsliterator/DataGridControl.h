#ifndef _DATAGRIDCONTROL_H
#define _DATAGRIDCONTROL_H

class DataGridControl {
protected:
	HWND hDataGridControl;
	HWND pArent;
	HIMAGELIST hImageList;

public:
	DWORD dwStyle;

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

	int CreateDataGrid(__in HWND hParent, 
								 __in int x, 
								 __in int y, 
								 __in int nWidth, 
								 __in int nHeight, 
								 __in int CtrlID,
								 __in BOOL FontItalic,
								 __in BOOL FontUnderline,
								 __in int FontSize)
	{
		if(!hDataGridControl)
			hDataGridControl = CreateWindowExW(WS_EX_LEFTSCROLLBAR, L"SysListView32", L"", 
											dwStyle,
											x, y, nWidth, nHeight, hParent, (HMENU)CtrlID, 
											(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
											NULL);
		else return -1;	// already here
		if(!hDataGridControl) return GetLastError();
		LOGFONT lfArial = {
				-MulDiv(FontSize, GetDeviceCaps(GetDC(hParent), LOGPIXELSY), 72), 0, 0, 0,
				FW_MEDIUM, FontItalic, FontUnderline, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Georgia"
			};
		m_hfArial = CreateFontIndirectW(&lfArial);
		hfOld = (HFONT)SelectObject(GetDC(hParent), (void*)(HFONT)m_hfArial);
		SendMessageW(hDataGridControl, WM_SETFONT, (WPARAM)m_hfArial, MAKELPARAM(1, 0));
		return TRUE;
	}

public:
	operator HWND(){ return hDataGridControl; }
	DataGridControl(BOOL noHeader){
		hDataGridControl = NULL;
		hImageList = NULL;
		dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | LVS_REPORT | LVS_NOLABELWRAP | LVS_ALIGNLEFT | WS_VSCROLL;
		if(noHeader) dwStyle |= LVS_NOCOLUMNHEADER;
	}
	~DataGridControl(){
		hDataGridControl = NULL;
		hImageList = NULL;
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
		return CreateDataGrid(hParent, 
							  x, 
						      y, 
						      nWidth, 
						      nHeight, 
						      CtrlID, 
						      FontItalic, 
						      FontUnderline, 
						      FontSize);
	}

	__w64 long SetFullRowSelection(__in_opt HWND hControl = NULL){
		if(!hDataGridControl && !hControl) return -1;	// no grid
		return SendMessageW(hControl ? hControl : hDataGridControl, 
							LVM_SETEXTENDEDLISTVIEWSTYLE, 
							0, LVS_EX_FULLROWSELECT);
	}

	__w64 long AddColumn(__in int nWidth,
								   __in wchar_t * Name,
								   __in int Index,
								   __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no grid
		LVCOLUMNW lvC;
		RtlSecureZeroMemory(&lvC, sizeof(LVCOLUMN));
		lvC.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvC.iSubItem	= 0;
		lvC.cx			= nWidth;
		lvC.pszText		= Name;
		lvC.fmt			= LVCFMT_LEFT;
		return SendMessageW(hControl ? hControl : hDataGridControl,
							LVM_INSERTCOLUMN,
							(WPARAM)(int)Index,
							(LPARAM)(const LV_COLUMN *)(&lvC));
	}

	__w64 long SetBkgImage(__in wchar_t * ImagePath,
									 __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no datagrid!!!
		LVBKIMAGEW IBBkImg = {0};
		IBBkImg.ulFlags = LVBKIF_SOURCE_URL | LVBKIF_STYLE_TILE;
		IBBkImg.pszImage = ImagePath;
		return SendMessageW (hControl ? hControl : hDataGridControl, 
							 LVM_SETBKIMAGE,
							 0,
							 (LPARAM)(LPLVBKIMAGE)&IBBkImg);
	}

	int InitImageList(void){
		if(!hImageList) hImageList = ImageList_Create(32,32,ILC_COLOR32,2,10);
		else return -1;	// already created
		if(!hImageList) return GetLastError();	// bug creating image list
		return TRUE;	// created successfuly
	}

	int AddIconToImageList(__in int IconID){
		if(!hImageList) return -1;	// image list is not initialized
		int iNdex = ImageList_ReplaceIcon(hImageList, -1, 
									LoadIcon(GetModuleHandle(NULL), 
									MAKEINTRESOURCE(IconID)));
		return iNdex;
	
	}

	__w64 long SetImageList(){
		if(!hImageList) return -1;	// image list is not initialized
		if(!hDataGridControl) return -2;	// no datagrid?
		return SendMessageW(hDataGridControl, 
							LVM_SETIMAGELIST, 
							(WPARAM)LVSIL_SMALL,
							(LPARAM)(HIMAGELIST)hImageList);
	}

	int InsertRootItem(__in wchar_t * Text, 
								 __in int ImageIndex,
								 __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no datagrid?
		LVITEMW lvI = {0};
		lvI.mask		= LVIF_TEXT | LVIF_IMAGE;
		lvI.iSubItem	= 0;
		if(ImageIndex != -1)
			lvI.iImage		= ImageIndex;
		lvI.pszText		= Text;
		int iNdex = SendMessageW(hControl ? hControl : hDataGridControl,
								LVM_INSERTITEM,
								0,
								(LPARAM)(const LV_ITEM *)(&lvI));
		return iNdex;
	}

	__w64 long InsertChildItem(__in wchar_t * Text,
										 __in int RootItemIndex,
										 __in int ChildIndex,
										 __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no grid
		LV_ITEM Lvi;
		Lvi.iSubItem = ChildIndex;
		Lvi.pszText = Text;
		return SendMessageW(hControl ? hControl : hDataGridControl, 
							LVM_SETITEMTEXT, 
							(WPARAM)RootItemIndex,
							(LPARAM)(LV_ITEM *)&Lvi);
	}

	BOOL ClearDataGrid(__in_opt HWND hControl = NULL){
		if(!hDataGridControl && !hControl) return -1;	// no grid
		int iTems = (int)SendMessageW(hControl ? hControl : hDataGridControl, 
									  LVM_GETITEMCOUNT, 
									  0, 
									  0L);
		while(iTems >= 0){
			SendMessageW(hControl ? hControl : hDataGridControl, 
						 LVM_DELETEITEM, 
						 (WPARAM)(int)iTems, 
						 0L);
			iTems--;
		}
		return TRUE;
	}

	int GetItemsNr(__in_opt HWND hControl = NULL){
		if(!hDataGridControl && !hControl) return -1;	// no grid
		int iTems = (int)SendMessageW(hControl ? hControl : hDataGridControl, 
									  LVM_GETITEMCOUNT, 
									  0, 
									  0L);
		return iTems;
	}

	int GetSelectedIndex(__in_opt HWND hControl = NULL){
		if(!hDataGridControl && !hControl) return -1;	// no grid
		return (int)SendMessageW(hControl ? hControl : hDataGridControl,
								LVM_GETNEXTITEM,
								(WPARAM)(int)-1,
								MAKELPARAM((LVNI_SELECTED), 0));
	}

	BOOL DeleteItem(__in int Index,
							  __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no grid
		return (BOOL)SendMessageW(hControl ? hControl : hDataGridControl, 
								  LVM_DELETEITEM, 
								  (WPARAM)(int)Index,
								  0L);
	}

	__w64 long UpdateItem(__in wchar_t * Text, 
									__in int ImageID, 
									__in int ItemIndex,
									__in int iSubIndex,
									__in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;	// no grid
		LVITEM lvI;
		lvI.mask		= LVIF_TEXT | LVIF_IMAGE;
		lvI.iSubItem	= iSubIndex;	// if 0 then root item gets updated
		if(ImageID != -1)
			lvI.iImage		= ImageID;
		lvI.pszText		= Text;
		lvI.iItem		= ItemIndex;
		return SendMessageW(hControl ? hControl : hDataGridControl, 
							LVM_SETITEM, 
							0, 
							(LPARAM)(&lvI));
	}

	wchar_t * GetSelectedItemText(__in int iSubIndex,
											__in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return NULL;	// no grid
//		static wchar_t *output;
		wchar_t item[10240] = {0};
		int iPos = GetSelectedIndex(hControl);
		if(iPos == -1) return NULL;
		LVITEM lvi = {0};
		lvi.cchTextMax = sizeof(item);
		lvi.iSubItem = iSubIndex;
		lvi.pszText = item;
		int iTextLen = SendMessageW(hControl ? hControl : hDataGridControl, 
									LVM_GETITEMTEXT, 
									(WPARAM)iPos, 
									(LPARAM)&lvi);
		if(iTextLen > sizeof(item)) return NULL;
		return lvi.pszText;
	}

	wchar_t * GetItemTextByIndex(__in int iSubIndex,
										   __in int iNdex,
										   __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return NULL;	// no grid
//		static wchar_t *output;
		wchar_t item[10240] = {0};
		int iPos = iNdex;
		LVITEM lvi = {0};
		lvi.cchTextMax = sizeof(item);
		lvi.iSubItem = iSubIndex;
		lvi.pszText = item;	
		int iTextLen = SendMessageW(hControl ? hControl : hDataGridControl, 
									LVM_GETITEMTEXT, 
									(WPARAM)iPos, 
									(LPARAM)&lvi);
		if(iTextLen > sizeof(item)) return NULL;
		return lvi.pszText;
	}

	/*if iSubItem == -1, search whole grid*/
	int GetIndexByItemText(__in wchar_t * SubItemText,
									 __in int iSubItem,
									 __in BOOL isPartial,
									 __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;
		int iColumns, iLoops, iTempItems;
		int iTemsCount = (int)SendMessageW(hControl ? hControl : hDataGridControl,
										   LVM_GETITEMCOUNT,
										   0,
										   0L);
		if(iTemsCount <= 0) return -1;
		if(iSubItem == -1){
			HWND hHeader = (HWND)SendMessageW(hControl ? hControl : hDataGridControl,
											  LVM_GETHEADER,
											  0,
											  0L);
			if(!hHeader) return -1;
			iColumns = (int)SendMessageW(hHeader, 
										 HDM_GETITEMCOUNT, 
										 0, 
										 0L);
			iLoops = iColumns;
			iColumns--;
		} else {
			iColumns = iSubItem;
			iLoops = 1;
		}
		iTempItems = iTemsCount;
		LVITEM lvi;
		int iNdex = -1;
		wchar_t item[10240];
		while(iLoops > 0){
			while(iTemsCount >= 0){
				RtlSecureZeroMemory(&lvi, sizeof(lvi));
				RtlSecureZeroMemory(&item, sizeof(item));
				lvi.cchTextMax = sizeof(item);
				lvi.iSubItem = iColumns;
				lvi.pszText = item;
				SendMessageW(hControl ? hControl : hDataGridControl, 
							 LVM_GETITEMTEXT, 
							 (WPARAM)iTemsCount, 
							 (LPARAM)&lvi);
				if(wcscmp(lvi.pszText, SubItemText) == 0){
					iNdex = iTemsCount;
					break;
				}
				if(isPartial){
					if(wcsstr(lvi.pszText, SubItemText)){
						iNdex = iTemsCount;
						break;
					}
				}
				iTemsCount--;
			}
			iTemsCount = iTempItems;
			iColumns--;
			iLoops--;
		}
		return iNdex;
	}

	BOOL ShowHide(__in BOOL Mode, 
							__in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;
		return ShowWindow(hControl ? hControl : hDataGridControl, Mode ? SW_SHOW : SW_HIDE);
	}

	int Width(__in_opt HWND hControl = NULL){
		RECT rcWind;
		if(!hDataGridControl && !hControl) return -1;
		GetWindowRect(hControl ? hControl : hDataGridControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.right;
	}

	int Height(__in_opt HWND hControl = NULL){
		RECT rcWind;
		if(!hDataGridControl && !hControl) return -1;
		GetWindowRect(hControl ? hControl : hDataGridControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return rcWind.bottom;
	}

	int Reload(__in_opt HWND hControl = NULL){
		RECT rcWind;
		if(!hDataGridControl && !hControl) return -1;
		GetWindowRect(hControl ? hControl : hDataGridControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		InvalidateRect(hControl ? hControl : hDataGridControl, &rcWind, TRUE);
	}

	int SetPosition(__in int x, 
							  __in int y,
							  __in_opt HWND hControl = NULL)
	{
		RECT rcWind;
		if((!hDataGridControl && !hControl) || (x == 0 && y == 0)) return -1;
		GetWindowRect(hControl ? hControl : hDataGridControl, &rcWind);
		OffsetRect(&rcWind, -rcWind.left, -rcWind.top);
		return MoveWindow(hControl ? hControl : hDataGridControl, x, y, rcWind.right, rcWind.bottom, TRUE);
	}

	int SetSize(__in int nWidth, 
						  __in int nHeight,
						  __in_opt HWND hControl = NULL)
	{
		RECT rcClient, rcWind;
		POINT ptDiff;
		if((!hDataGridControl && !hControl) || (nWidth == 0 && nHeight == 0)) return -1;
		GetClientRect(hControl ? hControl : hDataGridControl, &rcClient);
		GetWindowRect(hControl ? hControl : hDataGridControl, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		return SetWindowPos(hControl ? hControl : hDataGridControl, HWND_TOP, 0, 0, 
			nWidth + ptDiff.x, nHeight + ptDiff.y, SWP_NOMOVE);
	}

	/*if there is a ItemIndex param, thats freekin' easy, 
	else, we find item index by text*/
	__w64 long ScrollToItem(__in_opt int ItemIndex,
									  __in_opt int ColumnIndex,
									  __in_opt wchar_t * ItemText,
									  __in_opt BOOL isTextPartial,
									  __in_opt HWND hControl = NULL)
	{
		if(!hDataGridControl && !hControl) return -1;
		int iNdex;
		if((ItemText < 0) && (ItemText == NULL || wcslen(ItemText) == 0))
			return -1;	// what to search for?
		if(ItemIndex >= 0){
			return SendMessageW(hControl ? hControl : hDataGridControl, 
								LVM_ENSUREVISIBLE, 
								(WPARAM)(int)ItemIndex, 
								(LPARAM)(BOOL)FALSE);
		} else {
			iNdex = GetIndexByItemText(ItemText, ColumnIndex, isTextPartial, hControl);
			return SendMessageW(hControl ? hControl : hDataGridControl, 
								LVM_ENSUREVISIBLE, 
								(WPARAM)(int)iNdex, 
								(LPARAM)(BOOL)FALSE);
		}
	}

};

#endif 