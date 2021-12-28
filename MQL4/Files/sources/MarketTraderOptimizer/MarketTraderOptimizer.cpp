#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <Mmsystem.h>
#include <process.h>

#pragma comment(lib, "Winmm.lib")

#define MODE_TRADES 0
#define MODE_HISTORY 1
#define OP_BUY 0
#define OP_SELL 1
#define OP_BUYLIMIT 2
#define OP_SELLLIMIT 3
#define OP_BUYSTOP 4
#define OP_SELLSTOP 5
#define OP_SL 0
#define OP_TP 1
#define OP_CLOSE 2
#define OP_DELETE 3
#define PRICE_CLOSE 0
#define PRICE_OPEN 1
#define PRICE_HIGH 2
#define PRICE_LOW 3
#define PRICE_MEDIAN 4
#define PRICE_TYPICAL 5
#define PRICE_WEIGHTED 6

int bars,ibars;
int tfdepth=5;
char* pathCONFIG;
char* pathHST;

int randcycles=25;

struct mdata{
	long int ctm[50000];
	double open[50000];
	double low[50000];
	double high[50000];
	double close[50000];
	double volume[50000];
	int spread[50000];
};
struct mextremums{
	int low[5000];
	int high[5000];
	int barscnt;
	int midprofit;
};

mdata* testermetadata;
mextremums* extremums;

struct tresults{
	int profitcntorders;
	int profitcntordersbuy;
	int profitcntorderssell;
	int period_ma_fast;
	int period_ma_slow;
	int cci_period;
	int period_ma_fast2;
	int period_ma_slow2;
	int cci_period2;
	int profitcntpointsbuy;
	int profitcntpointssell;
	int notprofitcntorders;
	int notprofitcntorderssell;
	int stoplossbuy;
	int stoplosssell;
};

int testercurbar,timeshift;
int randbytes[128][257];

char config[200][9][100];int cindex=0;
char t1config[1000][100];
char t1config2[200];

