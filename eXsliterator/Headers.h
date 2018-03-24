#include <Winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <olectl.h>
#include <ole2.h>
#include <gdiplus.h>
#include <stdio.h>
#include <iostream>
#include <regex>
#include <Commctrl.h>
#include <psapi.h>
#include <string.h>
#include <Wininet.h>
#include <Shlwapi.h>
#include <comutil.h>
#include "SQLite/sqlite3.h"
char *global_net_buffer = NULL;
#define WIN_API

#ifdef C_API
#define malloc(s) malloc(s)
#define free(m) free(m)
#endif

#ifdef WIN_API
#define malloc(s) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, s)
#define free(m) HeapFree(GetProcessHeap(), 0, m)
#endif

#ifdef NT_API
#define malloc(s) RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, s)
#define free(m) RtlFreeHeap(GetProcessHeap(), 0, m)
#endif 

//#define MONITOR
#define GLOBAL_TRIAL_TIME		60
/***********************************************************/
#define TIMER_DEF				6611
int TimerTime_shit = GLOBAL_TRIAL_TIME;
#define CHECK_FOR_RUNS			2323
#define CHECK_FOR_ACTIVATION	2424
/***********************************************************/
#define EXTERNAL_COMMAND	(WM_USER + 0x0019)
#define MAGIC_BUTTON		77777
#define MDIALOGCLASS L"MachinizedDialogImplementation"
HWND MainWindowHandle, DictWindowHandle;
using namespace Gdiplus;
int InsertWindow(HWND hWnd, int CtrlID, bool Transliterated, wchar_t *WindowName);
DWORD ProcMonitorThread(LPVOID lParam);
DWORD __stdcall GetAddr(char *Function, wchar_t *Module);
HKL SetCheckRussian();
int VerifyFilePresence(wchar_t *RelativePath);
inline bool HideThread(HANDLE hThread);

char * RawAnsiGetRequest(__in wchar_t *Host,
									   __in wchar_t *Port,
									   __in wchar_t *Path,
									   __in wchar_t *UserAgent,
									   __in wchar_t *Timeout);
char * SecureGetRequest(__in wchar_t *Host,
									  __in wchar_t *Port,
									  __in wchar_t *Path,
									  __in wchar_t *UserAgent,
									  __in wchar_t *ConnectionTimeout);


wchar_t *dict_db();
#define d(x)	OutputDebugStringW(x)
#define da(x)	OutputDebugStringA(x)
int DBInsertTranslation(sqlite3 *db,wchar_t *Word,wchar_t *Translation,wchar_t *Type);
__w64 long OnInternalPaint(__in HWND m_hWnd, 
									 __in HDC hdc, 
									 __in RECT m_rcClient,
									 __in_opt BOOL isDlg = FALSE);

int RunDialog(__in HWND hParent, 
						__in wchar_t *WindowName,
						__in BOOL isParented,
						__in WNDPROC wndProc,
						__in __w64 long Parameter);

BOOL KillDialog(__in wchar_t *WindowName);
BOOL CALLBACK EnumWindowsProcEx(HWND hWnd, long lParam);

#include "resource.h"
#include "NtAPI.h"
#include "Lang.h"
#include "Keys.h"
#include "Struct.h"
//#include "Protection.h"
#include "System.h"
#include "Timing.h"

#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "DGButton.h"
#include "DataGridControl.h"
#include "ComboControl.h"
#include "EntryControl.h"
#include "FrameControl.h"
#include "RadioControl.h"
#include "Tooltip.h"
#include "GuiEffects.h"
#include "TrayControl.h"
#include "ContextMenu.h"
#include "DebugLog.h"
#include "ResImage.h"
#include "iniControl.h"

int GlobalTransliterationsNumber;
HANDLE MonitorAllWindowsThreadHandle;
HANDLE MainMutex, DictMutex, DictThreadMutex;
RECT m_rcClient, m_rcDialog;

GuiEffects *GuiEffectsEx;
DataGridControl *DataGrid[2];
DGButton *DarkGlasButton[6];
FrameControl *Frame[1];
ComboControl *TranslationDir[1];
RadioControl *TranslitRadio[1];
EntryControl *WordEntry[1];
DebugLog *Deb;
TrayControl *Tray;
ContextMenu *CMenu[3];
ResImage *m_Bkg = NULL;
Timing *finalcunt;

sqlite3 *MemDatabase, *PhyDataBase;
#define DICT_DB		L"dict_db.db"
#define MUTEXRUN	L"exsliterator_is_running"
#define MUTEXDICT	L"exsliterator_dictionary_is_running"
#define MUTEXDTH	L"translation_thread_is_running"

EntryControl *Entry[2];

#include "Network.h"
#include "SqlMem.h"
#include "SqlPhy.h"
#include "Dictionary.h"
#include "Settings.h"
#include "Globals.h"
#include "Umtr.h"
