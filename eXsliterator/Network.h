#ifndef _NETWORK_H
#define _NETWORK_H
#include <Windns.h>
//Type field of Query and Answer
#define T_A		    1		/* host address */
#define T_NS		2		/* authoritative server */
#define T_CNAME		5		/* canonical name */
#define T_SOA		6		/* start of authority zone */
#define T_PTR		12		/* domain name pointer */
#define T_MX		15		/* mail routing information */
/*
#define ansi_alloc(size)			(char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char)*size+1)
#define ansi_free(addr)				(char*)HeapFree(GetProcessHeap(), 0, val)
#define ansi_realloc(addr, size)	(char*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (LPVOID)addr, sizeof(char)*size+1)
#define uni_alloc(size)				(wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t*)*size+1);
#define uni_free(addr)				(wchar_t*)HeapFree(GetProcessHeap(), 0, addr);
*/ 
struct DNS_HEADER_EX {
	unsigned	short id;		    // identification number
	
	unsigned	char rd     :1;		// recursion desired
	unsigned	char tc     :1;		// truncated message
	unsigned	char aa     :1;		// authoritive answer
	unsigned	char opcode :4;	    // purpose of message
	unsigned	char qr     :1;		// query/response flag
	
	unsigned	char rcode  :4;	    // response code
	unsigned	char cd     :1;	    // checking disabled
	unsigned	char ad     :1;	    // authenticated data
	unsigned	char z      :1;		// its z! reserved
	unsigned	char ra     :1;		// recursion available
	
	unsigned    short q_count;	    // number of question entries
	unsigned	short ans_count;	// number of answer entries
	unsigned	short auth_count;	// number of authority entries
	unsigned	short add_count;	// number of resource entries
};


//Constant sized fields of query structure
struct QUESTION {
	unsigned short qtype;
	unsigned short qclass;
};


//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct  R_DATA {
	unsigned short type;
	unsigned short _class;
	unsigned int   ttl;
	unsigned short data_len;
};
#pragma pack(pop)


//Pointers to resource record contents
struct RES_RECORD {
	std::string name;
	struct R_DATA* resource;
	std::string rdata;
	int pref;
};

//Structure of a Query
typedef struct {
	std::string name;
	struct QUESTION* ques;
} QUERY;

class Network {
protected:
	LPVOID hInternet;
	HANDLE hNetworkThread;
	char *WebResult;
public:

	typedef struct _WEBTHREADPARAMS {
		wchar_t *Hostname;
		wchar_t *Scriptname;
		wchar_t *PostData;
	}WEBTHREADPARAMS, *PWEBTHREADPARAMS;
	WEBTHREADPARAMS WebParms;
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
	char *RequestData;
	char *SERVER;
	int HTTP_PORT;
	std::string dns_servers[10];
	int TIMEOUT;

	char * Unicode_To_Ansi(const wchar_t * s){
		if (s==NULL) return NULL;
		int cw = lstrlenW(s);
		if (cw==0) {
			CHAR *psz = new CHAR[1];
			*psz='\0';
			return psz;
		}
		int cc = WideCharToMultiByte(CP_UTF8,0,s,cw,NULL,0,NULL,NULL);
		if (cc==0) return NULL;
		CHAR *psz = new CHAR[cc+1];
		cc = WideCharToMultiByte(CP_UTF8,0,s,cw,psz,cc,NULL,NULL);
		if (cc == 0) {
			delete[] psz;
			return NULL;
		}
		psz[cc]	=	'\0';
		return psz;
	}