inline int rdtsc(){__asm__ __volatile__("rdtsc");}	
void initrandbytes(){
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	int cpucount=sysinfo.dwNumberOfProcessors;
	int h=0,i1;
	for(i1=0;i1<cpucount;i1++){
		h=0;
		while(h==0){
			for(int z1=0;z1<51;z1++){for(int z=0;z<256;z++){randbytes[i1][z]^=((rand()<<1) % 256)&255;}}
			for(int z1=0;z1<5;z1++)for(int z=0;z<256;z++)randbytes[i1][z]^=(randbytes[i1][(z+1)%256]>>1)&255;
			for(int z=0;z<256;z++)if (randbytes[i1][z]>127)h=1;
		}
	}
	randbytes[i1][256]=0;
}
int rand(int min, int max, int cpuid){
	int h=randbytes[cpuid][randbytes[cpuid][256]];
	if(h<min)h+=min;
	while(h>max)h=h%max;
	if(h<min)h+=min;
	randbytes[cpuid][randbytes[cpuid][256]]^=randbytes[cpuid][(randbytes[cpuid][256]+1)%256];
	randbytes[cpuid][256]=(randbytes[cpuid][256]+1)%256;	
	return h;
}
inline char* intToStr(int i){
	static char strt[18]="";memset(strt,0,18);
	snprintf(strt, 17, "%ld", i);

	return (char *)strt;
}
inline int strToInt(const char *s){
	char *endChar;
	setlocale(LC_NUMERIC, "C");
	int t = strtol(s, &endChar, 10);
	if(*endChar!='\0')return -1;

	return t;
}
inline char* doubleToStr(double d,int precsion) {
	static char str[18]="";
	memset(str,0,18);
	snprintf(str, 12, "%.*f", precsion,d);

	return (char *)str;
}	
/* void lstrcat(char *buff,const char *past)
{
	int len=0,len0,len1;
	len0=strlen(buff);
	len1=strlen(past);
	memcpy(buff+len0,past,len1);memset(buff+len0+len1,0,1);
}	 */
inline char* timeToStr(const time_t st) {
	static char strtime1[28]="";
    tm stm1;
    time_t st1;
    st1=time(0);
	memset(strtime1,0,28);
	memset(&stm1,0,sizeof(struct tm));
	if(st==0)
	stm1=*localtime((const time_t *)&st1);
	else
	stm1=*localtime((const time_t *)&st);
	strftime(strtime1,27,"%d.%m.%Y %H:%M:%S ",&stm1);
	return (char *)strtime1;
}
inline char* gmtimeToStr(const time_t st) {
	static char gmstrtime[28]="";
     tm stm1;
     time_t st1;
    st1=time(0);
	memset(&gmstrtime,0,28);
	memset(&stm1,0,sizeof(struct tm));
	if(st==0)
	stm1=*gmtime((const time_t *)&st1);
	else
	stm1=*gmtime((const time_t *)&st);
	strftime(gmstrtime,27,"%d.%m.%Y %H:%M ",&stm1);
	return (char *)gmstrtime;
}
typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);
DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
int tsocketscpucnt ()
{
    LPFN_GLPI glpi;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

    glpi = (LPFN_GLPI) GetProcAddress(
                            GetModuleHandle(TEXT("kernel32")),
                            "GetLogicalProcessorInformation");
    if (NULL == glpi) 
    {
        //_tprintf(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
        return (1);
    }

    while (!done)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer) 
                    free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        returnLength);

                if (NULL == buffer) 
                {
                    //_tprintf(TEXT("\nError: Allocation failure\n"));
                    return (2);
                }
            } 
            else 
            {
                //_tprintf(TEXT("\nError %d\n"), GetLastError());
                return (3);
            }
        } 
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
    {
        switch (ptr->Relationship) 
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            break;

        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
            break;

        case RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
            Cache = &ptr->Cache;
            if (Cache->Level == 1)
            {
                processorL1CacheCount++;
            }
            else if (Cache->Level == 2)
            {
                processorL2CacheCount++;
            }
            else if (Cache->Level == 3)
            {
                processorL3CacheCount++;
            }
            break;

        case RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            break;

        default:
            //_tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

/*     _tprintf(TEXT("\nGetLogicalProcessorInformation results:\n"));
    _tprintf(TEXT("Number of NUMA nodes: %d\n"),numaNodeCount);
    _tprintf(TEXT("Number of physical processor sockets: %d\n"),processorPackageCount);
    _tprintf(TEXT("Number of processor cores: %d\n"),processorCoreCount);
    _tprintf(TEXT("Number of logical processors: %d\n"),logicalProcessorCount);
    _tprintf(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), 
            processorL1CacheCount,
            processorL2CacheCount,
            processorL3CacheCount); */

    free(buffer);

    return processorPackageCount;
}
double sma(const int period, const int price, const int shift,int tcurbar){
	double sum=0.0,tmp;
	if(price==PRICE_CLOSE){
		for(int i=0;i<period;i+=2)
		{
			sum+=testermetadata->close[tcurbar-(i+shift)];
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}else
	if(price==PRICE_MEDIAN){
		for(int i=0;i<period;i+=2)
		{
            tmp=testermetadata->high[tcurbar-(i+shift)];
            tmp+=testermetadata->low[tcurbar-(i+shift)];
            tmp*=0.5;
			sum+=tmp;
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}else
	if(price==PRICE_TYPICAL){
		for(int i=0;i<period;i+=2)
		{
			tmp=testermetadata->high[tcurbar-(i+shift)];
			tmp+=testermetadata->low[tcurbar-(i+shift)];
			tmp+=testermetadata->close[tcurbar-(i+shift)];
			tmp/=3.0;
			sum+=tmp;
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}
}	
double cci(const int period, const int shift, int tcurbar ){
	double price,sum,mul,CCIBuffer,RelBuffer,DevBuffer,MovBuffer;
	int k;
	MovBuffer = sma(period, PRICE_TYPICAL, shift,tcurbar);
	mul = 0.015/period;
	sum = 0.0;
	k = period-1;
	k = ((int)(k/2))*2;
	while(k>=0)
	{
		price=(testermetadata->high[tcurbar-(k+shift)]+testermetadata->low[tcurbar-(k+shift)]+testermetadata->close[tcurbar-(k+shift)])/3.0;
		sum+=fabs(price-MovBuffer);
		k-=2;
	}
	DevBuffer = sum*mul;
	price =(testermetadata->high[tcurbar-(shift)]+testermetadata->low[tcurbar-(shift)]+testermetadata->close[tcurbar-(shift)])/3.0;
	RelBuffer=price-MovBuffer;
	if(DevBuffer==0.0)CCIBuffer=0.0;else CCIBuffer = RelBuffer / DevBuffer;
	return(CCIBuffer);
}
int iLowest(int count, int start){
	double Low=99999999;int cLow=0;
	for(int i = start;i > start-count;i--)
	{
		if(testermetadata->low[i]<Low){
		Low=testermetadata->low[i];cLow=i;}
	}
	return(cLow);
}
int iHighest(int count, int start){
	double High=-99999999;int cHigh=0;
	for(int i = start;i > start-count;i--)
	{
		if(testermetadata->high[i]>High){
		High=testermetadata->high[i];cHigh=i;}
	}
	return(cHigh);
}
double loadHST(const char* symbol,const char* tf){
	FILE *hFile;
	char* membuf = new char[2];
	membuf = (char*)realloc(membuf,bars*60);
	
	char* path = new char[2];
	path = (char*)realloc(path,500);memset(path,0,500);
	lstrcat(path,pathHST);lstrcat(path,symbol);lstrcat(path,tf);lstrcat(path,".hst");
	
	int i,i1=0,testerdigits;double point;
	for(i=0;i<50000;i++){
		testermetadata->ctm[i1]=0;
		testermetadata->open[i1]=0.0;
		testermetadata->high[i1]=0.0;
		testermetadata->low[i1]=0.0;
		testermetadata->close[i1]=0.0;
		testermetadata->volume[i1]=0;
		testermetadata->spread[i1]=0;
	}
	hFile = fopen(path, "rb");
	if(!(!hFile)){
			fseek(hFile,0,SEEK_END);
			int dwFileSize = ftell(hFile);
			//int bars=(dwFileSize-148)/60;if(bars>=(ibars))bars=ibars;
			if(dwFileSize>=148+60){
				i=0;
				fseek(hFile,dwFileSize-60*bars-60*timeshift,SEEK_SET);
				fread(membuf,60*bars,1,hFile);
				while(i1<bars){
					memcpy(&testermetadata->ctm[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->open[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->high[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->low[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->close[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->volume[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->spread[i1],&membuf[i],4);i+=12;
					i1++;
				}
			}
			fseek(hFile,84,SEEK_SET);
			fread(&testerdigits,4,1,hFile);
			fclose(hFile);
	}
	delete[] path;
	delete[] membuf;
	point = 1.0 / (int) pow((double)10, testerdigits);
	return point;
}
double icci(int shift, int period_ma_fast, int period_ma_slow, int cci_period,int tcurbar){
   double ma_fast,ma_slow;
   int i1;
   ma_fast=ma_slow=cci(cci_period,shift,tcurbar)+10000.0;
   for(i1=1; i1<period_ma_slow; i1++)
      ma_slow=ma_slow+cci(cci_period,i1+shift,tcurbar)+10000.0;
   ma_slow=ma_slow/period_ma_slow;
   for(i1=1; i1<period_ma_fast; i1++)
      ma_fast=ma_fast+cci(cci_period,i1+shift,tcurbar)+10000.0;
   ma_fast=ma_fast/period_ma_fast;
   return (ma_fast-ma_slow);	
}
int DeltaMasLength(int period_ma_fast, int period_ma_slow, int cci_period,int tcurbar){
/* 	double tmp1,tmp2,tmp3,prevtmp1=icci(1,period_ma_fast, period_ma_slow, cci_period,tcurbar);tmp3=prevtmp1;
	double tmp3_2=fabs(icci(10,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	double tmp3_3=fabs(icci(18,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	if(tmp3<0)tmp2=-1;else tmp2=1;
    prevtmp1=tmp3=fabs(tmp3);
	
	if((tmp3>(tmp3_2*1.3))&&(tmp3>(tmp3_3*1.4)))
	if(tmp3>fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar)))
		for(int i1=2;i1<200;i1++){
			tmp1=fabs(icci(i1,period_ma_fast, period_ma_slow, cci_period,tcurbar));
			if(prevtmp1<tmp1) return (i1*tmp2);
			prevtmp1=tmp1;
		}
	return 0; */
	double tmp1,tmp2,tmp3,prevtmp1=icci(2,period_ma_fast, period_ma_slow, cci_period,tcurbar);tmp3=prevtmp1;
	double tmp3_2=fabs(icci(10,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	double tmp3_3=fabs(icci(18,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	if(tmp3<0)tmp2=-1;else tmp2=1;
    prevtmp1=tmp3=fabs(tmp3);
	
	if((tmp3>(tmp3_2*1.2))&&(tmp3>(tmp3_3*1.3)))
	{
		double tmp4=fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar));
		double tmp5=fabs(icci(1,period_ma_fast, period_ma_slow, cci_period,tcurbar));		
		if(tmp4<=tmp5)
		if(tmp3>fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar))){
			int i1;
			for(i1=3;i1<30;i1+=4){
				tmp1=fabs(icci(i1,period_ma_fast, period_ma_slow, cci_period,tcurbar));
				if(prevtmp1<tmp1) return (i1*tmp2);
				prevtmp1=tmp1;
			}
			return (i1*tmp2);
		}
	}
	return 0;	
}
void testerstart(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, tresults &result){
	int openorder=-1,openorderclosed=1,timeout=(int)(ctimeout/tf/60/2);
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntorders=0,notprofitcntorders=0;
	int tprofitcntpointsbuy=9999999,tprofitcntpointssell=9999999;
	int profitcntordersbuy=0, profitcntorderssell=0;
	double openorderprice;
	int tcurbar;
	int slb=0,sls=0,w1,w2;
	for(int i2=0;i2<extremums->barscnt;i2++){
		w1 = extremums->low[i2];if(w1>(bars-1))w1=(bars-1);
		w2 = extremums->high[i2];if(w2>(bars-1))w2=(bars-1);
		if((openorder>=0)&&(openorderclosed==0)){
			int profitorderb = (int)((testermetadata->high[w2]-openorderprice)/point);
			int profitorders = (int)((testermetadata->low[w1]-openorderprice)/point);
			if((profitorderb>0)&&(openorder==OP_BUY)){
				profitcntordersbuy++;
				profitcntpointsbuy=(int)((testermetadata->high[w2]-openorderprice)/point);
				slb = profitcntpointsbuy /4;
			}
			if((profitorders<0)&&(openorder==OP_SELL)){
				profitcntorderssell++;
				profitcntpointssell=(int)((openorderprice-testermetadata->low[w1])/point);
				sls = profitcntpointssell /4 ;		
			}
			if( ((profitorderb<0)&&(openorder==OP_BUY)) || ((profitorders>0)&&(openorder==OP_SELL)) )notprofitcntorders++;
			openorderclosed=1;
		}
		
		for(int y=0;y<6;y++)
		if(openorderclosed==1){
			if(((openorder==0)||(openorder==-1))&&(w2>0)){
				tcurbar=w2+y;
				int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
				if((abs(signal)>9) && (signal>0)){
				//if(signal>0){
					openorder=OP_SELL;
					openorderclosed=0;
					openorderprice=testermetadata->open[tcurbar];
					//profitcntpointssell=0;
				}
			}else
			if(((openorder==1)||(openorder==-1))&&(w1>0) ){
				tcurbar=w1+y;
				int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
				if((abs(signal)>9) && (signal<0)){
				//if(signal<0){
					openorder=OP_BUY;
					openorderclosed=0;
					openorderprice=testermetadata->open[tcurbar];
					//profitcntpointsbuy=0;
				}
			}		
		}
	}
	
	result.profitcntpointsbuy=profitcntpointsbuy;
	result.profitcntpointssell=profitcntpointssell;
	result.period_ma_fast=period_ma_fast;
	result.period_ma_slow=period_ma_slow;
	result.cci_period=cci_period;
	result.profitcntorders=profitcntordersbuy+profitcntorderssell;
	result.profitcntordersbuy=profitcntordersbuy;
	result.profitcntorderssell=profitcntorderssell;
	result.notprofitcntorders=notprofitcntorders;
	result.stoplossbuy=slb;
	result.stoplosssell=sls;
	
	return;
}
void testerstartb(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, tresults &result){
	int openorder=-1,openorderclosed=1,timeout=16;//(int)(ctimeout/tf/60/2);
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntorders=0,notprofitcntorders=0;
	int profitcntordersbuy=0, profitcntorderssell=0;
	double openorderprice;
	int tcurbar;
	int slb=0,sls=0;
	for(int i=250;i<bars;i++){
		if((openorder>=0)&&(openorderclosed==0)){
			int profitorderb = (int)((testermetadata->high[i]-openorderprice)/point)-testermetadata->spread[bars-2];
			if((profitorderb>0)&&(openorder==OP_BUY)){
				profitcntordersbuy++;
				profitcntpointsbuy+=(testermetadata->high[iHighest(timeout,i)]-openorderprice)/point;
				slb+=abs((testermetadata->low[iLowest(timeout,i)]-openorderprice)/point);
			}
			else notprofitcntorders++;
			openorderclosed=1;
		}
		
		if(openorderclosed==1){
			if((openorder==OP_BUY)||(openorder==-1) ){
				tcurbar=i;
				int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
				if((abs(signal)>19) && (signal<0)){
					openorder=OP_BUY;
					openorderclosed=0;
					openorderprice=testermetadata->open[tcurbar];
					i+=timeout;
				}
			}		
		}
	}
	if(profitcntordersbuy>0){
		profitcntpointsbuy = (int)(profitcntpointsbuy/profitcntordersbuy);
		slb = (int)(slb/profitcntordersbuy);
	}
	else profitcntordersbuy=0;
	if((slb<extremums->midprofit)||(profitcntpointsbuy<extremums->midprofit)){result.profitcntorders=0;return;}
	
	result.profitcntpointsbuy=profitcntpointsbuy;
	//result.profitcntpointssell=profitcntpointssell;
	result.period_ma_fast=period_ma_fast;
	result.period_ma_slow=period_ma_slow;
	result.cci_period=cci_period;
	result.profitcntorders=profitcntordersbuy;
	result.profitcntordersbuy=profitcntordersbuy;
	//result.profitcntorderssell=profitcntorderssell;
	result.notprofitcntorders=notprofitcntorders;
	result.stoplossbuy=slb;
	//result.stoplosssell=sls;
	
	return;
}
void testerstarts(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, tresults &result){
	int openorder=-1,openorderclosed=1,timeout=16;//(int)(ctimeout/tf/60/2);
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntorders=0,notprofitcntorders=0,notprofitcntorderssell=0;
	int profitcntordersbuy=0, profitcntorderssell=0;
	double openorderprice;
	int tcurbar;
	int slb=0,sls=0;
	for(int i=250;i<bars;i++){
		if((openorder>=0)&&(openorderclosed==0)){
			int profitorders = (int)((testermetadata->low[i]-openorderprice)/point)-testermetadata->spread[bars-2];
			if((profitorders<0)&&(openorder==OP_SELL)){
				profitcntorderssell++;
				profitcntpointssell+=abs((testermetadata->low[iLowest(timeout,i)]-openorderprice)/point);
				sls+=abs((testermetadata->high[iHighest(timeout,i)]-openorderprice)/point);
			}else notprofitcntorderssell++;
			openorderclosed=1;
		}
		
		if(openorderclosed==1){
			if((openorder==OP_SELL)||(openorder==-1)){
				tcurbar=i;
				int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
				if((abs(signal)>19) && (signal>0)){
					openorder=OP_SELL;
					openorderclosed=0;
					openorderprice=testermetadata->open[tcurbar];
					i+=timeout;
				}
			}
		}
	}
	if(profitcntorderssell>0){
		profitcntpointssell=(int)(profitcntpointssell/profitcntorderssell);
		sls=(int)(sls/profitcntorderssell);
	}
	else profitcntpointssell=0;
	if((sls<extremums->midprofit)||(profitcntpointssell<extremums->midprofit)){result.profitcntorderssell=0;return;}
	
	//result.profitcntpointsbuy=profitcntpointsbuy;
	result.profitcntpointssell=profitcntpointssell;
	result.period_ma_fast2=period_ma_fast;
	result.period_ma_slow2=period_ma_slow;
	result.cci_period2=cci_period;
	//result.profitcntorders=profitcntordersbuy+profitcntorderssell;
	//result.profitcntordersbuy=profitcntordersbuy;
	result.profitcntorderssell=profitcntorderssell;
	result.notprofitcntorderssell=notprofitcntorderssell;
	//result.stoplossbuy=slb;
	result.stoplosssell=sls;
	
	return;
}
struct tthread{
	HANDLE handle;
	bool handleclosed;
	DWORD threadid;
	int id;
	int tf;
	int timeout;
	double point;
	int randcycles;
	bool done;
	tresults results;
	tresults tmpresults;
};
int treadcount;
DWORD WINAPI myThread0(LPVOID lpParameter){
	tthread& thread = *((tthread*)lpParameter);
	
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0,period_ma_fast=0,period_ma_slow=0,cci_period=0;
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntordersbuy=0,profitcntorderssell=0;
	int sls=0,slb=0;
	tresults& testerresult = thread.tmpresults;
	
	int tf=thread.tf;int timeout=thread.timeout;
	for(int i=0;i<thread.randcycles;i++){
		int t1=2,t2=1, t3=0;
		while(t1>=t2){t1=rand(12,55,thread.id);t2=rand(77,222,thread.id);t3=rand(55,222,thread.id);}
		testerstartb(tf,thread.point,timeout,t1,t2,t3,testerresult);
		//if( (testerresult.profitcntpointsbuy!=-1) && (testerresult.profitcntpointssell!=-1) )
		//if( ((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (testerresult.profitcntorders >4) && (testerresult.profitcntorders>(testerresult.notprofitcntorders*3))){
		if(testerresult.profitcntorders>1);
		if((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)){
			profitcntpointsbuy = testerresult.profitcntpointsbuy;
			profitcntpointssell = testerresult.profitcntpointssell;
			period_ma_fast = testerresult.period_ma_fast;
			period_ma_slow = testerresult.period_ma_slow;
			cci_period = testerresult.cci_period;
			profitcntorders = testerresult.profitcntorders;
			profitcntordersbuy = testerresult.profitcntordersbuy;
			profitcntorderssell = testerresult.profitcntorderssell;
			notprofitcntorders = testerresult.notprofitcntorders;
			sls = testerresult.stoplosssell;
			slb = testerresult.stoplossbuy;
			
		}
	}
	thread.results.profitcntpointsbuy = profitcntpointsbuy;
	thread.results.profitcntpointssell = profitcntpointssell;
	thread.results.period_ma_fast = period_ma_fast;
	thread.results.period_ma_slow = period_ma_slow;
	thread.results.cci_period = cci_period;
	thread.results.profitcntorders = profitcntorders;
	thread.results.profitcntordersbuy = profitcntordersbuy;
	thread.results.profitcntorderssell = profitcntorderssell;
	thread.results.notprofitcntorders = notprofitcntorders;
	thread.results.stoplossbuy = slb;
	thread.results.stoplosssell = sls;
	thread.done = true;
	
	
	return 0;
}
DWORD WINAPI myThread(LPVOID lpParameter){
	tthread& thread = *((tthread*)lpParameter);
	
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0,notprofitcntorderssell=0,period_ma_fast=0,period_ma_slow=0,cci_period=0,period_ma_fast2=0,period_ma_slow2=0,cci_period2=0;
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntordersbuy=0,profitcntorderssell=0;
	int sls=0,slb=0;
	tresults& testerresult = thread.tmpresults;
	
	int tf=thread.tf;int timeout=thread.timeout;
	for(int i=0;i<thread.randcycles;i++){
		int t1=2,t2=1, t3=0;
		while(t1>=t2){t1=rand(12,55,thread.id);t2=rand(77,222,thread.id);t3=rand(55,222,thread.id);}
		testerstartb(tf,thread.point,timeout,t1,t2,t3,testerresult);
		//if( (testerresult.profitcntpointsbuy!=-1) && (testerresult.profitcntpointssell!=-1) )
		//if( ((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (testerresult.profitcntorders >4) && (testerresult.profitcntorders>(testerresult.notprofitcntorders*3))){
		if(testerresult.profitcntorders>0)
		if(testerresult.profitcntorders>(4*testerresult.notprofitcntorders))
		if((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)){
		//if(testerresult.profitcntpointsbuy>profitcntpointsbuy){
			profitcntpointsbuy = testerresult.profitcntpointsbuy;
			//profitcntpointssell = testerresult.profitcntpointssell;
			period_ma_fast = testerresult.period_ma_fast;
			period_ma_slow = testerresult.period_ma_slow;
			cci_period = testerresult.cci_period;
			profitcntorders = testerresult.profitcntorders;
			profitcntordersbuy = testerresult.profitcntordersbuy;
			//profitcntorderssell = testerresult.profitcntorderssell;
			notprofitcntorders = testerresult.notprofitcntorders;
			//sls = testerresult.stoplosssell;
			slb = testerresult.stoplossbuy;
			
		}
	}
	for(int i=0;i<thread.randcycles;i++){
		int t1=2,t2=1, t3=0;
		while(t1>=t2){t1=rand(12,55,thread.id);t2=rand(77,222,thread.id);t3=rand(55,222,thread.id);}
		testerstarts(tf,thread.point,timeout,t1,t2,t3,testerresult);
		//if( (testerresult.profitcntpointsbuy!=-1) && (testerresult.profitcntpointssell!=-1) )
		//if( ((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (testerresult.profitcntorders >4) && (testerresult.profitcntorders>(testerresult.notprofitcntorders*3))){
		if(testerresult.profitcntorderssell>0)
		if(testerresult.profitcntorderssell>(4*testerresult.notprofitcntorderssell))
		if((testerresult.profitcntorderssell-testerresult.notprofitcntorderssell)>(profitcntorderssell-notprofitcntorderssell)){
		//if(testerresult.profitcntpointssell>profitcntpointssell){
			//profitcntpointsbuy = testerresult.profitcntpointsbuy;
			profitcntpointssell = testerresult.profitcntpointssell;
			period_ma_fast2 = testerresult.period_ma_fast2;
			period_ma_slow2 = testerresult.period_ma_slow2;
			cci_period2 = testerresult.cci_period2;
			//profitcntorders = testerresult.profitcntorders;
			//profitcntordersbuy = testerresult.profitcntordersbuy;
			profitcntorderssell = testerresult.profitcntorderssell;
			//notprofitcntorders = testerresult.notprofitcntorders;
			notprofitcntorderssell = testerresult.notprofitcntorderssell;
			sls = testerresult.stoplosssell;
			//slb = testerresult.stoplossbuy;
			
		}
	}
	thread.results.profitcntpointsbuy = profitcntpointsbuy;
	thread.results.profitcntpointssell = profitcntpointssell;
	thread.results.period_ma_fast = period_ma_fast;
	thread.results.period_ma_slow = period_ma_slow;
	thread.results.cci_period = cci_period;
	thread.results.period_ma_fast2 = period_ma_fast2;
	thread.results.period_ma_slow2 = period_ma_slow2;
	thread.results.cci_period2 = cci_period2;
	thread.results.profitcntorders = profitcntorders+profitcntorderssell;
	thread.results.profitcntordersbuy = profitcntordersbuy;
	thread.results.profitcntorderssell = profitcntorderssell;
	thread.results.notprofitcntorders = notprofitcntorders+notprofitcntorderssell;
	thread.results.stoplossbuy = slb;
	thread.results.stoplosssell = sls;
	thread.done = true;
	
	
	return 0;
}
const char* testertest(const char* ctf,double point, const char* ctimeout,int spr) {
	static char itemconfig[200]="";
	memset(itemconfig,0,200);
	int tf=strToInt(ctf);int timeout=strToInt(ctimeout);
	int tpb,tps;

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	treadcount=sysinfo.dwNumberOfProcessors*tsocketscpucnt();
	tthread threads[treadcount];
	for(int i=0;i<treadcount;i++){
		threads[i].done = false;
		threads[i].id = i;
		threads[i].tf = tf;
		threads[i].timeout = timeout;
		threads[i].point = point;
		threads[i].randcycles = randcycles;
		threads[i].handleclosed = false;
		threads[i].handle=CreateThread(0, 0, myThread, &threads[i], 0, &threads[i].threadid);
	}	
	
	bool thredsdone=false;
	while(!thredsdone){
		thredsdone=true;
		for(int i=0;i<treadcount;i++){
			if(threads[i].done == false)thredsdone=false; 
			else {
				if(!threads[i].handleclosed){
					CloseHandle(threads[i].handle);
					threads[i].handleclosed = true;
				}
			}
		}
		SleepEx(100,true);
	}
	
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0,period_ma_fast=0,period_ma_slow=0,cci_period=0,period_ma_fast2=0,period_ma_slow2=0,cci_period2=0;
	int profitcntpointsbuy=0,profitcntpointssell=0,profitcntordersbuy=0,profitcntorderssell=0;
	int sls=0,slb=0;
	for(int i=0;i<treadcount;i++){
	//	if( ((threads[i].results.profitcntorders-threads[i].results.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (threads[i].results.profitcntorders >4) && (threads[i].results.profitcntorders>(threads[i].results.notprofitcntorders*3))){
		if(threads[i].results.profitcntorders>1)
		if((threads[i].results.profitcntorders-threads[i].results.notprofitcntorders)>(profitcntorders-notprofitcntorders)){
			profitcntpointsbuy = threads[i].results.profitcntpointsbuy;
			profitcntpointssell = threads[i].results.profitcntpointssell;
			period_ma_fast = threads[i].results.period_ma_fast;
			period_ma_slow = threads[i].results.period_ma_slow;
			cci_period = threads[i].results.cci_period;
			period_ma_fast2 = threads[i].results.period_ma_fast2;
			period_ma_slow2 = threads[i].results.period_ma_slow2;
			cci_period2 = threads[i].results.cci_period2;
			profitcntorders = threads[i].results.profitcntorders;
			profitcntordersbuy = threads[i].results.profitcntordersbuy;
			profitcntorderssell = threads[i].results.profitcntorderssell;
			notprofitcntorders = threads[i].results.notprofitcntorders;
			sls = threads[i].results.stoplosssell;
			slb = threads[i].results.stoplossbuy;
		}
	}

	tpb = 0;
	tps = 0;
	if(profitcntordersbuy>0)tpb = (int)(profitcntpointsbuy);
	if(profitcntorderssell>0)tps = (int)(profitcntpointssell);
	lstrcat(itemconfig,ctf);lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_fast));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_slow));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(cci_period));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(tps));lstrcat(itemconfig," ");
	lstrcat(itemconfig,ctimeout);lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(spr));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(notprofitcntorders));lstrcat(itemconfig," ");	
	lstrcat(itemconfig,intToStr(profitcntorders));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(sls));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(slb));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(tpb));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_fast2));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_slow2));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(cci_period2));
	return (const char *)itemconfig;
}

