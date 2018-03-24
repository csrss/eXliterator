
class Timing{

protected:
	__int64 FutureTime;
	__int64 LastTime;

private:
	typedef struct _SYSTEM_TIME_OF_DAY_INFORMATION {
		LARGE_INTEGER BootTime;
		LARGE_INTEGER CurrentTime;
		LARGE_INTEGER TimeZoneBias;
		ULONG CurrentTimeZoneId;
	} SYSTEM_TIME_OF_DAY_INFORMATION, *PSYSTEM_TIME_OF_DAY_INFORMATION;
	
	BOOL Hmmm(void)
	{
		__int64 TimeWhenBoot, CurrentTime;
		int ToWait = Opt.GlobalTrialTime;
		const static __int64 Second = 10000000;
		SYSTEM_TIME_OF_DAY_INFORMATION SysTimeInfo;
	
		if(!NtQuerySystemInformation) return FALSE;

		NtQuerySystemInformation(3,&SysTimeInfo,sizeof(SysTimeInfo),0);
	
		TimeWhenBoot = SysTimeInfo.BootTime.QuadPart;
		CurrentTime = SysTimeInfo.CurrentTime.QuadPart;

		if(FutureTime == 0)FutureTime = CurrentTime + (Second * ToWait);

		if(CurrentTime <= TimeWhenBoot){	// something is def. wrong
			return TRUE;
		} else {
			if(CurrentTime < FutureTime){
				LastTime = CurrentTime;
				return FALSE;
			} else if(CurrentTime == LastTime) {
				return TRUE;
			} else {
				return TRUE;
			}
		}
		return TRUE;
	}
	
public:
	Timing(){
		FutureTime = 0;
	}
	~Timing(){
		FutureTime = 0;
	}

	BOOL lolcheng(){
		return Hmmm();
	}

};