	wchar_t * Html_Ansi_To_Unicode(__in char * pstr){
		LPWSTR pwstr;
		DWORD dwWritten;
		int wcsChars;
		if(!pstr)return NULL;
		if((int)strlen(pstr) <= 0) return NULL;
		wcsChars = (int)strlen(pstr);
		pwstr = (wchar_t *)malloc(wcsChars * sizeof (wchar_t*) -1);
		dwWritten = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, 
			pstr, wcsChars, pwstr, (wcsChars * sizeof (wchar_t*) -1));
		return pwstr;
	}

	wchar_t * Ansi_To_Unicode(__in char * pstr){
		LPWSTR pwstr;
		DWORD dwWritten;
		int wcsChars;
		if(!pstr)return NULL;
		if((int)strlen(pstr) <= 0) return NULL;
		wcsChars = (int)strlen(pstr);
		pwstr = (wchar_t *)malloc(wcsChars * sizeof (wchar_t*) + 1);
		dwWritten = MultiByteToWideChar(CP_UTF8, 0, pstr, -1, pwstr, 
			(wcsChars * sizeof (wchar_t*) + 1));
		return pwstr;
	}

	void ToDnsNameFormat(std::string& outDns) {
		//convert www.google.com to 3www6google3com
		int lock=0 , i;
		char dnsLen;
		std::string host(outDns);

		outDns.clear();
		host.append(".");
		for(i=0;i<static_cast<int>(host.length());i++) {
			if(host[i]=='.') {
				dnsLen = i-lock;
				outDns.push_back(dnsLen);
				for(;lock<i;lock++)
					outDns.push_back(host[lock]);
				lock++;
			}
		}
	}

	void FromDnsNameFormat(std::string& outHost) {
	//convert 3www6google3com0 to www.google.com
		char p;
		int i, j;
		std::string dns(outHost);
	
		outHost.clear();
		for(i=0;i<static_cast<int>(dns.length());i++)	{
			p=dns[i];
			for(j=0;j<(int)p;j++)
				outHost.push_back(dns[++i]);
			if (i<static_cast<int>(dns.length())-1)
				outHost.push_back('.');
		}
	}

	std::string ReadName(unsigned char* reader,unsigned char* buffer,int* count) {
		std::string name;
		unsigned int p=0,jumped=0,offset;
		*count = 1;
		//read the names in 3www6google3com format
		while(*reader!=0) {
			if(*reader>=192) {
				offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000  ;)
				reader = buffer + offset - 1;
				jumped = 1;  //we have jumped to another location so counting wont go up!
			} else 
				name.push_back(*reader);
			reader=reader+1;
			if(jumped==0) *count = *count + 1; //if we havent jumped to another location then we can count up
		}
		if(jumped==1) *count = *count + 1;  //number of steps we actually moved forward in the packet
		//now convert 3www6google3com0 to www.google.com
		FromDnsNameFormat(name);
		return name;		
	}
 
	char* ngethostbyname(unsigned char *host, char *dns_server) {
		if(strcmp((char*)host, "localhost") == 0 || 
			strcmp((char*)host, "127.0.0.1") == 0) return "127.0.0.1";
		unsigned char buf[65536],*reader, *qname;
		int i, j, stop;
		unsigned long ioctl_opt = 1;
		int tries;
		SOCKET s;
		struct sockaddr_in a;
		struct RES_RECORD answers[20],auth[20],addit[20];  //the replies from the DNS server
		struct sockaddr_in dest;
		struct DNS_HEADER_EX *dns = NULL;
		struct QUESTION   *qinfo = NULL;
		s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  //UDP packet for DNS queries
		dest.sin_family=AF_INET;
		dest.sin_port=htons(53);
		dest.sin_addr.s_addr=inet_addr(dns_server);  //dns servers
		dns = (struct DNS_HEADER_EX *)&buf;
		dns->id = htons(static_cast<u_short>(GetCurrentProcessId()));
		dns->qr = 0;      //This is a query
		dns->opcode = 0;  //This is a standard query
		dns->aa = 0;      //Not Authoritative
		dns->tc = 0;      //This message is not truncated
		dns->rd = 1;      //Recursion Desired
		dns->ra = 0;      //Recursion not available! hey we dont have it (lol)
		dns->z  = 0;
		dns->ad = 0;
		dns->cd = 0;
		dns->rcode = 0;
		dns->q_count = htons(1);   //we have only 1 question
		dns->ans_count  = 0;
		dns->auth_count = 0;
		dns->add_count  = 0;
		//point to the query portion
		qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER_EX)];
		std::string dnsName((char *) host);
		ToDnsNameFormat(dnsName);
		for (i=0; i< static_cast<int>(dnsName.length()); i++)*(qname+i)=dnsName[i];
		*(qname+i) = 0;
		qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER_EX) + dnsName.length() + 1]; //fill it
		qinfo->qtype = htons(T_A); //Get MX Record
		qinfo->qclass = htons(1); //its internet (lol)
	
		if (ioctlsocket(s, FIONBIO, &ioctl_opt) == SOCKET_ERROR) {
			OutputDebugStringW(L"ioctlsocket ERROR!\r\n");
			return NULL;
		}
	
		if(sendto(s,(char*)buf,sizeof(struct DNS_HEADER_EX) + dnsName.length() +1 + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest))==SOCKET_ERROR) {
			OutputDebugStringW(L"sendto error!\r\n");
			return NULL;
		}
		i=sizeof(dest);
		for (tries=0; ; tries ++) {
			if(recvfrom (s,(char*)buf,65536,0,(struct sockaddr*)&dest,&i)==SOCKET_ERROR) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					return NULL;
				} else
					Sleep(100);
			} else
				break;
			if (tries >= 100) {
				return NULL;
			}
		}
	
		dns=(struct DNS_HEADER_EX*)buf;
		reader=&buf[sizeof(struct DNS_HEADER_EX) + dnsName.length() + 1 + sizeof(struct QUESTION)];
	
		stop=0;
		for(i=0;i<ntohs(dns->ans_count);i++) {
			answers[i].name=ReadName(reader,buf,&stop);
			reader = reader + stop;
			answers[i].resource = (struct R_DATA*)(reader);
			reader = reader + sizeof(struct R_DATA);
			switch (ntohs(answers[i].resource->type)) {
			case 1: //if its an ipv4 address
				answers[i].rdata.clear();
				for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
					answers[i].rdata.push_back(reader[j]);
	
				reader = reader + ntohs(answers[i].resource->data_len);
				break;
			case T_MX:
				answers[i].pref = reader[1] + 256 * reader[0];
				reader += 2;
				answers[i].rdata = ReadName(reader,buf,&stop);
			    reader = reader + stop;
				break;
			default:
				answers[i].rdata = ReadName(reader,buf,&stop);
			    reader = reader + stop;
			}
		}
		
		//read authorities
		for(i=0;i<ntohs(dns->auth_count);i++) {
			auth[i].name=ReadName(reader,buf,&stop);
			reader+=stop;
			auth[i].resource=(struct R_DATA*)(reader);
			reader+=sizeof(struct R_DATA);
			auth[i].rdata=ReadName(reader,buf,&stop);
			reader+=stop;
		}
	
		//read additional
		for(i=0;i<ntohs(dns->add_count);i++) {
			addit[i].name=ReadName(reader,buf,&stop);
			reader+=stop;
			addit[i].resource=(struct R_DATA*)(reader);
			reader+=sizeof(struct R_DATA);
			if(ntohs(addit[i].resource->type)==1) {
				addit[i].rdata.clear();
				for(j=0;j<ntohs(addit[i].resource->data_len);j++)
					addit[i].rdata.push_back(reader[j]);
				reader+=ntohs(addit[i].resource->data_len);
			
			} else {
				addit[i].rdata=ReadName(reader,buf,&stop);
			    reader+=stop;
			}
		}
	
		static char zzz[100] = {0};
		for(i=0;i<ntohs(dns->ans_count);i++) {
			switch (ntohs(answers[i].resource->type)) {
			case 1:  //IPv4 address
				long *p;
				p=(long*)answers[i].rdata.data();
				a.sin_addr.s_addr=(*p);    //working without ntohl
				strcpy(zzz, inet_ntoa(a.sin_addr));
			break;
			}
		}
	    return zzz;
	}

	int recvtimeout(int s, char *buf, int len, int timeout){
		fd_set fds;
		int n;
		struct timeval tv;
		FD_ZERO(&fds);
		FD_SET(s, &fds);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		n = select(s+1, &fds, NULL, NULL, &tv);
		if (n == 0) return -2; // timeout!
		if (n == -1) return -1; // error
		return recv(s, buf, len, 0);
	}

	int connecttimeout(char *host, int port, unsigned int TimeoutSec)
	{
		SOCKET client;
		char *destination;
		char extra_dns[] = { 0x38, 0x2e, 0x38, 0x2e, 0x38, 0x2e, 0x38, 0x00 };
		bool UseExtraDns = true;
		ULONG NonBlk = 1;
		int Ret;
		DWORD Err;
		char debug[MAX_PATH] = {0};

		destination = ngethostbyname((PUCHAR)host, extra_dns);
		if(destination == NULL || strcmp(destination, "") == 0 || strlen(destination) == 0){
			OutputDebugStringW(L"Unable to resolve host!\r\n");
			return -2;
		}

		client = socket(AF_INET, SOCK_STREAM, 0);
		if(client == INVALID_SOCKET){
			OutputDebugStringW(L"Socket error!\r\n");
			return -3;
		}

		if(ioctlsocket(client, FIONBIO, &NonBlk) != 0){
			OutputDebugStringW(L"Socket error @ ioctlsocket\r\n");
			return -4;
		}

		SOCKADDR_IN  sin;
		sin.sin_family				= AF_INET;
		sin.sin_port				= htons(port);
		sin.sin_addr.s_addr		= inet_addr(destination);

		Ret = WSAConnect(client, (const sockaddr*)&sin, sizeof(sin), NULL, NULL, NULL, NULL);
		if (Ret == SOCKET_ERROR){
            Err = WSAGetLastError();
            // Check if the error was WSAEWOULDBLOCK, where we'll wait.
            if (Err == WSAEWOULDBLOCK) {
                   OutputDebugStringW(L"Connect() returned WSAEWOULDBLOCK. Need to Wait..\r\n");
                   fd_set         Write, Err;
                   TIMEVAL      Timeout;
                   FD_ZERO(&Write);
                   FD_ZERO(&Err);
                   FD_SET(client, &Write);
                   FD_SET(client, &Err);
                   Timeout.tv_sec  = TimeoutSec;
                   Timeout.tv_usec = 0; // your timeout

                   Ret = select (0, NULL, &Write, &Err, &Timeout);
                   if (Ret == 0)
                           OutputDebugStringW(L"Connect Timeout.\r\n");
                   else {
						if (FD_ISSET(client, &Write)){
                               OutputDebugStringW(L"Connected...\r\n");
                               return client;
                        }
                          if (FD_ISSET(client, &Err)){
                                   OutputDebugStringW(L"Select() Error.\r\n");
                                   return -1;
                          }
                    }
			} else {
				RtlSecureZeroMemory(&debug, sizeof debug);
				sprintf(debug, "Connect Error!Returning error code: %d\r\n", WSAGetLastError());
				OutputDebugStringA(debug);
				return WSAGetLastError();
			}
		} else {
            OutputDebugStringW(L"Connected with no waiting.\r\n");
            return client;
		}
		return -1;
	}

	static std::string DeleteHtmlTags(std::string s){
		std::string result = s;
		unsigned int i, j;
	    for (i = 0, j = 0; (i < result.length()) && (j < result.length()); i++, j++){
			char c = result[i];
	        if (c == '<'){
				while ((c != '>')){ c = result[j]; j++; }
	            result = result.erase(i, j - i); 
	            i = -1; j = -1;
			}
		}
	return result;
	}

	int __stdcall DelTags(wchar_t *buf){
		wchar_t *newbuf = new wchar_t[ wcslen(buf) ];
		int len = wcslen(buf);
		int j = 0;

		// scan string
		for( int i=0; i<len ; i++ ){
		   // found an open '<', scan for its close
		   if ( buf[i] == '<' ){
		     // charge ahead in the string until it runs out or we find what we're looking for
		     for( ; i<len && buf[i] != '>'; i++ );
		   } else {
		      newbuf[j++] = buf[i];
		   }
		}

		newbuf[j] = 0;
		wcscpy( buf, newbuf );
		delete[] newbuf;
	return 0;
	}

	std::wstring& StringReplace(__in const std::wstring &SearchFor, 
								__in const std::wstring &ReplaceWith, 
								__in std::wstring &TextContainer)
	{
	std::wstring buff;
	int i, j;
    int sl = SearchFor.length();
    int strl = TextContainer.length();

    if (sl==0)return TextContainer;

    for(i=0, j=0; i<strl; j=0 ){
        while (i+j<strl && j<sl && TextContainer[i+j]==SearchFor[j])
            j++;
        if (j==sl){
            buff.append(ReplaceWith);
            i+=sl;
        } else  {
            buff.append( &TextContainer[i++], 1);
        }
    }
    TextContainer = buff;
    return TextContainer;
	}

	wchar_t *CutStringW(__in wchar_t *Buffer, 
						__in wchar_t *RemoveAllBeforeThis)
	{
	if ( !*RemoveAllBeforeThis ) return Buffer;
	for ( ; *Buffer; ++Buffer ){
		if ( *Buffer == *RemoveAllBeforeThis ){
			wchar_t *h, *n;
				for ( h = Buffer, n = RemoveAllBeforeThis; *h && *n; ++h, ++n ){
					if ( *h != *n )break;
				}
			if ( !*n ){
            return Buffer;
			}}}
	return NULL;
	}

	wchar_t * url_endecode(__in wchar_t *url, 
									 __in bool encode)
	{
	std::wstring Letters[] = {	L"а", L"б", L"в", L"г", L"д", L"е",
								L"ё", L"ж", L"з", L"и", L"й", L"к",
								L"л", L"м", L"н", L"о", L"п", L"р",
								L"с", L"т", L"у", L"ф", L"х", L"ц",
								L"ч", L"ш", L"щ", L"ъ", L"ы", L"ь",
								L"э", L"ю", L"я", L"`", L"!", L"@",
								L"#", L"$", /*L"%",*/  L"^", /*L"&",*/ L"*",
								L"(", L")", L"+", /*L"=",*/ L"[", L"]",
								L"{", L"}", L":", L";", L"'", L"\"",
								L"|", L"\\", L"<", L">", L"?", L"/"};

	std::wstring Hex[] = {	L"%D0%B0", L"%D0%B1", L"%D0%B2", L"%D0%B3", L"%D0%B4", L"%D0%B5",
							L"%D1%91", L"%D0%B6", L"%D0%B7", L"%D0%B8", L"%D0%B9", L"%D0%BA",
							L"%D0%BB", L"%D0%BC", L"%D0%BD", L"%D0%BE", L"%D0%BF", L"%D1%80",
							L"%D1%81", L"%D1%82", L"%D1%83", L"%D1%84", L"%D1%85", L"%D1%86",
							L"%D1%87", L"%D1%88", L"%D1%89", L"%D1%8A", L"%D1%8B", L"%D1%8C",
							L"%D1%8D", L"%D1%8E", L"%D1%8F", L"%60", L"%21", L"%40", L"%23",
							L"%24", /*L"%25",*/ L"%5E", /*L"%26",*/ L"%2A", L"%28", L"%29", L"%2B",
							/*L"%3D",*/ L"%5B", L"%5D", L"%7B", L"%7D", L"%3A", L"%3B", L"%27",
							L"%22", L"%7C", L"%5C", L"%3C", L"%3E", L"%3F", L"%2F"};
	int Rounds = 58;
	if(url == NULL || wcslen(url) == 0 || wcscmp(url, L"") == 0) return NULL;
	std::wstring WorkText = std::wstring(url);
	size_t found;
	/*Processing output*/

	while(Rounds >= 0){	// process each string in array
		found = WorkText.find(encode ? Letters[Rounds] : Hex[Rounds]);	// find code or char
		if (found != std::string::npos){	// if  found
			StringReplace(	encode ? Letters[Rounds] : Hex[Rounds], 
							encode ? Hex[Rounds] : Letters[Rounds], 
							WorkText);	// replace it
		} 
		Rounds--;	// dec number
	}	// silly simple

	/*DONE*/
	static wchar_t *output = (wchar_t*)malloc(sizeof(wchar_t*)*WorkText.length()+1);	// allocate unicode string
	wcscpy(output, WorkText.c_str());	// copy std string to unicode buffer
	return output;	// return it
	}

	/*
	Direction == true : decoding from hex to char
	Direction == false : encoding char with hex
	return : encoded or decoded string
	*/
	wchar_t * EnDecodeBuffer(__in wchar_t *buffer,
									   __in bool Direction)
	{
	// 64 strings in array = 0:63
	std::wstring RuRegular[] = {L"А", L"а", L"Б", L"б", L"В", L"в", L"Г", L"г",
								L"Д", L"д", L"Е", L"е", L"Ж", L"ж", L"З", L"з",
								L"И", L"и", L"Й", L"й", L"К", L"к", L"Л", L"л",
								L"М", L"м", L"Н", L"н", L"О", L"о", L"П", L"п",
								L"Р", L"р", L"С", L"с", L"Т", L"т", L"У", L"у",
								L"Ф", L"ф", L"Х", L"х", L"Ц", L"ц", L"Ч", L"ч",
								L"Ш", L"ш", L"Щ", L"щ", L"Ъ", L"ъ", L"Ы", L"ы",
								L"Ь", L"ь", L"Э", L"э", L"Ю", L"ю", L"Я", L"я",
								L"—", L"ё"};

	std::wstring RuHex[] = {L"&#x410;", L"&#x430;", L"&#x411;", L"&#x431;", 
							L"&#x412;", L"&#x432;", L"&#x413;", L"&#x433;",
							L"&#x414;", L"&#x434;", L"&#x415;", L"&#x435;", 
							L"&#x416;", L"&#x436;", L"&#x417;", L"&#x437;",
							L"&#x418;", L"&#x438;", L"&#x419;", L"&#x439;", 
							L"&#x41A;", L"&#x43A;", L"&#x41B;", L"&#x43B;",
							L"&#x41C;", L"&#x43C;", L"&#x41D;", L"&#x43D;", 
							L"&#x41E;", L"&#x43E;", L"&#x41F;", L"&#x43F;",
							L"&#x420;", L"&#x440;", L"&#x421;", L"&#x441;", 
							L"&#x422;", L"&#x442;", L"&#x423;", L"&#x443;",
							L"&#x424;", L"&#x444;", L"&#x425;", L"&#x445;", 
							L"&#x426;", L"&#x446;", L"&#x427;", L"&#x447;",
							L"&#x428;", L"&#x448;", L"&#x429;", L"&#x449;", 
							L"&#x42A;", L"&#x44A;", L"&#x42B;", L"&#x44B;",
							L"&#x42C;", L"&#x44C;", L"&#x42D;", L"&#x44D;", 
							L"&#x42E;", L"&#x44E;", L"&#x42F;", L"&#x44F;",
							L"&#x2014;", L"&#x451"};

	int Rounds = 65;
	if(buffer == NULL || wcslen(buffer) == 0 || wcscmp(buffer, L"") == 0) return NULL;
	std::wstring WorkText = std::wstring(buffer);
	size_t found;
	/*Processing output*/

	while(Rounds >= 0){	// process each string in array
		found = WorkText.find(Direction ? RuHex[Rounds] : RuRegular[Rounds]);	// find code or char
		if (found != std::string::npos){	// if  found
			StringReplace(	Direction ? RuHex[Rounds] : RuRegular[Rounds], 
							Direction ? RuRegular[Rounds] : RuHex[Rounds], 
							WorkText);	// replace it
		} 
		Rounds--;	// dec number
	}	// silly simple

	/*DONE*/
	static wchar_t *output = (wchar_t*)malloc(sizeof(wchar_t*) * WorkText.length() + 1);	// allocate unicode string
	wcscpy(output, WorkText.c_str());	// copy std string to unicode buffer
	return output;	// return it
	}

	
	unsigned int wtoi(wchar_t *n)
	{
        register unsigned int ret = 0;
        while ((((*n) < '0') || ((*n) > '9')) && (*n))
                n++;
        while ((*n) >= '0' && (*n) <= '9')
                ret = ret * 10 + (*n++) - '0';
        return ret;
	}

	char * SecureGetRequest(__in wchar_t *Host,
									  __in wchar_t *Port,
									  __in wchar_t *Path,
									  __in wchar_t *UserAgent,
									  __in wchar_t *ConnectionTimeout)
	{
		int Status, i, Return_Size = 0;
		WSADATA wsaData;
		SOCKET Socket;
		WORD wVersionRequested = MAKEWORD(2, 2);
		char *AnsiRequest;
		char recvbuffer[1024] = {0};

		wchar_t *Request = NULL;

		Status = WSAStartup(wVersionRequested, &wsaData);
		if(Status != 0){ 
		//	MessageBoxA(0,"WSAStartup error",NULL,0);
			return "WSA_ERROR";
		}
		Socket = connecttimeout(Unicode_To_Ansi(Host), wtoi(Port), wtoi(ConnectionTimeout));
		if(Socket == -1){ 
		//	MessageBoxA(0,"connecttimeout error",NULL,0);
			return "CONNECTION_ERROR";
		}

		int Request_len = sizeof(wchar_t*) * (wcslen(Host) + wcslen(Port) + 
											 wcslen(Path) + wcslen(UserAgent)) + 1024;

		Request = (wchar_t*)malloc(Request_len);
		wcscpy(Request, L"GET ");
		wcscat(Request, Path);
		wcscat(Request, L" HTTP/1.0");
		wcscat(Request, L"\r\nUser-Agent: ");
		wcscat(Request, UserAgent);
		wcscat(Request, L"\r\nHost: ");
		wcscat(Request, Host);
		wcscat(Request, L"\r\n\r\n");
		AnsiRequest = Unicode_To_Ansi(Request);
		free(Request);

		Status = send(Socket, AnsiRequest, (int)strlen(AnsiRequest), 0);
		if(Status == SOCKET_ERROR){
		//	MessageBoxA(0,"send error",NULL,0);
			return "SEND_ERROR";
		}
		char *PageBufferA = NULL;
		PageBufferA = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
									  (sizeof(char*)*(sizeof(recvbuffer))+1));

		RtlSecureZeroMemory(recvbuffer, sizeof recvbuffer);
		while(i = recvtimeout(Socket, recvbuffer, MAX_PATH, wtoi(ConnectionTimeout)) != 0){
			Return_Size += strlen(recvbuffer)*2;
			PageBufferA = (char*)HeapReAlloc(GetProcessHeap(), 
											 HEAP_ZERO_MEMORY, 
											 (LPVOID)PageBufferA, 
											 (sizeof(char)*(Return_Size)));
			strcat(PageBufferA, recvbuffer);
			RtlSecureZeroMemory(recvbuffer, sizeof recvbuffer);
		}
		closesocket(Socket);
		WSACleanup();
		static char *temp;
		for(i = 0; PageBufferA[i]!=0; ++i){
			if((PageBufferA[i]=='\r')&&(PageBufferA[i+1]=='\n')
			&&(PageBufferA[i+2]=='\r')&&(PageBufferA[i+3]=='\n')){
				temp = (char*)&PageBufferA[i] + 4;
				break;
			}
		}
		return temp;
	}

	char * EmitWebRequest(__in char *Server, 
									__in char *Path, 
									__in char *ReqType,
									__in char *UserAgent,
									__in char *http_version,
									__in char *formtype,
									__in char *data)
	{
		LPVOID hInternet;
		static char *RetnBuffer = NULL;
		BOOL IsGet = FALSE;
        DWORD dwRead;
		int formtype_len = 0, data_len = 0;
		char buffer[MAX_PATH*MAX_PATH];
		RtlSecureZeroMemory(buffer, sizeof buffer);
		int TotalSize = sizeof(buffer);
		static const char *accept[]={"*/*", NULL};
		if(strcmp(ReqType, "GET") == 0) {
			d(L"Get request processing\r\n");
			IsGet = TRUE;
		} else {
			d(L"Post request processing\r\n");
			formtype_len = (int)strlen(formtype);
			data_len = (int)strlen(data);
		}

		__asm {
//__first_block:
			push	0
			push	0
			push	0
			push	0
			push	UserAgent
			call	dword ptr InternetOpenA
			cmp		eax, 0
			je		__failed_1
// second block
			push	1
			push	0
			push	INTERNET_SERVICE_HTTP
			push	0
			push	0
			push	80
			push	Server
			push	eax
			call	dword ptr InternetConnectA
			cmp		eax, 0
			je		__failed_2
// 3rd block
			push	1
			push	INTERNET_FLAG_RELOAD
			push	accept
			push	0
			push	http_version
			push	Path
			push	ReqType
			push	eax
			call	dword ptr HttpOpenRequestA
			cmp		eax, 0
			je		__failed_3
// 4th block
			mov		hInternet, eax
			xor		eax, eax
			cmp		IsGet, 1
			je		__send_get_request
			jmp		__send_post_request

	__send_post_request:

			push	data_len
			push	data
			push	formtype_len
			push	formtype
			push	hInternet
			call	dword ptr HttpSendRequestA
			cmp		eax, 0
			je		__failed_post
			jmp		__succeed

	__send_get_request:

			push	0
			push	0
			push	0
			push	0
			push	hInternet
			call	dword ptr HttpSendRequestA
			cmp		eax, 0
			je		__failed_get
			jmp		__succeed

	__failed_1:
			xor		eax, eax
			jmp		__error_1

	__failed_2:
			xor		eax, eax
			jmp		__error_2

	__failed_3:
			xor		eax, eax
			jmp		__error_3

	__failed_post:
			xor		eax, eax
			jmp		__error_4

	__failed_get:
			xor		eax, eax
			jmp		__error_5

	__succeed:
		}

		int InitialSize = sizeof(char*) * sizeof(buffer) + 1;
		RetnBuffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, InitialSize);
		
		while(InternetReadFile(hInternet, buffer, sizeof(buffer), &dwRead)){
			OutputDebugStringW(L"Reading stream...\r\n");
			if ( dwRead == 0 ) break;
			buffer[dwRead] = 0;
			TotalSize += InitialSize;
			RetnBuffer = (char*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
				(void*)RetnBuffer, TotalSize);
			strcat(RetnBuffer, buffer);
		}

		RtlSecureZeroMemory(buffer, sizeof buffer);
		InternetCloseHandle(hInternet);
		if(RetnBuffer != NULL)
			return RetnBuffer;
		else 
			return "RetnBuffer_empty";

