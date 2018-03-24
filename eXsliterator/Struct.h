#ifndef _STRUCT_H
#define _STRUCT_H

#define EN_PL		0x00000010
#define EN_RU		0x00000011
#define PL_EN		0x00000012
#define PL_RU		0x00000013
#define RU_EN		0x00000014
#define RU_PL		0x00000015

#define cEN_PL		L"EN_PL"
#define cEN_RU		L"EN_RU"
#define cPL_EN		L"PL_EN"
#define cPL_RU		L"PL_RU"
#define cRU_EN		L"RU_EN"
#define cRU_PL		L"RU_PL"

typedef struct _MAIN_STRUCT {
	int VistaGlass;
	int UserLanguage;
	int DebugLogToFile;
	int AutoStart;
	int GlobalTranslit;
	int LocalTranslationSearch;
	int OnlineTranslationSearch;
	int TranslitIsON;
	int GlowText;
	int ButtonsIcons;
	int Tooltips;
	int ColHeaders;
	long DictType;
	int Kewl;
	wchar_t *TopGridBackground;
	wchar_t *BottomGridBaclground;
	int wVersion;
	int SerialIsA;
	int SerialErr;
	int ConnectionTimeOut;
	int TempTranslit;
	int AutoUpdates;
	int Blocked;
	char *MachineAddr;
	int GlobalTrialTime;
}MAIN_STRUCT, *PMAIN_STRUCT;
MAIN_STRUCT Opt;

int ResetStruct(){
	Opt.UserLanguage = 0;
	Opt.DebugLogToFile= 0;
	Opt.AutoStart = 0;
	Opt.GlobalTranslit = 0;
	Opt.LocalTranslationSearch = 0;
	Opt.OnlineTranslationSearch = 0;
	Opt.TranslitIsON = 0;
	Opt.GlowText = 0;
	Opt.ButtonsIcons = 0;
	Opt.Tooltips = 0;
	Opt.ColHeaders = 0;
	Opt.DictType = 0x00000000;
	Opt.Kewl = 0;
	Opt.TopGridBackground = NULL;
	Opt.BottomGridBaclground = NULL;
	Opt.wVersion = 0;
	Opt.SerialIsA = 0;
	Opt.SerialErr = 0;
	Opt.ConnectionTimeOut = 20;
	Opt.TempTranslit = 0;
	Opt.AutoUpdates = 0;
	Opt.Blocked = 0;
	Opt.MachineAddr = NULL;
	Opt.GlobalTrialTime = 60;
	return 0;
}

#endif