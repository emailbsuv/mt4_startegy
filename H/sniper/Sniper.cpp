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

const char* pathCONFIG = "C:\\Users\\Bogdan\\AppData\\Roaming\\MetaQuotes\\Terminal\\CCD68BFB06049A8615C607C3F6AD69B7\\tester\\files\\contests1.txt";
const char* pathRESULTS = "C:\\Users\\Bogdan\\AppData\\Roaming\\MetaQuotes\\Terminal\\CCD68BFB06049A8615C607C3F6AD69B7\\tester\\files\\results1.txt";


int bars = 1070;//1070//1500
int randcycles=50;
int tfdeptf=5;
const char* pathHST = "C:\\Users\\Bogdan\\AppData\\Roaming\\MetaQuotes\\Terminal\\CCD68BFB06049A8615C607C3F6AD69B7\\history\\InstaForex-1Demo.com\\";

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
	int openorder;
	double orderprofit;
	double orderopentime;
	double nullbartime;
};

int testercurbar;
int randptr=0,randbytes[257];

char config[200][9][100];int cindex=0;
char t1config[1000][100];

inline int rdtsc(){__asm__ __volatile__("rdtsc");}	
void initrandbytes(){
	int h=0;
	while(h==0){
		for(int z1=0;z1<51;z1++){for(int z=0;z<256;z++){randbytes[z]^=((rand()<<1) % 256)&255;}}
		for(int z1=0;z1<5;z1++)for(int z=0;z<256;z++)randbytes[z]^=(randbytes[(z+1)%256]>>1)&255;
		for(int z=0;z<256;z++)if (randbytes[z]>127)h=1;
	}
	randptr=0;
}
int rand(int min, int max){
	int h=randbytes[randptr];
	if(h<min)h+=min;
	if(h>max)h=h%max;
	if(h<min)h+=min;
	randbytes[randptr]^=randbytes[(randptr+1)%256];
	randptr=(randptr+1)%256;	
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
	double tmp1,tmp2,tmp3,prevtmp1=icci(2,period_ma_fast, period_ma_slow, cci_period,tcurbar);tmp3=prevtmp1;
	double tmp4=fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	double tmp5=fabs(icci(1,period_ma_fast, period_ma_slow, cci_period,tcurbar));
	if(tmp3<0)tmp2=-1;else tmp2=1;
    prevtmp1=tmp3=fabs(tmp3);
	
	if(tmp4<=tmp5)
	if(tmp3>fabs(icci(0,period_ma_fast, period_ma_slow, cci_period,tcurbar)))
		for(int i1=3;i1<200;i1++){
			tmp1=fabs(icci(i1,period_ma_fast, period_ma_slow, cci_period,tcurbar));
			if(prevtmp1<tmp1) return (i1*tmp2);
			prevtmp1=tmp1;
		}
	return 0;
}
tresults testerstart(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, int takeprofit){
	int openorder=-1,timeout=(int)(172800/tf/60);
	if(tf>240)timeout*=3;
	double orderprofit=0.0,orderopentime=0,nullbartime=0,orderopenprice=0.0;
	int tcurbar,i1;
	tresults* result = new tresults[1];
	for(int i=bars-1;i>bars-1-timeout;i--){
		tcurbar=i;
		int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
		if(abs(signal)>20){
			if(signal>0){
				openorder=OP_SELL;
				orderprofit=(testermetadata->open[i]-testermetadata->close[bars-1])/point;
				orderopentime=testermetadata->ctm[i];
				nullbartime=testermetadata->ctm[bars-1];
				i1=i;orderopenprice=testermetadata->open[i];
				break;
			}else
			if(signal<0){
				openorder=OP_BUY;
				orderprofit=(testermetadata->open[i]-testermetadata->close[bars-1])/point;
				orderopentime=testermetadata->ctm[i];
				nullbartime=testermetadata->ctm[bars-1];
				i1=i;orderopenprice=testermetadata->open[i];
				break;
			}
		}
		
	}
	for(int i2=i1;i2<bars;i2++){
		if((openorder==OP_BUY)&&(testermetadata->high[i2]>=(orderopenprice+point*takeprofit)))
		{
			openorder=-1;
			break;
		}
		if((openorder==OP_SELL)&&(testermetadata->low[i2]<=(orderopenprice-point*takeprofit)))
		{
			openorder=-1;
			break;
		}
	}
	result->openorder=openorder;
	result->orderprofit=orderprofit;
	result->orderopentime=orderopentime;
	result->nullbartime=nullbartime;
	
	return *result;
}
const char* testertest(const char* ctf,const char* ma1,const char* ma2,const char* cci1,double point, const char* ctimeout, const char* takeprofit) {
	static char itemconfig[200]="";
	memset(itemconfig,0,200);
	int tf=strToInt(ctf);int timeout=strToInt(ctimeout);
 
 	tresults* testerresult;
	testerresult = new tresults[1];
	testerresult[0] = testerstart(tf,point,timeout,strToInt(ma1),strToInt(ma2),strToInt(cci1),strToInt(takeprofit));
	if((testerresult->openorder==OP_BUY) && (testerresult->orderprofit>0)){
		lstrcat(itemconfig,"BUY ");
		lstrcat(itemconfig,intToStr((int)fabs(testerresult->orderprofit)));
		lstrcat(itemconfig," ");
		lstrcat(itemconfig,gmtimeToStr(testerresult->orderopentime));
		lstrcat(itemconfig," ");
		lstrcat(itemconfig,gmtimeToStr(testerresult->nullbartime));
	}
	if((testerresult->openorder==OP_SELL) && (testerresult->orderprofit<0)){
		lstrcat(itemconfig,"SELL ");
		lstrcat(itemconfig,intToStr((int)fabs(testerresult->orderprofit)));
		lstrcat(itemconfig," ");
		lstrcat(itemconfig,gmtimeToStr(testerresult->orderopentime));
		lstrcat(itemconfig," ");
		lstrcat(itemconfig,gmtimeToStr(testerresult->nullbartime));		
	}
	delete[] testerresult;
	
	return (const char *)itemconfig;
}
const char* sniper(const char* symbol,const char* ma1,const char* ma2,const char* cci1,const char* tf, const char* timeout, const char* takeprofit) {
	double point = loadHST(symbol,tf);

	return (const char *)testertest(tf,ma1,ma2,cci1,point,timeout,takeprofit);
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
char snipertxt[10][200][100];
void InitResults(){
	for(int tf=0; tf<9; tf++){
		for(int i1=0; i1<200; i1++)memset(&snipertxt[tf][i1][0],0,100);
	    lstrcat(&snipertxt[tf][0][0],"0");
	}
}
void PatchConfig(int tf, char* symbol, char* param){
	if(strlen(&param[0])>0){
		int i1,i3;
		if(tf==1)i3=0;if(tf==5)i3=1;if(tf==15)i3=2;if(tf==30)i3=3;if(tf==60)i3=4;if(tf==240)i3=5;
		if(tf==1440)i3=6;if(tf==10080)i3=7;if(tf==43200)i3=8;
		i1 = strToInt(&snipertxt[i3][0][0]);
		memset(&snipertxt[i3][i1+1][0],0,100);
		lstrcat(&snipertxt[i3][i1+1][0],&symbol[0]);
		lstrcat(&snipertxt[i3][i1+1][0]," : ");
		lstrcat(&snipertxt[i3][i1+1][0],&param[0]);
		
		memset(&snipertxt[i3][0][0],0,100);
		lstrcat(&snipertxt[i3][0][0],intToStr(i1+1));
	}
}
void SaveConfig(){
	char* membuf = new char[2];membuf = (char*)realloc(membuf,50000);memset(membuf,0,50000);
	FILE *os;os= fopen(pathRESULTS,"wb");

	int tf,i3;
	
	for(tf=0; tf<9; tf++)
	{
		if(strlen(&snipertxt[tf][0][0])>0){
	     if(tf==0)i3=1;if(tf==1)i3=5;if(tf==2)i3=15;if(tf==3)i3=30;if(tf==4)i3=60;if(tf==5)i3=240;
	     if(tf==6)i3=1440;if(tf==7)i3=10080;if(tf==8)i3=43200;
		 if(strToInt(&snipertxt[tf][0][0])>0)
		  {lstrcat(membuf,"[ Timeframe ");lstrcat(membuf,intToStr(i3));lstrcat(membuf," ]\r\n");}
		 for(int i2=0; i2<strToInt(&snipertxt[tf][0][0]); i2++)
		  {
		   lstrcat(membuf,&snipertxt[tf][i2+1][0]);lstrcat(membuf,"\r\n");
		   if(i2==(strToInt(&snipertxt[tf][0][0])-1))
			  lstrcat(membuf,"\r\n");
		  }
		}
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

	char *stm1;stm1 = (char *)malloc(100000);memset(stm1,0,100000);
	char tf[5];memset(tf,0,5);char timeout[10];memset(timeout,0,10);char takeprofit[10];memset(takeprofit,0,10);
	char ma1[4],ma2[4],cci1[4];
	char optresult[100];memset(optresult,0,100);
	
	testermetadata = new mdata[1];

	ReadConfig();
	InitResults();
	
	for(int i1=0;i1<cindex;i1++){
		for(int i2=0;i2<strToInt(&config[i1][0][0]);i2++)
		if(strToInt(GetElement(&config[i1][i2+2][0],1))>0){
			memset(tf,0,5);memset(timeout,0,10);memset(takeprofit,0,10);memset(optresult,0,100);
			memset(ma1,0,4);memset(ma2,0,4);memset(cci1,0,4);
			lstrcat(tf,GetElement(&config[i1][i2+2][0],0));
			lstrcat(ma1,GetElement(&config[i1][i2+2][0],1));
			lstrcat(ma2,GetElement(&config[i1][i2+2][0],2));
			lstrcat(cci1,GetElement(&config[i1][i2+2][0],3));
			lstrcat(timeout,GetElement(&config[i1][i2+2][0],5));
			lstrcat(takeprofit,GetElement(&config[i1][i2+2][0],4));
			lstrcat(optresult,sniper(&config[i1][1][0],ma1,ma2,cci1,tf,timeout,takeprofit));
			if(strlen(optresult)>0){printf(tf);printf(" ");printf(&config[i1][1][0]);printf(" ");printf(optresult);printf("\r\n");}
			PatchConfig(strToInt(tf),&config[i1][1][0],optresult);
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
	delete[] testermetadata;
	
}