__error_1:
		if(hInternet)InternetCloseHandle(hInternet);
		MessageBoxA(0,"InternetOpenA_failed",NULL,0);
		return "InternetOpenA_failed";
__error_2:
		if(hInternet)InternetCloseHandle(hInternet);
		MessageBoxA(0,"InternetConnectA_failed",NULL,0);
		return "InternetConnectA_failed";
__error_3:
		if(hInternet)InternetCloseHandle(hInternet);
		MessageBoxA(0,"HttpOpenRequestA_failed",NULL,0);
		return "HttpOpenRequestA_failed";	// when site is down this happens
__error_4:
		if(hInternet)InternetCloseHandle(hInternet);
		MessageBoxA(0,"HttpOpenRequestA_failed",NULL,0);
		return "POST_failed";
__error_5:
		if(hInternet)InternetCloseHandle(hInternet);
		MessageBoxA(0,"HttpOpenRequestA_failed",NULL,0);
		return "POST_failed";
	}

	DWORD WebRequestEx(__in WEBTHREADPARAMS *lParam)
	{
		WebResult = NULL;
		if(lParam->PostData != NULL){
			wchar_t *urlEncoded = url_endecode((wchar_t*)lParam->PostData, true);
			char *AnsiEncoded = Unicode_To_Ansi(urlEncoded);
			char *AnsiHostname = Unicode_To_Ansi((wchar_t*)lParam->Hostname);
			char *AnsiScript = Unicode_To_Ansi((wchar_t*)lParam->Scriptname);
//			MessageBoxA(0,AnsiHostname,AnsiScript,0);
//			OutputDebugStringW(L"Emiting POST request\r\n");
			WebResult = EmitWebRequest(AnsiHostname, 
									   AnsiScript, 
									   "POST",
									   "Opera",
									   HTTP_VERSIONA,
									   "Content-Type:application/x-www-form-urlencoded",
									   AnsiEncoded);		
		} else {
	//		OutputDebugStringW(L"Emiting GET request\r\n");
			WebResult = EmitWebRequest(Unicode_To_Ansi((wchar_t*)lParam->Hostname), 
									   Unicode_To_Ansi((wchar_t*)lParam->Scriptname), 
									   "GET",
									   "Opera",
									   HTTP_VERSIONA,
									   NULL,
									   NULL);
		}
		return 0;
	}

	static DWORD StaticWebRequest(LPVOID lParam){
		Network * self = (Network *)lParam;
		self->WebRequestEx(&self->WebParms);
		return 0;
	}

