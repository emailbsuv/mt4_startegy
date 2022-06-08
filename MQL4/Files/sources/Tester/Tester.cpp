#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <tchar.h>
#include "winsock2.h"
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

char* pathCONFIG;
char* pathRESULTS;
char* ident;


int bars,ibars;
int randcycles=50;
int timeshift;
char* pathHST;
double mulsl=1,multp=1;

struct mdata{
	long int ctm[50000];
	double open[50000];
	double low[50000];
	double high[50000];
	double close[50000];
	double volume[50000];
	int spread[50000];
};
mdata* testermetadata;

struct tresults{
	int totalloss;
	int totalprofit;
	int totalprofitorders;
	int totalnotprofitorders;
	int midtimeout;
};

int testercurbar;
int randptr=0,randbytes[257];

char config[200][9][100];int cindex=0;
char t1config[1000][100];
char totalresults[5000];

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
			//int bars=(dwFileSize-148)/60; if(bars>=ibars)bars=ibars;
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
/* 	double tmp1,tmp2,tmp3,prevtmp1=icci(2,period_ma_fast, period_ma_slow, cci_period,tcurbar);tmp3=prevtmp1;
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
void testerstart(int tf, double point, int ctimeout, int period_ma_fast, int period_ma_slow, int cci_period, int period_ma_fast2, int period_ma_slow2, int cci_period2,int tpsell,int slsell,int slbuy,int tpbuy, int sprd, tresults &result){
	double orderopenpricebuy,orderopenpricesell;
	int orderopenedsell=0,orderopenedbuy=0;
	int orderopentimebuy,orderopentimesell;
	int totalloss=0,totalprofit=0,totalprofitorders=0,totalnotprofitorders=0,totaltimeout=0;
	int tcurbar,i;

	for(i=bars-timeshift;i<bars;i++){
		tcurbar=i;
		
		if(orderopenedsell==1){
			if( testermetadata->high[i] >= (orderopenpricesell+point*slsell+point*sprd) ){
				orderopenedsell=0;
				totalloss+=slsell;
				totalnotprofitorders++;
				totaltimeout+=i-orderopentimesell;
			}
			if( testermetadata->low[i] <= (orderopenpricesell-point*tpsell-point*sprd) ){
				orderopenedsell=0;
				totalprofit+=tpsell;
				totalprofitorders++;
				totaltimeout+=i-orderopentimesell;
			}
		}
		if(orderopenedbuy==1){
			if( testermetadata->high[i] >= (orderopenpricebuy+point*tpbuy+point*sprd) ){
				orderopenedbuy=0;
				totalprofit+=tpbuy;
				totalprofitorders++;
				totaltimeout+=i-orderopentimebuy;
			}
			if( testermetadata->low[i] <= (orderopenpricebuy-point*slbuy-point*sprd) ){
				orderopenedbuy=0;
				totalloss+=slbuy;
				totalnotprofitorders++;
				totaltimeout+=i-orderopentimebuy;
			}
		}		
		
		int signal = DeltaMasLength(period_ma_fast, period_ma_slow, cci_period,tcurbar);
		int signal2 = DeltaMasLength(period_ma_fast2, period_ma_slow2, cci_period2,tcurbar);
		if(period_ma_fast2>0)
		if(abs(signal2)>19){
			//if((signal>0)&&(orderopenedsell==0)&&(iLowest(19,tcurbar)>10)&&(iHighest(19,tcurbar)<5)) {
			if((signal2>0)&&(orderopenedsell==0)){
				orderopenedsell=1;//OP_SELL;
				orderopentimesell=i;
				orderopenpricesell=testermetadata->open[i];
				//break;
			}
		}
		if(period_ma_fast>0)
		if(abs(signal)>19){
			//if((signal<0)&&(orderopenedbuy==0)&&(iLowest(19,tcurbar)<5)&&(iHighest(19,tcurbar)>10)) {
			if((signal<0)&&(orderopenedbuy==0)){
				orderopenedbuy=1;//OP_BUY;
				orderopentimebuy=i;
				orderopenpricebuy=testermetadata->open[i];
				//break;
			}
		}
		
	}

	i=bars-1;
	
	if(orderopenedsell==1){
		if( testermetadata->high[i] >= (orderopenpricesell) ){
			orderopenedsell=0;
			totalloss+=abs((testermetadata->high[i]-orderopenpricesell)/point);
			totalnotprofitorders++;
			totaltimeout+=i-orderopentimesell;
		}
		if( testermetadata->low[i] <= (orderopenpricesell) ){
			orderopenedsell=0;
			totalprofit+=abs((orderopenpricesell-testermetadata->low[i])/point);
			totalprofitorders++;
			totaltimeout+=i-orderopentimesell;
		}
	}
	if(orderopenedbuy==1){
		if( testermetadata->high[i] >= (orderopenpricebuy) ){
			orderopenedbuy=0;
			totalprofit+=abs((testermetadata->high[i]-orderopenpricebuy)/point);
			totalprofitorders++;
			totaltimeout+=i-orderopentimebuy;
		}
		if( testermetadata->low[i] <= (orderopenpricebuy) ){
			orderopenedbuy=0;
			totalloss+=abs((orderopenpricebuy-testermetadata->low[i])/point);
			totalnotprofitorders++;
			totaltimeout+=i-orderopentimebuy;
		}
	}	
	
	result.totalloss=totalloss;
	result.totalprofit=totalprofit;
	result.totalprofitorders=totalprofitorders;
	result.totalnotprofitorders=totalnotprofitorders;
	if((totalprofitorders+totalnotprofitorders)>0)
		result.midtimeout=(int)(totaltimeout*tf/(totalprofitorders+totalnotprofitorders));else result.midtimeout=0;

	
	return ;
}
tresults testerresult1;
const char* testertest(const char* ctf,int ma1,int ma2,int cci1,int ma1_2,int ma2_2,int cci1_2,double point, const char* ctimeout,int tpsell,int slsell,int slbuy,int tpbuy,int sprd) {
	static char itemconfig[200]="";
	memset(itemconfig,0,200);
	int tf=strToInt(ctf);int timeout=strToInt(ctimeout);
 
 	
	tresults& testerresult = testerresult1;

	testerstart(tf,point,timeout,ma1,ma2,cci1,ma1_2,ma2_2,cci1_2,tpsell,slsell,slbuy,tpbuy,sprd,testerresult);
	
	lstrcat(itemconfig,intToStr(testerresult1.totalloss));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(testerresult1.totalprofit));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(testerresult1.totalprofitorders));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(testerresult1.totalnotprofitorders));lstrcat(itemconfig," ");
	lstrcat(itemconfig,intToStr(testerresult1.midtimeout));
	

	
	return (const char *)itemconfig;
}
const char* sniper(const char* symbol,int ma1,int ma2,int cci1,int ma1_2,int ma2_2,int cci1_2,const char* tf, const char* timeout,int tpsell,int slsell,int slbuy,int tpbuy,int sprd) {
	double point = loadHST(symbol,tf);

	return (const char *)testertest(tf,ma1,ma2,cci1,ma1_2,ma2_2,cci1_2,point,timeout,tpsell,slsell,slbuy,tpbuy,sprd);
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
char snipertxt[10][1200][100];
void InitResults(){
	for(int tf=0; tf<9; tf++){
		for(int i1=0; i1<1200; i1++)memset(&snipertxt[tf][i1][0],0,100);
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
		lstrcat(&snipertxt[i3][i1+1][0]," ");
		lstrcat(&snipertxt[i3][i1+1][0],&param[0]);
		
		memset(&snipertxt[i3][0][0],0,100);
		lstrcat(&snipertxt[i3][0][0],intToStr(i1+1));
	}
}
char* SaveResults(){
	char* membuf = new char[2];membuf = (char*)realloc(membuf,50000);memset(membuf,0,50000);
	memset(totalresults,0,5000);
	FILE *os;os= fopen(pathRESULTS,"wb");

	int tf,i3,i4=0;
	int totalloss=0,totalprofit=0,totalprofitorders=0,totalnotprofitorders=0,midtimeout=0;
	int Ttotalloss[9],Ttotalprofit[9],Ttotalprofitorders[9],Ttotalnotprofitorders[9],Tmidtimeout[9];
	int Ytotalloss=0,Ytotalprofit=0,Ytotalprofitorders=0,Ytotalnotprofitorders=0,Ymidtimeout=0;
	for(int i=0;i<9;i++){
		Ttotalloss[i]=0;
		Ttotalprofit[i]=0;
		Ttotalprofitorders[i]=0;
		Ttotalnotprofitorders[i]=0;
		Tmidtimeout[i]=0;
	}
	double maxtf=0.0;
	for(tf=0; tf<9; tf++)
	{
		if(strlen(&snipertxt[tf][0][0])>0){
	     if(tf==0)i3=1;if(tf==1)i3=5;if(tf==2)i3=15;if(tf==3)i3=30;if(tf==4)i3=60;if(tf==5)i3=240;
	     if(tf==6)i3=1440;if(tf==7)i3=10080;if(tf==8)i3=43200;
		 for(int i2=0; i2<strToInt(&snipertxt[tf][0][0]); i2++)
		  {
		   totalloss+= strToInt( GetElement(&snipertxt[tf][i2+1][0],1) );
		   totalprofit+= strToInt( GetElement(&snipertxt[tf][i2+1][0],2) );
		   totalprofitorders+= strToInt( GetElement(&snipertxt[tf][i2+1][0],3) );
		   totalnotprofitorders+= strToInt( GetElement(&snipertxt[tf][i2+1][0],4) );
		   midtimeout+= strToInt( GetElement(&snipertxt[tf][i2+1][0],5) );
		   
			Ttotalloss[tf]+= strToInt( GetElement(&snipertxt[tf][i2+1][0],1) );
			Ttotalprofit[tf]+= strToInt( GetElement(&snipertxt[tf][i2+1][0],2) );
			Ttotalprofitorders[tf]+= strToInt( GetElement(&snipertxt[tf][i2+1][0],3) );
			Ttotalnotprofitorders[tf]+= strToInt( GetElement(&snipertxt[tf][i2+1][0],4) );
			Tmidtimeout[tf]+= strToInt( GetElement(&snipertxt[tf][i2+1][0],5) );
		   i4++;
		   if(strToInt(&snipertxt[tf][0][0])>0)if(maxtf<(double)i3)maxtf=(double)i3;
		  }
		}
		
	}
	for(tf=0; tf<9; tf++)
	{
	     if(tf==0)i3=1;if(tf==1)i3=5;if(tf==2)i3=15;if(tf==3)i3=30;if(tf==4)i3=60;if(tf==5)i3=240;
	     if(tf==6)i3=1440;if(tf==7)i3=10080;if(tf==8)i3=43200;
			Ytotalloss+=Ttotalloss[tf]*(maxtf/i3);
			Ytotalprofit+=Ttotalprofit[tf]*(maxtf/i3);
			Ytotalprofitorders+=Ttotalprofitorders[tf]*(maxtf/i3);
			Ytotalnotprofitorders+=Ttotalnotprofitorders[tf]*(maxtf/i3);
			Ymidtimeout+=Tmidtimeout[tf]*(maxtf/i3);
	}
	maxtf=(double)timeshift*maxtf;
	if((Ytotalprofitorders+Ytotalnotprofitorders)>0)midtimeout = (int)(Ymidtimeout/(Ytotalprofitorders+Ytotalnotprofitorders)); else midtimeout=0;
	double ordersperday,pipsperday;
	ordersperday=((double)(Ytotalprofitorders+Ytotalnotprofitorders)/(double)maxtf)*(double)1440.0;
	pipsperday=((double)(Ytotalprofit-Ytotalloss)/(double)maxtf)*(double)1440.0;
	lstrcat(membuf,ident);lstrcat(membuf,"\r\n");
	lstrcat(membuf,"koef SL : "); lstrcat(membuf,doubleToStr(mulsl,3));
	lstrcat(membuf,", koef TP : "); lstrcat(membuf,doubleToStr(multp,3));lstrcat(membuf,"\r\n");
	lstrcat(membuf,"bars shift : "); lstrcat(membuf,intToStr(timeshift));lstrcat(membuf,"\r\n");
	lstrcat(membuf,"totalloss : "); lstrcat(membuf,intToStr(totalloss));lstrcat(membuf," pips\r\n");
	lstrcat(membuf,"totalprofit : "); lstrcat(membuf,intToStr(totalprofit));lstrcat(membuf," pips\r\n");
	lstrcat(membuf,"totalprofitorders : "); lstrcat(membuf,intToStr(totalprofitorders));lstrcat(membuf,"\r\n");
	lstrcat(membuf,"totalnotprofitorders : "); lstrcat(membuf,intToStr(totalnotprofitorders));lstrcat(membuf,"\r\n");
	lstrcat(membuf,"frequency : "); lstrcat(membuf,doubleToStr(ordersperday,2));lstrcat(membuf," orders per day\r\n");
	lstrcat(membuf,"clear profit : "); lstrcat(membuf,doubleToStr(pipsperday,2));lstrcat(membuf," pips per day\r\n");
	lstrcat(membuf,"middle order lifetime : "); lstrcat(membuf,intToStr(midtimeout));lstrcat(membuf," minutes\r\n");
	
	lstrcat(&totalresults[0],"\r\n\r\n");lstrcat(&totalresults[0],membuf);
	
	lstrcat(membuf,"\r\n\r\n");
	
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



   FILE* fd;
   char *src_str;src_str = (char *)malloc(40000);memset(src_str,0,40000);
   fd = fopen(pathCONFIG, "rb");
   int src_size = fread(src_str, 40000,1,fd);
   fclose(fd);
   char *buff;buff = (char *)malloc(40000);memset(buff,0,40000);
   lstrcat(buff,"1111222233334444");
   lstrcat(buff,membuf);
   lstrcat(buff,src_str);
   int t;
   t=strlen(membuf);memcpy(&buff[0],&t,4);
   t=totalprofit;memcpy(&buff[4],&t,4);
   t=totalloss;memcpy(&buff[8],&t,4);
   t=strlen(src_str);memcpy(&buff[12],&t,4);
   int bufflen = 16+strlen(membuf)+strlen(src_str);
   //printf(&buff[16]);return 0;
   
/*    WSADATA ws;
   WSAStartup (MAKEWORD( 1, 1 ), &ws);

   SOCKET s;
   s = socket (AF_INET, SOCK_STREAM, 0);
   sockaddr_in sock_addr;
   ZeroMemory(&sock_addr, sizeof (sock_addr));
   sock_addr.sin_family = AF_INET;
   sock_addr.sin_addr.s_addr = inet_addr ("34.65.157.210");//192.168.127.133");
   sock_addr.sin_port = htons (6666);
   connect (s, (sockaddr *) &sock_addr, sizeof (sock_addr) );

   //send (s, (char* ) & buff, bufflen, 0 );
   //send (s,reinterpret_cast<const unsigned char*>(&buff), bufflen, 0 );
   send (s, buff, bufflen, 0 );
   
   closesocket (s);
   WSACleanup(); */
   
   free(buff);
   free(src_str);	
	
	

	
	int tmp0=strlen(membuf);
	fwrite(membuf,tmp0,1,os);
	fclose(os);
	delete[] membuf;
    return &totalresults[0];	
}