double ExtZigzagBuffer[50000];
double ExtHighBuffer[50000];
double ExtLowBuffer[50000];
int InitializeAll(int InpDepth)
  {
   for(int i1=0;i1<bars;i1++){
	   ExtZigzagBuffer[i1]=0.0;
	   ExtHighBuffer[i1]=0.0;
	   ExtLowBuffer[i1]=0.0;
   }
//--- first counting position
   return(bars-InpDepth);
  }
void ZigZag(double Point){
   int    i,limit,counterZ,whatlookfor=0;
   int    back,pos,lasthighpos=0,lastlowpos=0;
   double extremum;
   double curlow=0.0,curhigh=0.0,lasthigh=0.0,lastlow=0.0;
	int InpDepth=12;     // Depth
	int InpDeviation=5;  // Deviation
	int InpBackstep=3;   // Backstep
	int    ExtLevel=3;   
    limit=InitializeAll(InpDepth);
    
     {
      //--- find first extremum in the depth ExtLevel or 100 last bars
      i=counterZ=0;
      while(counterZ<ExtLevel && i<100)
        {
         if(ExtZigzagBuffer[bars-i]!=0.0)
            counterZ++;
         i++;
        }
      //--- no extremum found - recounting all from begin
      if(counterZ==0)limit=InitializeAll(InpDepth);
      else
        {
         //--- set start position to found extremum position
         limit=i-1;
         //--- what kind of extremum?
         if(ExtLowBuffer[bars-i]!=0.0) 
           {
            //--- low extremum
            curlow=ExtLowBuffer[bars-i];
            //--- will look for the next high extremum
            whatlookfor=1;
           }
         else
           {
            //--- high extremum
            curhigh=ExtHighBuffer[bars-i];
            //--- will look for the next low extremum
            whatlookfor=-1;
           }
         //--- clear the rest data
         for(i=limit-1; i>=0; i--)  
           {
            ExtZigzagBuffer[bars-i]=0.0;  
            ExtLowBuffer[bars-i]=0.0;
            ExtHighBuffer[bars-i]=0.0;
           }
        }
     }
//--- main loop      
   for(i=limit; i>=0; i--)
     {
      //--- find lowest low in depth of bars
      extremum=testermetadata->low[bars-iLowest(InpDepth,i)];
      //--- this lowest has been found previously
      if(extremum==lastlow)
         extremum=0.0;
      else 
        { 
         //--- new last low
         lastlow=extremum; 
         //--- discard extremum if current low is too high
         if(testermetadata->low[bars-i]-extremum>InpDeviation*Point)
            extremum=0.0;
         else
           {
            //--- clear previous extremums in backstep bars
            for(back=1; back<=InpBackstep; back++)
              {
               pos=i+back;
               if(ExtLowBuffer[bars-pos]!=0 && ExtLowBuffer[bars-pos]>extremum)
                  ExtLowBuffer[bars-pos]=0.0; 
              }
           }
        } 
      //--- found extremum is current low
      if(testermetadata->low[bars-i]==extremum)
         ExtLowBuffer[bars-i]=extremum;
      else
         ExtLowBuffer[bars-i]=0.0;
      //--- find highest high in depth of bars
      extremum=testermetadata->high[bars-iHighest(InpDepth,i)];
      //--- this highest has been found previously
      if(extremum==lasthigh)
         extremum=0.0;
      else 
        {
         //--- new last high
         lasthigh=extremum;
         //--- discard extremum if current high is too low
         if(extremum-testermetadata->high[bars-i]>InpDeviation*Point)
            extremum=0.0;
         else
           {
            //--- clear previous extremums in backstep bars
            for(back=1; back<=InpBackstep; back++)
              {
               pos=i+back;
               if(ExtHighBuffer[bars-pos]!=0 && ExtHighBuffer[bars-pos]<extremum)
                  ExtHighBuffer[bars-pos]=0.0; 
              } 
           }
        }
      //--- found extremum is current high
      if(testermetadata->high[bars-i]==extremum)
         ExtHighBuffer[bars-i]=extremum;
      else
         ExtHighBuffer[bars-i]=0.0;
     }
//--- final cutting 
   if(whatlookfor==0)
     {
      lastlow=0.0;
      lasthigh=0.0;  
     }
   else
     {
      lastlow=curlow;
      lasthigh=curhigh;
     }
   for(i=limit; i>=0; i--)
     {
      switch(whatlookfor)
        {
         case 0: // look for peak or lawn 
            if(lastlow==0.0 && lasthigh==0.0)
              {
               if(ExtHighBuffer[bars-i]!=0.0)
                 {
                  lasthigh=testermetadata->high[bars-i];
                  lasthighpos=i;
                  whatlookfor=-1;
                  ExtZigzagBuffer[bars-i]=lasthigh;
                 }
               if(ExtLowBuffer[bars-i]!=0.0)
                 {
                  lastlow=testermetadata->low[bars-i];
                  lastlowpos=i;
                  whatlookfor=1;
                  ExtZigzagBuffer[bars-i]=lastlow;
                 }
              }
             break;  
         case 1: // look for peak
            if(ExtLowBuffer[bars-i]!=0.0 && ExtLowBuffer[bars-i]<lastlow && ExtHighBuffer[bars-i]==0.0)
              {
               ExtZigzagBuffer[bars-lastlowpos]=0.0;
               lastlowpos=i;
               lastlow=ExtLowBuffer[bars-i];
               ExtZigzagBuffer[bars-i]=lastlow;
              }
            if(ExtHighBuffer[bars-i]!=0.0 && ExtLowBuffer[bars-i]==0.0)
              {
               lasthigh=ExtHighBuffer[bars-i];
               lasthighpos=i;
               ExtZigzagBuffer[bars-i]=lasthigh;
               whatlookfor=-1;
              }   
            break;               
         case -1: // look for lawn
            if(ExtHighBuffer[bars-i]!=0.0 && ExtHighBuffer[bars-i]>lasthigh && ExtLowBuffer[bars-i]==0.0)
              {
               ExtZigzagBuffer[bars-lasthighpos]=0.0;
               lasthighpos=i;
               lasthigh=ExtHighBuffer[bars-i];
               ExtZigzagBuffer[bars-i]=lasthigh;
              }
            if(ExtLowBuffer[bars-i]!=0.0 && ExtHighBuffer[bars-i]==0.0)
              {
               lastlow=ExtLowBuffer[bars-i];
               lastlowpos=i;
               ExtZigzagBuffer[bars-i]=lastlow;
               whatlookfor=1;
              }   
            break;               
        }
     }	
//=============== Optimize ZigZag ===============================
	double curhl=0.0;int curpos=0,curl2=-1,errors=0,hptr=0,lptr=0;
	for(int i=250;i<bars;i++){
		if(ExtZigzagBuffer[i]!=0.0){
			curhl=ExtZigzagBuffer[i];curpos=i;break;
		}
	}
	for(int i=0;i<5000;i++){extremums->high[i]=0;extremums->low[i]=0;}
	extremums->barscnt = 1;
	for(int i=curpos+1;i<bars;i++){
		if(ExtZigzagBuffer[i]!=0.0){
			if(curhl<ExtZigzagBuffer[i]){if( (curl2==-1) || (curl2==0) ){
				if(curl2==-1){extremums->high[hptr]=curpos;extremums->low[lptr]=0;lptr++;}
				  else {extremums->low[lptr]=i;lptr++;}
				curl2=1;
				}else errors++;}
			if(curhl>ExtZigzagBuffer[i]){if( (curl2==-1) || (curl2==1) ){
				if(curl2==-1){extremums->low[lptr]=curpos;extremums->high[hptr]=0;hptr++;}
				  else {extremums->high[hptr]=i;hptr++;}
				curl2=0;
				}else errors++;}
			curhl=ExtZigzagBuffer[i];
			extremums->barscnt++;
		}
	}
	int profit=0;
	for(int i=1;i<extremums->barscnt-1;i++){
		int t;
		t=abs((int)((testermetadata->high[extremums->high[i]]-testermetadata->low[extremums->low[i]])/Point));
		//if(profit>t)profit=t;
		profit+=t;
		
		if(extremums->high[0]==0)
			t=abs((int)((testermetadata->high[extremums->high[i]]-testermetadata->low[extremums->low[i-1]])/Point));
		else
			t=abs((int)((testermetadata->high[extremums->high[i-1]]-testermetadata->low[extremums->low[i]])/Point));
		//if(profit>t)profit=t;
		profit+=t;
	}
	extremums->midprofit = (profit/(extremums->barscnt-2)/2);
}