public:
	operator char*(){ return WebResult; }
	Network(){
		SERVER = NULL;
		RequestData = NULL;
		hInternet = NULL;
		hNetworkThread = NULL;
		WebResult = NULL;
	}
	~Network(){
		SERVER = NULL;
		RequestData = NULL;
		if(hInternet)InternetCloseHandle(hInternet);
		hInternet = NULL;
		if(hNetworkThread) TerminateThread(hNetworkThread, 0);
		hNetworkThread = NULL;
		WebResult = NULL;
		WebParms.Hostname = NULL;
		WebParms.Scriptname = NULL;
		WebParms.PostData = NULL;
	}

	char * RawAnsiGetRequest(__in wchar_t *Host,
									   __in wchar_t *Port,
									   __in wchar_t *Path,
									   __in wchar_t *UserAgent,
									   __in wchar_t *Timeout)
	{
		return	SecureGetRequest(Host, Port, Path, UserAgent, Timeout);
	}

	int WebRequest(__in wchar_t *Server, 
							 __in wchar_t *Path,
							 __in wchar_t *PostData,
							 __in wchar_t *Timeout)
	{
		DWORD dwTimeout = wtoi(Timeout);
		WebParms.Hostname = Server;
		WebParms.Scriptname = Path;
		WebParms.PostData = PostData;
		hNetworkThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)StaticWebRequest, 
									  (void*)this, (DWORD)0, (DWORD*)0);
		if ( WaitForSingleObject ( hNetworkThread, dwTimeout * 1000) == WAIT_TIMEOUT ){
			if(hInternet)InternetCloseHandle(hInternet);
			if(hNetworkThread)TerminateThread(hNetworkThread, 0);
			WebResult = "CONNECTION_TIMEOUT";
			return -1;
		} 
		return 0;
	}

	char * UnicodeToAnsi(__in const wchar_t * s)
	{
		return Unicode_To_Ansi(s);
	}

	wchar_t * HtmlAnsiToUnicode(__in char * pstr)
	{
		return Html_Ansi_To_Unicode(pstr);
	}

	wchar_t * AnsiToUnicode(__in char * pstr)
	{
		return Ansi_To_Unicode(pstr);
	}

	wchar_t * CutString(__in wchar_t *Buffer, 
								  __in wchar_t *RemoveAllBeforeThis)
	{
		return CutStringW(Buffer, RemoveAllBeforeThis);
	}

	std::wstring& ReplaceStdString(__in const std::wstring &search, 
								   __in const std::wstring &replace, 
								   __in std::wstring &buffer)
	{
		return StringReplace(search, replace, buffer);
	}

	wchar_t * ReplaceWString(__in wchar_t *search,
									   __in wchar_t *replace,
									   __in wchar_t *buffer)
	{
		static wchar_t *Result = NULL;
		if(search == NULL || replace == NULL || buffer == NULL || wcslen(buffer) == 0){
			OutputDebugStringW(L"ReplaceWString: wrong values!\r\n");
			return NULL;
		}

		std::wstring stdresult = StringReplace(std::wstring(search), 
			std::wstring(replace), std::wstring(buffer));
		Result = (wchar_t *)malloc(sizeof(wchar_t*) * stdresult.length() + MAX_PATH);
		wcscpy(Result, stdresult.c_str());
		return Result;
	}

	int DelHTMLTags(__in wchar_t *buf)
	{
		return DelTags(buf);
	}

	wchar_t * EnDeBuffer(__in wchar_t *buffer,
								   __in bool Direction)
	{
		return EnDecodeBuffer(buffer, Direction);
	}

};

