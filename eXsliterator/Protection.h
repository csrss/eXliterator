#ifndef _PROTECTION_H
#define _PROTECTION_H
#pragma warning(disable: 4409)

class ProtectionClass{
protected:
	BOOL DebugStatus;
	BOOL VMWareStatus;
	NTSTATUS Initialization;
private:

	int NtGlobalFlagDebugCheck(){
		__asm {
			mov eax, fs:[30h]
			mov al, [eax+68h] 
			and al, 70h
			cmp al, 70h
			je being_debugged
			jmp okey
	
		being_debugged:
			xor eax, eax
			mov eax, 1
	
		okey:
			xor eax, eax
		    mov eax, 0
		}
	}

	int TestVMWare(){
		int flag = 0;
		__try {
			__asm {
				push edx;
				push ecx;
				push ebx;
				mov eax, 'VMXh';
				mov ebx, 0; // This can be any value except MAGIC
				mov ecx, 10; // "CODE" to get the VMware Version
				mov edx, 'VX'; // Port Number
				in eax, dx; // Read port
				//On return EAX returns the VERSION
				cmp ebx, 'VMXh'; // is it VMware
				setz [flag]; // set return value
				pop ebx;
				pop ecx;
				pop edx;
				}
			}
		__except(EXCEPTION_EXECUTE_HANDLER) {
			flag = 0;
		}
		if (flag != 0) return 1;
		else return 0;
	}
 
	int VMStrReg(){
		unsigned char mem[4] = {0, 0, 0, 0};
		__asm str mem;
		if ((mem[0] == 0x00) && (mem[1] == 0x40)) return 1;
		else return 0;
	}

	int VMLdtReg(){
		unsigned char ldt_info[6];
		int ldt_flag = 0;
		__asm {
			sldt ldt_info;
		}
		if ((ldt_info[0] != 0x00) && (ldt_info[1] != 0x00))ldt_flag = 1;
		if (ldt_flag == 1) return 1;
		if (ldt_flag == 0) return 0;
	}

	inline bool Int2DCheck(){
	__try{
		__asm{
			int 0x2d
			xor eax, eax
			add eax, 2
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		return 0;
	}
	return 1;
	}

	int IsDebLame(){
		if (IsDebuggerPresent()) {
			return 1;
		} else {
			return 0;
		}
	}

	HWND LameIsDeb(){
		HWND Hnd = FindWindowW(L"OLLYDBG", 0);
		if (Hnd == NULL) {
			return NULL;
		} else {
			return Hnd;
		}
	}

	int QueryDebug(){
		NTSTATUS status;
		unsigned long debugFlag;
		status = NtQueryInformationProcess (NtCurrentProcess(), 31, &debugFlag, 4, NULL); 
		if (debugFlag == 0x00000000) return 1;
		else if (debugFlag == 0x00000001) return 0;
		return -1;
	}

	int DebugQuery(){
		NTSTATUS status;
		unsigned long retVal;
		status = NtQueryInformationProcess (NtCurrentProcess(), 0x07, &retVal, 4, NULL);
		if (retVal != 0) return 1;
		else return 0;
	}

	int DebugNoDebug(){
		HANDLE hDebugObject;
		NTSTATUS status;
		status = NtQueryInformationProcess (NtCurrentProcess(), 0x1e, &hDebugObject, 4, NULL);
		if (hDebugObject) return 1;
		else return 0;
	}

	int DebugYesDebug(){
		HANDLE hnd = GetCurrentThread();
		CONTEXT ctx;
		NTSTATUS status;
		status = GetThreadContext(hnd, &ctx);
		if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) ||
			(ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || 
			(ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
			return 1;
		else return 0;
	}

	int VmDetectEx () {
		unsigned char m[2+4], rpill[] = "\x0f\x01\x0d\x00\x00\x00\x00\xc3";
		*((unsigned*)&rpill[3]) = (unsigned)m;
		((void(*)())&rpill)();
		if(m[5]>0xd0)return 1; 
		else return 0;
	}

	int CheckFilterState(){
		if(NtSetDebugFilterState(0,0,TRUE) == STATUS_SUCCESS) return 1;
		return 0;
	}

	BOOL VirtualCheck(void){
		if(TestVMWare() == 1){	// if TestVMWare succeeds we return TRUE
			return TRUE;
		} else {	// TestVMWare returned 0, we are checking further...
			if(VMStrReg() == 1){	// if VMStrReg returns 1, we know now - we are trapped
				return TRUE;	// return that we are really trapped
			} else {	// seems like another check failure, so we checking a bit deeper
				if(VMLdtReg() == 1){	// checking once again against vmware, we are in vmware
					return TRUE;
				} else {	// looks like we are not in vmware so far, but we need to do more checks
					if(VmDetectEx() == 1){	// last check for vmware
						return TRUE;	// we are in a deep shit, return true
					}	// VmDetectEx returned 0
				}	// VMLdtReg returned 0
			}	// VMStrReg returned 0
		}	// TestVMWare returned 0
		// If we got so far, that means we are not in vmware
		return FALSE;
	}

	BOOL DebugCheck(void){
		if(IsDebLame() == 1){	// found debugger with the lamest way
			return TRUE;	// return true
		} else {	// looks like a hacker is a bit smarter
			if(LameIsDeb() != NULL){	// found OLLY, return 
				return TRUE;
			} else {	// nothing found, going further
				if(QueryDebug() == 1){	// fucker found
					return TRUE;
				} else {	// still nothing, proceeding
					if(DebugQuery() == 1){	// gotcha
						return TRUE;
					} else {	// maybe there is no debugger? 
						if(DebugNoDebug() == 1){	// smart ass
							return TRUE;
						} else {	// seems like there is no debugger really
							if(DebugYesDebug() == 1){	// sucker located!
								return TRUE;
							} else {	// or there is not or a hacker is a really smart ass
								if(NtGlobalFlagDebugCheck() == 1){	// bitch ass muthafucka
									return TRUE;
								} else {	// damn it
									if(CheckFilterState() == 1){	// got you biatch
										return TRUE;
									} else {	// fuck!!!
//										if(DebugPrefix() == 1){	// oh man...
//											return TRUE;
//										} else {	// the fuck?
//											return FALSE;
											if(Int2DCheck() == 1){	// get the fuck out!!!
												return TRUE;
											} else {	// there is no debugger for a fucks sake
												return FALSE;
//											}
										} // DebugPrefix = 0
									} // CheckFilterState = 0
								} // NtGlobalFlagDebugCheck = 0
							} // DebugYesDebug = 0
						} // DebugNoDebug = 0
					} // DebugQuery = 0
				} // QueryDebug = 0
			} // LameIsDeb = 0
		} // IsDebLame = 0
		return TRUE;
	}

public:
	operator BOOL(){ return DebugStatus; }
	operator BOOLEAN(){ return VMWareStatus; }
	operator NTSTATUS(){ return Initialization; }

	ProtectionClass(){
//		DebugStatus = 0;
//		VMWareStatus = 0;
//		DebugStatus = DebugCheck();
//		VMWareStatus = VirtualCheck();
		Initialization = 0xdeadbeef;
	}
	~ProtectionClass(){}

	BOOL VirtCheck(void){
		return VirtualCheck();
	}

	BOOL DebCheck(void){
		return DebugCheck();
	}
};

#endif