const char* optimize(const char* symbol,const char* tf, const char* timeout,int spr) {
	double point = loadHST(symbol,tf);
	ZigZag(point);
	return (const char *)testertest(tf,point,timeout,spr);
}
void ReadConfig(){
	
	FILE *hFile;
	char* membuf = new char[2];membuf = (char*)realloc(membuf,50000);memset(membuf,0,50000);
	char* str1 = new char[2];str1 = (char*)realloc(str1,100);memset(str1,0,100);
	char tconfig[1000][100];int tconfigindex=0;
	
	int i,i1=0,testerdigits;double point;
	hFile = fopen(pathCONFIG, "rb");
	if(!(!hFile)){
			fseek(hFile,0,SEEK_END);
			int dwFileSize = ftell(hFile);
			fseek(hFile,0,SEEK_SET);
			fread(membuf,dwFileSize,1,hFile);

			fclose(hFile);
	}
	char* ptr=&membuf[0];char* prevptr=&membuf[0];tconfigindex=0;
	while(ptr!=NULL){
	char symbol1='\r';
	char symbol2='\0';
	ptr=strchr (prevptr,symbol1);
		if (ptr!=NULL){
			*ptr=symbol2;
			*(ptr+1)=symbol2;
			memset(&tconfig[tconfigindex][0],0,100);
			lstrcat(&tconfig[tconfigindex][0],prevptr);
			tconfigindex++;
			prevptr=ptr+2;
		}
	}
	memset(&tconfig[tconfigindex][0],0,100);
	lstrcat(&tconfig[tconfigindex][0],prevptr);
	tconfigindex++;	

//==============================
	int tmp02=0,cindex1=0,cindex2,c1=0;
	memset(membuf,0,50000);
	while(tmp02==0)
	  {
	   if(!(c1==tconfigindex))
		 {
		  cindex2=1;
		  memset(&config[cindex1][cindex2][0],0,100);
		  lstrcat(&config[cindex1][cindex2][0],&tconfig[c1][0]);c1++;
		  cindex2++;
		  memset(&config[cindex1][0][0],0,100);
		  lstrcat(&config[cindex1][0][0],"0");
		  memset(str1,0,100);
		  lstrcat(str1,"1");
		  while((strlen(str1)>0) && (c1!=tconfigindex))
			{
			 memset(str1,0,100);
			 lstrcat(str1,&tconfig[c1][0]);c1++;
			 if(strlen(str1)>0)
			   {
				memset(&config[cindex1][cindex2][0],0,100);
				lstrcat(&config[cindex1][cindex2][0],str1);
				cindex2++;
				int tmp03=strToInt(&config[cindex1][0][0]);
				memset(&config[cindex1][0][0],0,100);
				lstrcat(&config[cindex1][0][0],intToStr(tmp03+1));
			   }
			}
		  if(c1==tconfigindex)
			 tmp02=1;
		  cindex1++;
		  

		 }
	  }
	cindex=cindex1;

	delete[] str1;
	delete[] membuf;

	
}
char* SetElement(char* str, int index,char* str2){
	char* membuf = new char[2];membuf = (char*)realloc(membuf,300);memset(membuf,0,300);
	int tconfigindex=0;
	lstrcat(membuf,str);

	char* ptr=&membuf[0];char* prevptr=&membuf[0];tconfigindex=0;
	while(ptr!=NULL){
	char symbol1=' ';
	char symbol2='\0';
	ptr=strchr (prevptr,symbol1);
		if (ptr!=NULL){
			*ptr=symbol2;
			memset(&t1config[tconfigindex][0],0,100);
			lstrcat(&t1config[tconfigindex][0],prevptr);
			tconfigindex++;
			prevptr=ptr+1;
		}
	}
	memset(&t1config[tconfigindex][0],0,100);
	lstrcat(&t1config[tconfigindex][0],prevptr);
	tconfigindex++;	
	delete[] membuf;
	memset(&t1config[index][0],0,100);
	lstrcat(&t1config[index][0],str2);
	int i;
	for(i=0;i<tconfigindex-1;i++){lstrcat(t1config2,&t1config[i][0]);lstrcat(t1config2," ");}
	lstrcat(t1config2,&t1config[i][0]);
	return &t1config2[0];
}
char* GetElement(char* str, int index){
	char* membuf = new char[2];membuf = (char*)realloc(membuf,300);memset(membuf,0,300);
	int tconfigindex=0;
	lstrcat(membuf,str);

	char* ptr=&membuf[0];char* prevptr=&membuf[0];tconfigindex=0;
	while(ptr!=NULL){
	char symbol1=' ';
	char symbol2='\0';
	ptr=strchr (prevptr,symbol1);
		if (ptr!=NULL){
			*ptr=symbol2;
			memset(&t1config[tconfigindex][0],0,100);
			lstrcat(&t1config[tconfigindex][0],prevptr);
			tconfigindex++;
			prevptr=ptr+1;
		}
	}
	memset(&t1config[tconfigindex][0],0,100);
	lstrcat(&t1config[tconfigindex][0],prevptr);
	tconfigindex++;	
	delete[] membuf;
	return &t1config[index][0];
}
void PatchConfig(char* symbol, char* param){
	int i1,i2;
	for(i1=0; i1<cindex; i1++)
	if(strcmp(symbol,config[i1][1])==0)
	   for(i2=0; i2<strToInt(&config[i1][0][0]); i2++){
		  char c1[100],c2[100]; memset(&c1[0],0,100);memset(&c2[0],0,100);
		  lstrcat(&c1[0],GetElement(&param[0],0));
		  lstrcat(&c2[0],GetElement(&config[i1][i2+2][0],0));
		  if(strcmp(c1,c2)==0){
			 memset(&config[i1][i2+2][0],0,100);
			 lstrcat(&config[i1][i2+2][0],param);
		  }
	   }
}
void SaveConfig(){
	char* membuf = new char[2];membuf = (char*)realloc(membuf,50000);memset(membuf,0,50000);
	FILE *os;os= fopen(pathCONFIG,"wb");
	
	for(int i1=0; i1<cindex; i1++)
	{
		lstrcat(membuf,&config[i1][1][0]);lstrcat(membuf,"\r\n");
		for(int i2=0; i2<strToInt(&config[i1][0][0]); i2++)
		  {
		   lstrcat(membuf,&config[i1][i2+2][0]);
		   if(i1!=(cindex-1))
			  lstrcat(membuf,"\r\n");
		   if((i1==(cindex-1))&&(i2!=(strToInt(&config[i1][0][0])-1)))
			  lstrcat(membuf,"\r\n");
		  }
		if(i1!=(cindex-1))
		   lstrcat(membuf,"\r\n");
	}	
	
	int tmp0=strlen(membuf);
	fwrite(membuf,tmp0,1,os);
	fclose(os);
	delete[] membuf;	
}
size_t to_narrow(const wchar_t * src, char * dest, size_t dest_len){
  size_t i;
  wchar_t code;

  i = 0;

  while (src[i] != '\0' && i < (dest_len - 1)){
    code = src[i];
    if (code < 128)
      dest[i] = char(code);
    else{
      dest[i] = '?';
      if (code >= 0xD800 && code <= 0xD8FF)
        // lead surrogate, skip the next code unit, which is the trail
        i++;
    }
    i++;
  }

  dest[i] = '\0';

  return i - 1;

}
int time1(const time_t st) {
     tm stm1;
     time_t st1;
    st1=time(0);
	memset(&stm1,0,sizeof(struct tm));
	if(st==0)
	stm1=*localtime((const time_t *)&st1);
	else
	stm1=*localtime((const time_t *)&st);
	return (int)mktime(&stm1);
}
int main(int argc, char *argv[]){
	printf(timeToStr(time(NULL))); printf(" - time start\r\n");
	SleepEx(5000,true);
	double title1,dt0=time(NULL);
	rdtsc();
	srand(time(0));
	initrandbytes();

	pathCONFIG = new char[500];memset(pathCONFIG,0,500);lstrcat(pathCONFIG,argv[3]);
	pathHST = new char[500];memset(pathHST,0,500);lstrcat(pathHST,"..\\..\\history\\");lstrcat(pathHST,argv[7]);lstrcat(pathHST,"\\");
	tfdepth = 10;bars = strToInt(argv[1]);
	timeshift = strToInt(argv[4]);
	//if(bars<=(timeshift+1500))bars=timeshift+1500;
	char *stm1;stm1 = (char *)malloc(400000);memset(stm1,0,400000);
	char tf[10];memset(tf,0,10);char timeout[60];memset(timeout,0,60);char takeprofitbuy[10];memset(takeprofitbuy,0,10);
	char optresult[300];memset(optresult,0,300);
	
	testermetadata = new mdata[1];
	extremums = new mextremums[1];

	ReadConfig();
	
	for(int i1=0;i1<cindex;i1++){
		printf("\r\n");printf(&config[i1][1][0]);printf("\r\n");
		for(int i2=0;i2<strToInt(&config[i1][0][0]);i2++)
		if(i2<tfdepth){
			memset(tf,0,10);memset(timeout,0,60);memset(optresult,0,300);
			lstrcat(tf,GetElement(&config[i1][i2+2][0],0));
			lstrcat(timeout,GetElement(&config[i1][i2+2][0],5));
			int spr = strToInt(GetElement(&config[i1][i2+2][0],6));
			lstrcat(optresult,optimize(&config[i1][1][0],tf,timeout,spr));
			printf(optresult);printf("\r\n");
			PatchConfig(&config[i1][1][0],optresult);
		}else {
			printf(&config[i1][i2+2][0]);printf("\r\n");
		}
	}
	SaveConfig();
	lstrcat(stm1,"\r\n");

	title1=time(NULL);title1-=dt0;title1/=60;
	lstrcat(stm1,doubleToStr(title1,1));
	lstrcat(stm1," minutes used");
	printf(stm1);
	MessageBeep(MB_OK);
    free(stm1);
	free(pathHST);
	free(pathCONFIG);
	delete[] extremums;
	delete[] testermetadata;



	FILE *hFile;
	wchar_t path1[255];GetCurrentDirectoryW(255,path1);
	char path[255];memset(path,0,255);to_narrow(path1, path, 255);
	lstrcat(path,"\\settings.txt");
	char* str1 = new char[2];str1 = (char*)realloc(str1,500);memset(str1,0,500);
	hFile = fopen(path, "rb");
	if(!(!hFile)){
		fseek(hFile,0,SEEK_END);
		int dwFileSize = ftell(hFile);
		fseek(hFile,0,SEEK_SET);
		fread(str1,dwFileSize,1,hFile);
		fclose(hFile);
	}
	str1 = SetElement(str1,1,intToStr(time1(NULL)+60*60*10));
	hFile = fopen(path, "wb");
	if(!(!hFile)){
		fseek(hFile,0,SEEK_SET);
		fwrite(str1,strlen(str1),1,hFile);
		fclose(hFile);
	}
	
	wchar_t tt[255];GetCurrentDirectoryW(255,tt);
	char tt1[255];memset(tt1,0,255);to_narrow(tt, tt1, 255);
	lstrcat(tt1,"\\TesterStart.bat");
    _execlp(tt1, tt1, "", "", "", "", nullptr);

	
}