typedef struct tagNETPARAMS{
	wchar_t *Server;
	wchar_t *Path;
	wchar_t *PostData;
	wchar_t *Timeout;
	BOOL IsLing;
	wchar_t *Word;
	long TransType;
	wchar_t *TransCType;
	HWND optWindowHandle;
} NETPARAMS, *PNETPARAMS;

wchar_t * ParseLingResponse(__in char * Buffer){
	static wchar_t *TranslationBufferW = NULL;
	wchar_t *fPageBufferW = NULL;
	Network *Net = new Network();
	wchar_t *PageBufferW = Net->AnsiToUnicode(Buffer);
	TranslationBufferW = (LPWSTR)NtMalloc(0, 0, 
						(sizeof(wchar_t*)*(wcslen(PageBufferW)) + MAX_PATH), 0);
	if(wcsstr(PageBufferW, L"<A HREF='http://www.angielski.edu.pl'>angielski.EDU.pl</A>"))
		PageBufferW = Net->CutString(PageBufferW, L"<A HREF='http://www.angielski.edu.pl'>angielski.EDU.pl</A>");
	std::wstring StdPageBufferW = std::wstring(PageBufferW);
//	std::tr1::wcmatch res;
//	std::wstring pat = L"<div class='dictname'>(.*)<script type='text/javascript'>";
//	std::tr1::wregex pattern(pat, std::tr1::regex_constants::icase);
//	const std::tr1::wsregex_token_iterator end;
//	for (std::tr1::wsregex_token_iterator i(StdPageBufferW.begin(), StdPageBufferW.end(), pattern); i != end; ++i)
//	{
//		std::wstring porn = std::wstring(*i);
//		std::wstring Story = Net->ReplaceStdString(L"br", L"\n", porn);
		std::wstring PatternNext = L"<div class='dictdef'>(.*)</div>";
		std::tr1::wregex RegPattern(PatternNext, std::tr1::regex_constants::icase);
		const std::tr1::wsregex_token_iterator NextEnd;

		for (std::tr1::wsregex_token_iterator z(StdPageBufferW.begin(), 
												StdPageBufferW.end(), 
												RegPattern); z != NextEnd; ++z)
		{
			std::wstring BoomLay = std::wstring(*z);
			fPageBufferW = (wchar_t *)malloc(sizeof(wchar_t*) * BoomLay.length() + MAX_PATH);
			wcscpy(fPageBufferW, BoomLay.c_str());//	(wchar_t*)BoomLay.c_str();
			Net->DelHTMLTags(fPageBufferW);
			wcscat(TranslationBufferW, fPageBufferW);
			wcscat(TranslationBufferW, L". ");
			free(fPageBufferW);
		}

	delete Net;
	return TranslationBufferW;
}

