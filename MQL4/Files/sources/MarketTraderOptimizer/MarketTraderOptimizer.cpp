#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <Mmsystem.h>

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

int bars = 1200;
int tfdepth=5;
char* pathCONFIG;
char* pathHST;

int randcycles=200;

struct mdata{
	long int ctm[2000];
	double open[2000];
	double low[2000];
	double high[2000];
	double close[2000];
	double volume[2000];
};
mdata* testermetadata;

struct tresults{
	int profitcntorders;
	int period_ma_fast;
	int period_ma_slow;
	int cci_period;
	int profitcntpoints;
	int notprofitcntorders;
};

int testercurbar;
int randbytes[128][257];

char config[200][9][100];int cindex=0;
char t1config[1000][100];

inline int rdtsc(){__asm__ __volatile__("rdtsc");}	
void initrandbytes(){
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	int cpucount=sysinfo.dwNumberOfProcessors;
	int h=0,i1;
	for(i1=0;i1<cpucount;i1++){
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
double sma(const int period, const int price, const int shift,int tcurbar){
	double sum=0.0,tmp;
	if(price==PRICE_CLOSE){
		for(int i=0;i<period;i++)
		{
			sum+=testermetadata->close[tcurbar-(i+shift)];
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}else
	if(price==PRICE_MEDIAN){
		for(int i=0;i<period;i++)
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
		for(int i=0;i<period;i++)
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
	while(k>=0)
	{
		price=(testermetadata->high[tcurbar-(k+shift)]+testermetadata->low[tcurbar-(k+shift)]+testermetadata->close[tcurbar-(k+shift)])/3.0;
		sum+=fabs(price-MovBuffer);
		k--;
	}
	DevBuffer = sum*mul;
	price =(testermetadata->high[tcurbar-(shift)]+testermetadata->low[tcurbar-(shift)]+testermetadata->close[tcurbar-(shift)])/3.0;
	RelBuffer=price-MovBuffer;
	if(DevBuffer==0.0)CCIBuffer=0.0;else CCIBuffer = RelBuffer / DevBuffer;
	return(CCIBuffer);
}
double loadHST(const char* symbol,const char* tf){
	FILE *hFile;
	char* membuf = new char[2];
	membuf = (char*)realloc(membuf,bars*60);
	
	char* path = new char[2];
	path = (char*)realloc(path,500);memset(path,0,500);
	lstrcat(path,pathHST);lstrcat(path,symbol);lstrcat(path,tf);lstrcat(path,".hst");
	
	int i,i1=0,testerdigits;double point;
	hFile = fopen(path, "rb");
	if(!(!hFile)){
			fseek(hFile,0,SEEK_END);
			int dwFileSize = ftell(hFile);
			
			if(dwFileSize>=148+60){
				i=0;
				fseek(hFile,dwFileSize-60*bars,SEEK_SET);
				fread(membuf,60*bars,1,hFile);
				while(i1<bars){
					memcpy(&testermetadata->ctm[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->open[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->high[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->low[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->close[i1],&membuf[i],8);i+=8;
					memcpy(&testermetadata->volume[i1],&membuf[i],8);i+=20;
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
	
	if((tmp3>(tmp3_2*1.2))&&(tmp3>(tmp3_3*1.3))){
		double tmp4=fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar));
		double tmp5=fabs(icci(1,period_ma_fast, period_ma_slow, cci_period,tcurbar));		
		if(tmp4<=tmp5)
		if(tmp3>fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar)))
			for(int i1=3;i1<200;i1++){
				tmp1=fabs(icci(i1,period_ma_fast, period_ma_slow, cci_period,tcurbar));
				if(prevtmp1<tmp1) return (i1*tmp2);
				prevtmp1=tmp1;
			}
	}
	return 0;	
}
//tresults* 
void testerstart(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, tresults &result){
	int openorder=-1,openorderclosed=1,timeout=(int)(ctimeout/tf/60/2);
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0;
	double openorderprice;
	int tcurbar;
	//tresults& result = *((tresults*)result1);
	//tresults* result = new tresults[1];
	for(int i=250;i<bars;i++){
		if((openorder>=0)&&(openorderclosed==0)){
			int profitorder = (int)((testermetadata->close[i]-openorderprice)/point);
			if( ((profitorder>0)&&(openorder==OP_BUY)) || ((profitorder<0)&&(openorder==OP_SELL)) )profitcntorders++;
			if((profitorder>0)&&(openorder==OP_BUY))profitcntpoints+=profitorder;
			if((profitorder<0)&&(openorder==OP_SELL))profitcntpoints+=abs(profitorder);
			//if((profitorder<0)&&(openorder==OP_BUY))profitcntpoints-=abs(profitorder);
			//if((profitorder>0)&&(openorder==OP_SELL))profitcntpoints-=abs(profitorder);
			if( ((profitorder<0)&&(openorder==OP_BUY)) || ((profitorder>0)&&(openorder==OP_SELL)) )notprofitcntorders++;
			if(profitcntpoints<14){
				result.profitcntpoints=-1;
				return;
			}
			openorderclosed=1;
			continue;
		}
		tcurbar=i;
		if((openorder==0)||(openorder==-1)){
			int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
			if((abs(signal)>19) && (signal>0)){
				openorder=OP_SELL;
				openorderclosed=0;
				openorderprice=testermetadata->open[i];
				i+=timeout;
			}
		}else
		if((openorder==1)||(openorder==-1)){
			int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
			if((abs(signal)>19) && (signal<0)){
				openorder=OP_BUY;
				openorderclosed=0;
				openorderprice=testermetadata->open[i];
				i+=timeout;
			}
		}		
		
		
	}
	
	result.profitcntpoints=profitcntpoints;
	result.period_ma_fast=period_ma_fast;
	result.period_ma_slow=period_ma_slow;
	result.cci_period=cci_period;
	result.profitcntorders=profitcntorders;
	result.notprofitcntorders=notprofitcntorders;
	
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
DWORD WINAPI myThread(LPVOID lpParameter){
	tthread& thread = *((tthread*)lpParameter);
	
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0,period_ma_fast=0,period_ma_slow=0,cci_period=0;
	tresults& testerresult = thread.tmpresults;
	
	int tf=thread.tf;int timeout=thread.timeout;
	for(int i=0;i<thread.randcycles;i++){
		int t1=2,t2=1, t3=0;
		while(t1>=t2){t1=rand(8,24,thread.id);t2=rand(24,222,thread.id);t3=rand(55,222,thread.id);}
		testerstart(tf,thread.point,timeout,t1,t2,t3,testerresult);
		if(testerresult.profitcntpoints!=-1)
		if( ((testerresult.profitcntorders-testerresult.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (testerresult.profitcntorders >4) && (testerresult.profitcntorders>(testerresult.notprofitcntorders*3))){
			profitcntpoints = testerresult.profitcntpoints;
			period_ma_fast = testerresult.period_ma_fast;
			period_ma_slow = testerresult.period_ma_slow;
			cci_period = testerresult.cci_period;
			profitcntorders = testerresult.profitcntorders;
			notprofitcntorders = testerresult.notprofitcntorders;
			
		}
	}
	thread.results.profitcntpoints = profitcntpoints;
	thread.results.period_ma_fast = period_ma_fast;
	thread.results.period_ma_slow = period_ma_slow;
	thread.results.cci_period = cci_period;
	thread.results.profitcntorders = profitcntorders;
	thread.results.notprofitcntorders = notprofitcntorders;
	thread.done = true;
	
	return 0;
}
const char* testertest(const char* ctf,double point, const char* ctimeout, const char* takeprofit) {
	static char itemconfig[200]="";
	memset(itemconfig,0,200);
	int tf=strToInt(ctf);int timeout=strToInt(ctimeout);
	int tp;

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	treadcount=sysinfo.dwNumberOfProcessors;
	//DWORD threadid = GetCurrentThreadId();
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
	//	threads[i].tmpresults = new tresults;
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
	
	int profitcntpoints=0,profitcntorders=0,notprofitcntorders=0,period_ma_fast=0,period_ma_slow=0,cci_period=0;
	for(int i=0;i<treadcount;i++){
		if( ((threads[i].results.profitcntorders-threads[i].results.notprofitcntorders)>(profitcntorders-notprofitcntorders)) && (threads[i].results.profitcntorders >4) && (threads[i].results.profitcntorders>(threads[i].results.notprofitcntorders*3))){
		//if(testerresult->profitcntorders > profitcntorders){
			profitcntpoints = threads[i].results.profitcntpoints;
			period_ma_fast = threads[i].results.period_ma_fast;
			period_ma_slow = threads[i].results.period_ma_slow;
			cci_period = threads[i].results.cci_period;
			profitcntorders = threads[i].results.profitcntorders;
			notprofitcntorders = threads[i].results.notprofitcntorders;
		}
	}

	tp = strToInt(takeprofit);
	if(profitcntorders>0)tp = (int)(profitcntpoints/profitcntorders/1.5);
	lstrcat(itemconfig,ctf);lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_fast));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(period_ma_slow));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(cci_period));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(tp));lstrcat(itemconfig," ");//takeprofit
	lstrcat(itemconfig,ctimeout);lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(profitcntpoints));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(notprofitcntorders));lstrcat(itemconfig," ");	
	lstrcat(itemconfig,intToStr(profitcntorders));
	return (const char *)itemconfig;
}
const char* optimize(const char* symbol,const char* tf, const char* timeout, const char* takeprofit) {
	double point = loadHST(symbol,tf);

	return (const char *)testertest(tf,point,timeout,takeprofit);
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
int main(int argc, char *argv[]){
	printf(timeToStr(time(NULL))); printf(" - time start\r\n");
	double title1,dt0=time(NULL);
	rdtsc();
	srand(time(0));
	initrandbytes();

	pathCONFIG = new char[500];memset(pathCONFIG,0,500);lstrcat(pathCONFIG,argv[3]);
	pathHST = new char[500];memset(pathHST,0,500);lstrcat(pathHST,"..\\..\\history\\");lstrcat(pathHST,argv[4]);lstrcat(pathHST,"\\");
	tfdepth = strToInt(argv[2]);bars = strToInt(argv[1]);
	char *stm1;stm1 = (char *)malloc(100000);memset(stm1,0,100000);
	char tf[5];memset(tf,0,5);char timeout[10];memset(timeout,0,10);char takeprofit[10];memset(takeprofit,0,10);
	char optresult[100];memset(optresult,0,100);
	
	testermetadata = new mdata[1];

	ReadConfig();
	
	for(int i1=0;i1<cindex;i1++){
		printf("\r\n");printf(&config[i1][1][0]);printf("\r\n");
		for(int i2=0;i2<strToInt(&config[i1][0][0]);i2++)
		if(i2<tfdepth){
			memset(tf,0,5);memset(timeout,0,10);memset(takeprofit,0,10);memset(optresult,0,100);
			lstrcat(tf,GetElement(&config[i1][i2+2][0],0));
			lstrcat(timeout,GetElement(&config[i1][i2+2][0],5));
			lstrcat(takeprofit,GetElement(&config[i1][i2+2][0],4));
			lstrcat(optresult,optimize(&config[i1][1][0],tf,timeout,takeprofit));
			printf(optresult);printf("\r\n");
			PatchConfig(&config[i1][1][0],optresult);
		}else {
			printf(&config[i1][i2+2][0]);printf("\r\n");
		}
	}
	SaveConfig();
/* 	lstrcat(stm1,gmtimeToStr(testermetadata->ctm[bars-1]));lstrcat(stm1,"\r\n");
	lstrcat(stm1,doubleToStr(testermetadata->open[bars-1],4));lstrcat(stm1,"\r\n");
	lstrcat(stm1,doubleToStr(testermetadata->high[bars-1],4));lstrcat(stm1,"\r\n");
	lstrcat(stm1,doubleToStr(testermetadata->low[bars-1],4));lstrcat(stm1,"\r\n");
	lstrcat(stm1,doubleToStr(testermetadata->close[bars-1],4));lstrcat(stm1,"\r\n"); */
	//lstrcat(stm1,intToStr(testerdigits));lstrcat(stm1,"\r\n");
	lstrcat(stm1,"\r\n");

	title1=time(NULL);title1-=dt0;title1/=60;
	lstrcat(stm1,doubleToStr(title1,1));
	lstrcat(stm1," minutes used");
	printf(stm1);
	//PlaySoundA((LPCSTR) "opcomplete.wav", NULL, SND_FILENAME | SND_ASYNC);
	MessageBeep(MB_OK);
    free(stm1);
	free(pathHST);
	free(pathCONFIG);
	delete[] testermetadata;
	
}