int main(int argc, char *argv[]){

	printf(timeToStr(time(NULL))); printf(" - time start\r\n");
	double title1,dt0=time(NULL);
	rdtsc();
	srand(time(0));
	initrandbytes();

    pathCONFIG = new char[500];memset(pathCONFIG,0,500);lstrcat(pathCONFIG,argv[3]);
	pathRESULTS = new char[500];memset(pathRESULTS,0,500);lstrcat(pathRESULTS,pathCONFIG);lstrcat(pathRESULTS,"-RESULTS.txt");
	ident = new char[500];memset(ident,0,500);lstrcat(ident,argv[8]);
	char* terminal = new char[500];memset(terminal,0,500);lstrcat(terminal,argv[9]);
	
	pathHST = new char[500];memset(pathHST,0,500);lstrcat(pathHST,"..\\..\\history\\");lstrcat(pathHST,argv[7]);lstrcat(pathHST,"\\");
	timeshift = strToInt(argv[4]);bars = strToInt(argv[1]);
	if(timeshift==0){timeshift=1200; /*bars-=400;*/}
	mulsl = strtod(argv[5], NULL);
	multp = strtod(argv[6], NULL);
	char *stm1;stm1 = (char *)malloc(100000);memset(stm1,0,100000);
	char tf[15];memset(tf,0,15);char timeout[30];memset(timeout,0,30);
	int tpbuy,tpsell,slsell,slbuy;
	int ma1,ma2,cci1,ma1_2,ma2_2,cci1_2,sprd;
	char optresult[100];memset(optresult,0,100);
	
	testermetadata = new mdata[1];

	ReadConfig();
	InitResults();
	
	for(int i1=0;i1<cindex;i1++){
		for(int i2=0;i2<strToInt(&config[i1][0][0]);i2++)
		if((strToInt(GetElement(&config[i1][i2+2][0],1))>0) || (strToInt(GetElement(&config[i1][i2+2][0],12))>0)){
			memset(tf,0,15);memset(timeout,0,30);memset(optresult,0,100);
			tpbuy=strToInt(GetElement(&config[i1][i2+2][0],11));tpbuy*=multp;if(tpbuy<1)tpbuy=1;
			tpsell=strToInt(GetElement(&config[i1][i2+2][0],4));tpsell*=multp;if(tpsell<1)tpsell=1;
			slsell=strToInt(GetElement(&config[i1][i2+2][0],9));slsell*=mulsl;if(slsell<1)slsell=1;
			slbuy=strToInt(GetElement(&config[i1][i2+2][0],10));slbuy*=mulsl;if(slbuy<1)slbuy=1;
			
			lstrcat(tf,GetElement(&config[i1][i2+2][0],0));
			ma1=strToInt(GetElement(&config[i1][i2+2][0],1));
			ma2=strToInt(GetElement(&config[i1][i2+2][0],2));
			cci1=strToInt(GetElement(&config[i1][i2+2][0],3));
			sprd=strToInt(GetElement(&config[i1][i2+2][0],6));
			ma1_2=strToInt(GetElement(&config[i1][i2+2][0],12));
			ma2_2=strToInt(GetElement(&config[i1][i2+2][0],13));
			cci1_2=strToInt(GetElement(&config[i1][i2+2][0],14));
			lstrcat(timeout,GetElement(&config[i1][i2+2][0],5));
			lstrcat(optresult,sniper(&config[i1][1][0],ma1,ma2,cci1,ma1_2,ma2_2,cci1_2,tf,timeout,tpsell,slsell,slbuy,tpbuy,sprd));
			
			if(strlen(optresult)>0){printf(tf);printf(" ");printf(&config[i1][1][0]);printf(" ");printf(optresult);printf("\r\n");}
			PatchConfig(strToInt(tf),&config[i1][1][0],optresult);
		}
	}
	printf(SaveResults());
	lstrcat(stm1,"\r\n");

	title1=time(NULL);title1-=dt0;title1/=60;
	lstrcat(stm1,doubleToStr(title1,1));
	lstrcat(stm1," minutes used, results saved in ");lstrcat(stm1,pathRESULTS);lstrcat(stm1,"\r\n");
	printf(stm1);
	MessageBeep(MB_OK);
    free(stm1);
	delete[] testermetadata;
	
	_execlp(terminal, "cmd.exe", "/c", "", "", "", nullptr);
	//_execlp(terminal, terminal, "/portable", "", "", "", nullptr);
	
}