wchar_t * GetRawResult(char *html){
	static wchar_t *RetnBuffer = NULL;
	int iLen = 0;
	if(!html) return NULL;
	typedef std::tr1::match_results<std::string::const_iterator> Results;
	std::tr1::regex re("<div id=\"result\">", std::tr1::regex::extended);
	std::string     str(html);
	Results         res;
	if (std::tr1::regex_search(str, res, re)) {
	} else {
		return NULL;
	}

	Network *Net = new Network();
	wchar_t *htmlW = Net->AnsiToUnicode(html);
	if(!htmlW) return NULL;	// <div id="result">

	if(wcsstr(htmlW, L"unavailable")){
		return L"unavailable";
	}

	wchar_t *partial = Net->CutString(htmlW, L"<div id=\"result\">");
//	MessageBoxW(0,partial,NULL,0);
	if(partial == NULL){
		Deb->dprint(false, L"CutString returned NULL\r\n");
		return NULL;
	}
	iLen = sizeof(wchar_t*) * wcslen(partial) + 1;
	RetnBuffer = (wchar_t*)malloc(iLen);
	std::wstring StdPageBufferW = std::wstring(partial);
	std::wstring pat = L"<span style(.*)</span>";
	std::tr1::match_results<std::wstring::const_iterator> result;
	std::tr1::wregex pattern(pat, std::tr1::regex_constants::icase);
	const std::tr1::wsregex_token_iterator end;
	for (std::tr1::wsregex_token_iterator i(StdPageBufferW.begin(), StdPageBufferW.end(), pattern); i != end; ++i)
	{
		std::wstring porn = std::wstring(*i);
		wcscat(RetnBuffer, porn.c_str());
	}
//	MessageBoxW(0,RetnBuffer,NULL,0);
	Net->DelHTMLTags(RetnBuffer);
//	MessageBoxW(0,RetnBuffer,NULL,0);
	std::wstring Almost = std::wstring(RetnBuffer);
	Net->ReplaceStdString(L"	", L" ", Almost);
//	MessageBoxW(0,Almost.c_str(),NULL,0);
	free(RetnBuffer);
	RetnBuffer = (wchar_t*)malloc(iLen);
	wcscpy(RetnBuffer, Almost.c_str());
	delete Net;
	return RetnBuffer;
}

int Request(__in void * lParam){
	PNETPARAMS pNetParams;
	bool Mode;
	Opt.DebugLogToFile ? Mode = true : Mode = false;
	char *RawData = NULL;
	wchar_t *Result = NULL;
	pNetParams = (PNETPARAMS)lParam;
	int connection_timeout = 0;
	wchar_t *Word = (wchar_t*)pNetParams->Word;
	wchar_t *TranslationType = (wchar_t*)pNetParams->TransCType;

	DictThreadMutex = CreateMutexW(NULL, FALSE, MUTEXDTH);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		MessageBoxW(0, TranslationThRun[Opt.UserLanguage], L"INFORMATION", MB_ICONINFORMATION);
		return -1;
	}
//	MessageBoxW(0,TranslationType,L"Translation type",0);

	Network *Net = new Network();
	Net->WebRequest((wchar_t*)pNetParams->Server,
					(wchar_t*)pNetParams->Path,
					(wchar_t*)pNetParams->PostData,
					(wchar_t*)pNetParams->Timeout);

	RawData = Net->operator char *();
	
//	MessageBoxA(0,RawData,NULL,0);
	if(RawData == NULL || strlen(RawData) == 0 || strcmp(RawData, "") == 0 ) {
		Deb->dprint(Mode, L"__Request: RawData == NULL\r\n");
		goto __failed;
	} else if(strcmp(RawData, "CONNECTION_TIMEOUT") == 0){
		connection_timeout = 1;
		Deb->dprint(Mode, L"__Request: connection timeout!\r\n");
		goto __failed;
	}
//	MessageBoxA(0,RawData,NULL,0);
	if((BOOL)pNetParams->IsLing){
		Result = ParseLingResponse(RawData);
		if(Result == NULL || wcslen(Result) == 0 || wcscmp(Result, L"") == 0) {
			Deb->dprint(Mode, L"__Request: ParseLingResponse == NULL\r\n");
			goto __failed;
		}
	} else {
		wchar_t *raw_result = GetRawResult(RawData);
	//	MessageBoxW(0,raw_result,NULL,0);
		if(raw_result == NULL || wcslen(raw_result) == 0 || wcscmp(raw_result, L"") == 0) {
			Deb->dprint(Mode, L"__Request: GetRawResult == NULL\r\n");
			goto __failed;
		}
		Result = Net->EnDeBuffer(raw_result, true);
	//	MessageBoxW(0,Result,NULL,0);
	}
//	MessageBoxW(0,Result,NULL,0);
	if(Result == NULL || wcslen(Result) == 0 || wcscmp(Result, L"") == 0){
__failed:
//		MessageBoxW(0,Result,NULL,0);
		int RootIndex = DataGrid[1]->InsertRootItem(Word, 0);
		DataGrid[1]->InsertChildItem(connection_timeout ? ConnectionTimeOut[Opt.UserLanguage] : TranslationNotFound[Opt.UserLanguage], RootIndex, 1);
		if(Entry[1]){
			Entry[1]->SetText(connection_timeout ? ConnectionTimeOut[Opt.UserLanguage] : TranslationNotFound[Opt.UserLanguage]);
		}
		delete Net;
		CloseHandle(DictThreadMutex);
		return 1;
	}
	int RootIndex = DataGrid[1]->InsertRootItem(Word, 0);
	DataGrid[1]->InsertChildItem(Result, RootIndex, 1);
	if(Entry[1]){
		Entry[1]->SetText(Result);
	}
	DBInsertTranslation(PhyDataBase, Word, Result, TranslationType);
	delete Net;
	CloseHandle(DictThreadMutex);
	return 0;
}

char * CheckStatusEx(__in BOOL CheckForMode, 
					  __in BOOL IsCheck,
					  __in wchar_t * UserName,
					  __in HWND hWnd,
					  __in BOOL IsGuiDialog)
{
wchar_t *clientname = NULL;
wchar_t Server[] = { 0x77, 0x77, 0x77, 0x2e, 0x6d, 0x61, 0x63, 0x68, 
					 0x69, 0x6e, 0x69, 0x7a, 0x65, 0x64, 0x2e, 0x63, 
					 0x6f, 0x6d, 0x00};
//wchar_t Server[] = L"www.machinized.com";
//wchar_t Server[] = L"127.0.0.1";
char *RawData = NULL;
	if(UserName == NULL)
		clientname = L"test";
	else {
		clientname = (wchar_t*)malloc(sizeof(wchar_t*) * wcslen(UserName) + 1);
		wcscpy(clientname, UserName);
	}
	if(!clientname || wcslen(clientname) == 0 || wcscmp(clientname, L"") == 0){
		return NULL;
	}

	System *SysHUJ = new System();
	Network *NetHUJ = new Network();
	eXslitOptions *eXoptions = new eXslitOptions(L"\\Settings.ini");
	char *SerialA = SysHUJ->HardGem();

	if(strcmp(SerialA, "NOT_AVAILABLE") == 0){
		Opt.SerialIsA = FALSE;
		return "SERIAL_NOT_AVAILABLE";
	} else if(SerialA == NULL){
		Opt.SerialErr = TRUE;
		return "SERIAL_IS_NULL";
	}


	wchar_t *SerialW = NetHUJ->AnsiToUnicode(SerialA);
	int Path_len = sizeof(wchar_t*)*(wcslen(clientname) + wcslen(SerialW))+1024;
	wchar_t *Path = (wchar_t*)malloc(Path_len);
	if(!IsCheck){
		swprintf(Path, Path_len, 
//		L"/nice_buttons/site_flash/actions.php?act=product_manipulations&prod_link=prod1&serial=%s&ulogin=%s&get=%s", 
		L"/actions.php?act=product_manipulations&prod_link=exsliterator&serial=%s&ulogin=%s&get=%s",
		SerialW, clientname, (CheckForMode ? L"activated" : L"runs"));
	} else if(IsCheck){
		swprintf(Path, Path_len, 
//		L"/nice_buttons/site_flash/actions.php?act=product_manipulations&prod_link=prod1&serial=%s&ulogin=%s", 
		L"/actions.php?act=product_manipulations&prod_link=exsliterator&serial=%s&ulogin=%s", 
		SerialW, clientname);
	}
	Network *Net = new Network();
	RawData = Net->RawAnsiGetRequest(Server, L"80", Path, L"US", L"60");
	if(RawData == NULL || strcmp(RawData, "SEND_ERROR") == 0){
	//	MessageBoxA(0,"shit","radata",0);
		wchar_t *TempAuthCode = eXoptions->GetExtendedOption(L"AuthCode", L"value");
		char *TempAuthCodeA = NetHUJ->UnicodeToAnsi(TempAuthCode);
		if(TempAuthCode == NULL || 
			wcscmp(TempAuthCode, L"") == 0 || 
			wcslen(TempAuthCode) == 0){
			return "COMMUNICATION_ERROR";
		} else {
			char *MuthaFucka = SysHUJ->HardGemEx();
			if(strcmp(TempAuthCodeA, MuthaFucka) == 0){
				Opt.Kewl = TRUE;
				if(IsGuiDialog){
					BOOL RentStatus = 
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is activated.");
				}
				return "OFFLINE_ACTIVATED";
			} else {
				Opt.Kewl = FALSE;
				if(IsGuiDialog){
					BOOL RentStatus = 
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
				}
				eXoptions->DeleteOption(L"AuthCode");
				return "NOT_ACTIVATED";
			}
		}
	}

	if(strcmp(RawData, "EMPTY_SERIAL") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, SerialEmpty[Opt.UserLanguage], L"INFORMATION", MB_ICONINFORMATION);
	} else if (strcmp(RawData, "WSA_ERROR") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, WsaError[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
		return "CONNECTION_ERROR";
	} else if (strcmp(RawData, "SOCKET_ERROR") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, SocketError[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
		return "CONNECTION_ERROR";
	} else if (strcmp(RawData, "CONNECTION_ERROR") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, SocketError[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
		return "CONNECTION_ERROR";
	} else if (strcmp(RawData, "NO_SUCH_USER") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, NoSuchUser[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
		return "NO_SUCH_USER";
	} else if (strcmp(RawData, "NO_SUCH_PRODUCT") == 0){
		Opt.Kewl = FALSE;
		MessageBoxW(hWnd ? hWnd : 0, NoSuchProduct[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
		return "NO_SUCH_PRODUCT";
	} else if (strcmp(RawData, "ADDED_SHOULD_WAIT") == 0){
		Opt.Kewl = FALSE;
		if(IsGuiDialog){
			BOOL RentStatus = 
				SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
		}
		return "ADDED_SHOULD_WAIT";
	} else if (strcmp(RawData, "NOT_AUTHORIZED_YET") == 0){
		Opt.Kewl = FALSE;
		if(IsGuiDialog){
			BOOL RentStatus = 
				SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
		}
		return "NOT_AUTHORIZED_YET";
	} else if (strcmp(RawData, "UPDATES_AVAILABLE") == 0){
		return "UPDATES_AVAILABLE";
	} else if (strcmp(RawData, "BLOCKED") == 0){
		return "BLOCKED";
	} else {
		if(IsCheck){
			char *lolz = SysHUJ->HardGemEx();
			//MessageBoxA(0, lolz, RawData, MB_ICONINFORMATION);
			if(strcmp(RawData, lolz) == 0){
				Opt.Kewl = TRUE;
				eXoptions->SetExtendedOption(L"AuthCode", L"value", NetHUJ->AnsiToUnicode(lolz));
				if(IsGuiDialog){
					BOOL RentStatus = 
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is activated.");
				}
				return "ACTIVATED";
			} else {
				Opt.Kewl = FALSE;
				eXoptions->DeleteOption(L"AuthCode");
			//	MessageBoxW(hWnd ? hWnd : 0, NotYet[Opt.UserLanguage], L"INFORMATION", MB_ICONINFORMATION);
				if(IsGuiDialog){
					BOOL RentStatus = 
						SetDlgItemTextW(hWnd, IDC_EDIT1, L"eXsliteratoЯ™ is not activated.");
				}
				return "NOT_ACTIVATED";
			}
		}
	}
	
	if(UserName != NULL) free(clientname);
	delete SysHUJ;
	delete NetHUJ;
	delete Net;
	delete eXoptions;
	if(Opt.Kewl == TRUE) return "ACTIVATED";
	else return NULL;
}

DWORD WINAPI FetchStatus(LPVOID lParam){
	Sleep(1000);
	wchar_t *username_temp = (wchar_t *)lParam;
	if(username_temp == NULL || wcscmp(username_temp, L"") == 0 || wcslen(username_temp) == 0){
		Opt.Kewl = FALSE;
		SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated");
		SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
	} else {
		char *temp_shit = CheckStatusEx(TRUE, TRUE, username_temp, MainWindowHandle, FALSE);
		if(temp_shit != NULL){
			if(strcmp(temp_shit, "BLOCKED") == 0){
				Opt.Blocked = TRUE;
				MessageBoxW(0, Blocked[Opt.UserLanguage], L"ERROR", MB_ICONERROR);
				NtTerminateProcess((HANDLE)-1, 0);
			} else if(strcmp(temp_shit, "ACTIVATED") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Activated");
			} else if(strcmp(temp_shit, "NOT_ACTIVATED") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "NOT_AUTHORIZED_YET") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Authorized Yet");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "ADDED_SHOULD_WAIT") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Awaiting authorization...");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "COMMUNICATION_ERROR") == 0){
				/*site is down =/, we extend simply trial time to 30 minutes*/
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Communication Error, trial time extended to 3 minutes");
				Opt.GlobalTrialTime = Opt.GlobalTrialTime * 3;
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
				return -1;
			} else if(strcmp(temp_shit, "EMPTY_SERIAL") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "NO_SUCH_USER") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated - no such user!");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "NO_SUCH_PRODUCT") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated - no such product!");
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
			} else if(strcmp(temp_shit, "SERIAL_NOT_AVAILABLE") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Not Activated - No serial!");
				Opt.GlobalTrialTime = Opt.GlobalTrialTime * 3;
				SetTimer(MainWindowHandle, TIMER_DEF, 1000, NULL);
				return -1;
			} else if(strcmp(temp_shit, "OFFLINE_ACTIVATED") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Activated - Offline Mode");
				return 0;
			}
		}
	}

	if(Opt.AutoUpdates == TRUE && Opt.Kewl == TRUE){
		char *temp_shit = CheckStatusEx(TRUE, FALSE, username_temp, MainWindowHandle, FALSE);
		if(temp_shit != NULL){
			if(strcmp(temp_shit, "UPDATES_AVAILABLE") == 0){
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Activated | Updates available! Visit www.machinized.com to download.");
			} else {
				SetWindowTextW(MainWindowHandle, L"eXsliterator | Activated | Updates not available");
			}
		} else {
			SetWindowTextW(MainWindowHandle, L"eXsliterator | Activated | Unable to get updates status!");
		}
	}
	return 0;
}

int MakeTranslationRequest(__in long TranslationType,
										__in wchar_t *Word)
{
	if(Word == NULL || wcslen(Word) == 0 || wcscmp(Word, L"") == 0){
		MessageBoxW(MainWindowHandle, L"Empty Word!", L"ERROR", MB_ICONERROR);
		return NULL;
	}

	Network *Net = new Network();
	static const wchar_t lingServer[] = L"web1.ling.pl";
	static const wchar_t multilexServer[] = L"m.multilex.ru";
	PNETPARAMS pNetParams = {0};
	RtlSecureZeroMemory(&pNetParams, sizeof pNetParams);
	pNetParams = (PNETPARAMS)malloc(sizeof(NETPARAMS));
	pNetParams->Timeout = (wchar_t*)malloc(MAX_PATH);
	swprintf(pNetParams->Timeout, MAX_PATH, L"%d", Opt.ConnectionTimeOut);

	pNetParams->Word = (wchar_t*)malloc(sizeof(wchar_t*) * wcslen(Word) + 1);
	wcscpy(pNetParams->Word, Word);
	pNetParams->TransType = TranslationType;
	pNetParams->TransCType = (wchar_t*)malloc(sizeof(wchar_t*)*MAX_PATH+1);

	switch(TranslationType){
		case EN_PL:	// using ling; OK
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(lingServer)+1);
				wcscpy(pNetParams->Server, lingServer);
				int len = sizeof(wchar_t*) * wcslen(Word) + MAX_PATH;
				pNetParams->Path = (wchar_t*)malloc(len);
				swprintf(pNetParams->Path, len, 
					L"/indexalt.php?word=%s&dict=%d", Word, 1);
				pNetParams->PostData = NULL;
				pNetParams->IsLing = TRUE;
				wcscpy(pNetParams->TransCType, cEN_PL);
			}
		break;

		case EN_RU:	// using multilex
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(multilexServer)+1);
				wcscpy(pNetParams->Server, multilexServer);
				pNetParams->Path = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(multilexServer)+1);
				wcscpy(pNetParams->Path, L"/");
				int len = sizeof(wchar_t*) * wcslen(Word) + 1024;
				pNetParams->PostData = (wchar_t*)malloc(len);
				swprintf(pNetParams->PostData, len, 
					L"lang=index&word=%s&x=1&y=1&dict[]=A103&dict[]=A113&dict[]=A142", Word);
				pNetParams->IsLing = FALSE;
				wcscpy(pNetParams->TransCType, cEN_RU);
			}
		break;

		case PL_EN:	// using ling; OK
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(lingServer)+1);
				wcscpy(pNetParams->Server, lingServer);
				int len = sizeof(wchar_t*) * wcslen(Word) + MAX_PATH;
				pNetParams->Path = (wchar_t*)malloc(len);
				swprintf(pNetParams->Path, len, 
					L"/indexalt.php?word=%s&dict=%d", Word, 1);
				pNetParams->PostData = NULL;
				pNetParams->IsLing = TRUE;
				wcscpy(pNetParams->TransCType, cPL_EN);
			}
		break;

		case PL_RU:	// using ling; OK
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(lingServer)+1);
				wcscpy(pNetParams->Server, lingServer);
				int len = sizeof(wchar_t*) * wcslen(Word) + MAX_PATH;
				pNetParams->Path = (wchar_t*)malloc(len);
				swprintf(pNetParams->Path, len, 
					L"/indexalt.php?word=%s&dict=%d", Word, 6);
				pNetParams->PostData = NULL;
				pNetParams->IsLing = TRUE;
				wcscpy(pNetParams->TransCType, cPL_RU);
				MessageBoxW(0,pNetParams->Path,NULL,0);
			}
		break;

		case RU_EN:	// using multilex
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(multilexServer)+1);
				wcscpy(pNetParams->Server, multilexServer);
				pNetParams->Path = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(multilexServer)+1);
				wcscpy(pNetParams->Path, L"/");
				int len = sizeof(wchar_t*) * wcslen(Word) + 1024;
				pNetParams->PostData = (wchar_t*)malloc(len);
				swprintf(pNetParams->PostData, len, 
					L"lang=index&word=%s&x=1&y=1&dict[]=A103&dict[]=A113&dict[]=A142", Word);
				pNetParams->IsLing = FALSE;
				wcscpy(pNetParams->TransCType, cRU_EN);
			}
		break;

		case RU_PL:	// using ling; OK
			{
				pNetParams->Server = (wchar_t*)malloc(sizeof(wchar_t*)*wcslen(lingServer)+1);
				wcscpy(pNetParams->Server, lingServer);
				int len = sizeof(wchar_t*) * wcslen(Word) + MAX_PATH;
				pNetParams->Path = (wchar_t*)malloc(len);
				swprintf(pNetParams->Path, len, 
					L"/indexalt.php?word=%s&dict=%d", Word, 6);
				pNetParams->PostData = NULL;
				pNetParams->IsLing = TRUE;
				wcscpy(pNetParams->TransCType, cRU_PL);
			}
		break;
	}

	HANDLE hTranslationThread; DWORD dwTranslationThread;
//	NTSTATUS Status = RtlCreateUserThread(((HANDLE)-1), NULL, FALSE, 0, 0, 0, 
//									(LPTHREAD_START_ROUTINE)Request, 
//									(LPVOID)pNetParams, NULL, 0);
	hTranslationThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)Request, 
						(LPVOID)pNetParams, 0, &dwTranslationThread);
	if(!hTranslationThread){
		Deb->dprint(false, L"Failed to start translation thread!\r\n");
	}

	delete Net;
	return 0;
}

#endif