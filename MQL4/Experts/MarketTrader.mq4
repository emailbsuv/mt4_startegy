#property copyright "https://t.me/markettraderoptimizer"
#property link      "https://t.me/markettraderoptimizer"
#property description "v0.8952-mamacci"

#import "user32.dll"
  int GetParent(int hWnd);
  int PostMessageA(int hWnd,int Msg,int wParam,int lParam);
#import
#import "shell32.dll"
 int ShellExecuteW(int hWnd, string Operation, string FileName, string Parameters, string Directory, int ShowCmd);
#import
#import "kernel32.dll"
 int GetModuleFileNameW(int hWnd, string &Operation,int len);
#import

input bool TradeSundaySaturday=true;
input double Lot=0.01;

string vers="v0.8952-mamacci ";
ENUM_ACCOUNT_TRADE_MODE account_type=(ENUM_ACCOUNT_TRADE_MODE)AccountInfoInteger(ACCOUNT_TRADE_MODE); 

int lastorder,firststart;
string config[200][9];
int cindex,cindex1=0,cindex2=1;
double prevbar;
double mulsl,multp;
string contests;
int opttimeleft;
int timer1=2,timer1limit1=2;
int timer2=300;
bool barsupdated;
int bars;

string TimetoStr(){
   int h = (int)(opttimeleft/60/60);
   int m = (int)((opttimeleft-h*60*60)/60);
   int s = opttimeleft-h*60*60-m*60;
   return (h+"час. "+m+"мин. "+s+"сек.");
}
string acctype(){

   string trade_mode; 
   switch(account_type) 
     { 
      case  ACCOUNT_TRADE_MODE_REAL: 
         trade_mode="REAL "; 
         break; 
      default: 
         trade_mode="DEMO "; 
         break; 
     } 
     return trade_mode;
}
int intotf(int i1){

   if(i1==0)return (15);
   if(i1==1)return (30);
   if(i1==2)return (60);
   if(i1==3)return (240);
   if(i1==4)return (1440);

   //if(i1==0)return (60);
   //if(i1==1)return (240);
   //if(i1==2)return (1440);   
   

}
int OnInit()
  {
  EventSetTimer(1);
  if(!FileIsExist("settings.txt",0)){
     string terminal="ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
     GetModuleFileNameW(NULL,terminal,255);

     int filehandle2;
     filehandle2=FileOpen("settings.txt",FILE_WRITE|FILE_TXT);FileSeek(filehandle2,0,SEEK_SET);FileWriteString(filehandle2,"1500 0 contests.txt 0 4.75 0.25 \""+AccountServer()+"\" \""+AccountName()+"("+acctype()+vers+AccountCompany()+")\" \""+terminal+"\"");FileClose(filehandle2);
     filehandle2=FileOpen("contests.txt",FILE_WRITE|FILE_TXT);
     FileSeek(filehandle2,0,SEEK_SET);
     for(int u1=0;u1<SymbolsTotal(true);u1++){
     FileWriteString(filehandle2,SymbolName(u1,true)+"\r\n");
        for(int u2=0;u2<5;u2++){
           FileWriteString(filehandle2,intotf(u2)+" 0 0 0 0 0 "+MarketInfo(SymbolName(u1,true),MODE_SPREAD)+" 0 0 0 0 0 0 0 0");
           if(u1!=SymbolsTotal(true)-1)FileWriteString(filehandle2,"\r\n");
           else if(u2!=4)FileWriteString(filehandle2,"\r\n");
        }
        if(u1!=SymbolsTotal(true)-1)FileWriteString(filehandle2,"\r\n");
     }
     FileClose(filehandle2);
  }

   int filehandle;
   string str1;
   prevbar=iOpen(NULL,0,0);
   firststart=1;
   filehandle=FileOpen("settings.txt",FILE_READ|FILE_TXT);
   FileSeek(filehandle,0,SEEK_SET);
   str1=FileReadString(filehandle);
   FileClose(filehandle);
   bars =StringToInteger(GetElement(str1,0));
   opttimeleft =StringToInteger(GetElement(str1,1));
   if((opttimeleft-TimeGMT())<=(60*5))opttimeleft=60*2;else opttimeleft-=TimeGMT();
   mulsl=StringToDouble(GetElement(str1,4));
   multp=StringToDouble(GetElement(str1,5));
   contests = GetElement(str1,2);
   filehandle=FileOpen(contests,FILE_READ|FILE_TXT);
   FileSeek(filehandle,0,SEEK_SET);
   string tmp01;
   int tmp02=0;
   if(filehandle!=INVALID_HANDLE)
     {
      while(tmp02==0)
        {
         if(!FileIsEnding(filehandle))
           {
            config[cindex1][cindex2]=FileReadString(filehandle);
            cindex2++;
            config[cindex1][0]="0";
            tmp01="1";
            while(StringLen(tmp01)>0)
              {
               tmp01=FileReadString(filehandle);
               if(StringLen(tmp01)>0)
                 {
                  config[cindex1][cindex2]=tmp01;
                  cindex2++;
                  config[cindex1][0]=IntegerToString(StringToInteger(config[cindex1][0])+1);
                 }
              }
            if(FileIsEnding(filehandle))tmp02=1;
               cindex1++;cindex2=1;
               
           }
        }
        cindex=cindex1;
     }
     FileClose(filehandle);
     
     
   return(INIT_SUCCEEDED);
  }
void OnTimer(){
         string mode="opttimeleft";
         if(ObjectFind(ChartID(),mode)<0)ObjectCreate(mode, OBJ_LABEL, 0,0,0);
         ObjectSet(mode, OBJPROP_CORNER, 0);
         ObjectSet(mode, OBJPROP_XDISTANCE, 10);
         ObjectSet(mode, OBJPROP_YDISTANCE, 40);
         ObjectSetText(mode,"Оптимизация через "+TimetoStr(), 16, "Arial Narrow", clrGreen);     
         opttimeleft=opttimeleft-1;
         if(opttimeleft<1){
            opttimeleft=0;
            mode="errors";
            if(!barsupdated){
               if(ObjectFind(ChartID(),mode)<0)ObjectCreate(mode, OBJ_LABEL, 0,0,0);
               ObjectSet(mode, OBJPROP_CORNER, 0);
               ObjectSet(mode, OBJPROP_XDISTANCE, 10);
               ObjectSet(mode, OBJPROP_YDISTANCE, 20);
               ObjectSetText(mode,"Исправьте ошибки! https://t.me/markettraderoptimizer", 16, "Arial Narrow", clrRed);             
            
            }else{
               ObjectSetText(mode,"Ошибки исправлены", 16, "Arial Narrow", clrGreen);
               ShellExecuteW(0,"","MarketTraderOptimizerStart.bat","",TerminalInfoString(TERMINAL_DATA_PATH)+"\MQL4\Files\\",1);
               PostMessageA(GetParent(GetParent(GetParent(WindowHandle(Symbol(),Period())))),18,0,0);

            }
            
         }
         timer2=timer2-1;
         if(timer2<1){
           timer2=300;
           OnTick1();
         }
         
         if(opttimeleft>300)timer1limit1=60;else timer1limit1=2;
         timer1=timer1-1;
         if(timer1<1){
           timer1=timer1limit1;
            barsupdated=true;
            int i,i1,i2,i4;double i3;int x=0,y=0;
            for(i=0;i<cindex;i++){
               for(i2=0;i2<StringToInteger(config[i][0]);i2++)
               {
                  mode=""+config[i][1]; SymbolSelect(mode,true);
                  if(ObjectFind(ChartID(),mode)<0)ObjectCreate(mode, OBJ_LABEL, 0,0,0);
                  ObjectSet(mode, OBJPROP_CORNER, 0);
                  ObjectSet(mode, OBJPROP_XDISTANCE, x*100);
                  ObjectSet(mode, OBJPROP_YDISTANCE, 60+y*15);
                  
                  i1=0;
                  //while(i1<bars){
                     for(i4=0;i4<=bars;i4++)i3=iClose(mode,StringToInteger(GetElement(config[i][i2+2],0)),i4);//iMA(mode,tfs[i2],3,0,MODE_SMA,PRICE_CLOSE,bars+1);
                     i1=StringToInteger(iBars(mode,GetElement(config[i][i2+2],0)));
                     if(i1>=bars){ObjectSetText(mode,mode+" : "+GetElement(config[i][i2+2],0)+" "+i1, 8, "Arial Narrow", clrGreen);
                     
                     }
                     else {
                        ObjectSetText(mode,mode+" : "+GetElement(config[i][i2+2],0)+" "+i1, 8, "Arial Narrow", clrGray);
                        barsupdated=false;
                        break;
                    //    Sleep(1000);
                     }
                  //}
               }
               y=y+1; if(y==30){y=0;x=x+1;}
            }         
         
         }

}
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//int filehandle=FileOpen("params.txt",FILE_READ|FILE_WRITE|FILE_TXT);
//FileSeek(filehandle,0,SEEK_END);
//FileWriteString(filehandle,TimeToStr(TimeLocal(),TIME_DATE|TIME_SECONDS)+ "\r\n"+MathRand()+"\r\n");
//FileClose(filehandle);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
string GetElement(string str, int index){

   string arr[];
   StringSplit(str,' ',arr);
   return arr[index];
}
string SetElement(string str, int index, string val){

   string arr[];
   StringSplit(str,' ',arr);
   arr[index]=val;
   string str2="";
   for(int i=0;i<ArraySize(arr);i++)if(i<ArraySize(arr)-1)str2=str2+arr[i]+" "; else str2=str2+arr[i];
   return str2;
}
void PatchLastDateTFOrder(string sm, string tf){
   for(int i2=0;i2<cindex;i2++)
   for(int i3=0;i3<StringToInteger(config[i2][0]);i3++)
   if((config[i2][1]==sm)&&(GetElement(config[i2][2+i3],0)==tf))config[i2][2+i3]=SetElement(config[i2][2+i3],5,(string)iTime(config[i2][1],GetElement(config[i2][2+i3],0),0));

   int filehandle;
   FileDelete(contests);
   filehandle=FileOpen(contests,FILE_WRITE|FILE_TXT);
   FileSeek(filehandle,0,SEEK_SET);
   if(filehandle!=INVALID_HANDLE){
         for( i2=0;i2<cindex;i2++){
            FileWriteString(filehandle,config[i2][1]+"\r\n");
            for( i3=0;i3<StringToInteger(config[i2][0]);i3++){
               if((i2<cindex-1)&&(i3<StringToInteger(config[i2][0])))
               FileWriteString(filehandle,config[i2][2+i3]+"\r\n");
               else{
                  if((i2==(cindex-1))&&(i3<(StringToInteger(config[i2][0])-1)))
                  FileWriteString(filehandle,config[i2][2+i3]+"\r\n");
                  else
                  FileWriteString(filehandle,config[i2][2+i3]);
               }
            }
            if(i2<(cindex-1))FileWriteString(filehandle,"\r\n");
         }
   }
   FileClose(filehandle);

}
int CheckRestriction(string symbol, string timeout){
   timeout="17280"+StringSubstr(timeout,StringLen(timeout)-1,1);
   if(IsTesting()&&(symbol!=Symbol()))return 1;
   for(int i1=0;i1<OrdersTotal();i1++)
     {
      OrderSelect(i1,SELECT_BY_POS,MODE_TRADES);
      if(OrderSymbol()==symbol)       
         //if(OrderComment()==timeout) 
         return (1);
     }
   return (0);
}
double iCCI2(string symbol, int tf, int shift, int period_ma_slow, int period_ma_fast, int cci_period)
  {
   double ma_fast,ma_slow;
   int i1;
   ma_fast=ma_slow=iCCI(symbol,tf,cci_period,PRICE_TYPICAL,shift)+10000.0;
   for(i1=1; i1<period_ma_slow; i1++)
      ma_slow=ma_slow+iCCI(symbol,tf,cci_period,PRICE_TYPICAL,i1+shift)+10000.0;
   ma_slow=ma_slow/period_ma_slow;
   for(i1=1; i1<period_ma_fast; i1++)
      ma_fast=ma_fast+iCCI(symbol,tf,cci_period,PRICE_TYPICAL,i1+shift)+10000.0;
   ma_fast=ma_fast/period_ma_fast;
   return (ma_fast-ma_slow);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int DeltaMasLength(string symbol, int tf, int period_ma_fast, int period_ma_slow, int cci_period)
  {
   int i,tmp2=1;
   double tmp1,tmp3=iCCI2(symbol,tf,2,period_ma_slow,period_ma_fast,cci_period);
   double prevtmp1;
   prevtmp1=MathAbs(tmp3);
   if(tmp3<0)tmp2=-1;else tmp2=1;
   tmp3=MathAbs(tmp3);

   double tmp3_2=MathAbs(iCCI2(symbol,tf,10,period_ma_slow,period_ma_fast,cci_period));
   double tmp3_3=MathAbs(iCCI2(symbol,tf,18,period_ma_slow,period_ma_fast,cci_period));
   if((tmp3>(tmp3_2*1.2))&&(tmp3>(tmp3_3*1.3)))
   {
      double tmp4=MathAbs(iCCI2(symbol,tf,0,period_ma_slow,period_ma_fast,cci_period));
      double tmp5=MathAbs(iCCI2(symbol,tf,1,period_ma_slow,period_ma_fast,cci_period));
      if(tmp4<=tmp5)
      if(tmp3>tmp5)
         for(i=3; i<200; i++)
           {
            tmp1=iCCI2(symbol,tf,i,period_ma_slow,period_ma_fast,cci_period);
            
            tmp1=MathAbs(tmp1);
            if(prevtmp1<tmp1)
               return (i*tmp2);
            prevtmp1=tmp1;
           }
   }
   return 0;
  }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void OnTick1()
  {
   int i1,i2,i3;
   double tmp001;
  
   //if(prevbar==iOpen(NULL,0,0))return;
   //prevbar=iOpen(NULL,0,0);
   for(i1=0; i1<cindex; i1++)for(i2=0; i2<config[i1][0]; i2++) tmp001=iClose(config[i1][1],StringToInteger(GetElement(config[i1][2+i2],0)),0);

   //for(i2=0;i2<cindex;i2++)
   //for(i3=0;i3<StringToInteger(config[i2][0]);i3++)
   //for(i1=0;i1<OrdersTotal();i1++)
   //  {
   //   OrderSelect(i1,SELECT_BY_POS,MODE_TRADES);
   //   if(OrderSymbol()==config[i2][1]){       
   //         if(((long)iTime(config[i2][1],GetElement(config[i2][2+i3],0),0))>((long)OrderComment()) )
   //           {
   //            if(OrderType()==OP_BUY)
   //               {if(!TradeSundaySaturday && (DayOfWeek()<2 || DayOfWeek()==6)){;}else OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_BID),3,Violet);Alert(config[i2][1]+" BUY Close "+OrderProfit());}
   //            else
   //               {if(!TradeSundaySaturday && (DayOfWeek()<2 || DayOfWeek()==6)){;}else OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_ASK),3,Violet);Alert(config[i2][1]+" SELL Close "+OrderProfit());}
   //           }
   //   }
   //  }
   int    res;
   
   for(i2=0;i2<cindex;i2++)
   for(i3=0;i3<StringToInteger(config[i2][0]);i3++)   
     {
      int signal;
      if(StringToInteger(GetElement(config[i2][2+i3],1))>0) 
      signal=DeltaMasLength(config[i2][1],
       StringToInteger(GetElement(config[i2][2+i3],0)),
       StringToInteger(GetElement(config[i2][2+i3],1)),
       StringToInteger(GetElement(config[i2][2+i3],2)),
       StringToInteger(GetElement(config[i2][2+i3],3)));
      int signal2;
      if(StringToInteger(GetElement(config[i2][2+i3],12))>0)
      signal2=DeltaMasLength(config[i2][1],
       StringToInteger(GetElement(config[i2][2+i3],0)),
       StringToInteger(GetElement(config[i2][2+i3],12)),
       StringToInteger(GetElement(config[i2][2+i3],13)),
       StringToInteger(GetElement(config[i2][2+i3],14)));
      
      double stoplevel = NormalizeDouble(MarketInfo(config[i2][1],MODE_SPREAD)*1.5*MarketInfo(config[i2][1],MODE_POINT), (int)MarketInfo(config[i2][1],MODE_DIGITS));
      double takeprofits = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],4))*multp*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double takeprofitb = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],11))*multp*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double stoplosss = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],9))*mulsl*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double stoplossb = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],10))*mulsl*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      string s2=GetElement(config[i2][2+i3],0);//(string)(iTime(config[i2][1],GetElement(config[i2][2+i3],0),0));
      int t1=0;
 
      if(takeprofits < stoplevel)takeprofits = stoplevel;
      if(takeprofitb < stoplevel)takeprofitb = stoplevel;
      if(stoplosss < stoplevel)stoplosss = stoplevel;
      if(stoplossb < stoplevel)stoplossb = stoplevel;
      
       if(!TradeSundaySaturday && (DayOfWeek()==0 || DayOfWeek()==6)){;}else
       if(((long)GetElement(config[i2][2+i3],5)) < ((long)iTime(config[i2][1],GetElement(config[i2][2+i3],0),0)))
       if((StringToInteger(GetElement(config[i2][2+i3],6))*2)>MarketInfo(config[i2][1],MODE_SPREAD))
       {
         if(StringToInteger(GetElement(config[i2][2+i3],12))>0)
         if((signal2>0)&&(MathAbs(signal2)>19))
           {
            //for(i1=0;i1<1;i1++){
               res=-1;while(res==-1){
                  res=OrderSend(config[i2][1],OP_SELL,Lot,MarketInfo(config[i2][1],MODE_BID),3,MarketInfo(config[i2][1],MODE_ASK)+stoplosss,MarketInfo(config[i2][1],MODE_BID)-takeprofits,s2,0,0,Red);
                  //res=OrderSend(config[i2][1],OP_BUYSTOP,0.01,MarketInfo(config[i2][1],MODE_ASK)+StringToInteger(GetElement(config[i2][2+i3],4))*Point*2,3,0,MarketInfo(config[i2][1],MODE_ASK)+StringToInteger(GetElement(config[i2][2+i3],4))*Point*3,GetElement(config[i2][2+i3],5),0,TimeCurrent()+60*10,Blue);
                  //res=OrderSend(cindex[i2][1],OP_SELLLIMIT,0.01,Bid+GetElement(config[i2][2+i3],4)*Point/2,3,0,Bid-GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Red);
               //Print("ERROR: "+GetLastError());
                  t1++;if(t1>5)res=0;Sleep(1000);}
                  //if(res>0)
                  PatchLastDateTFOrder(config[i2][1], GetElement(config[i2][2+i3],0));                  
                  Alert(acctype()+config[i2][1]+" TF " + GetElement(config[i2][2+i3],0)+" SELL SL "+ DoubleToString(MarketInfo(config[i2][1],MODE_ASK)+stoplosss)+" TP "+DoubleToString(MarketInfo(config[i2][1],MODE_BID)-takeprofits) + " " + vers);
            //}
            //Alert(config[i2][1]+" SELL");
           }
         if(StringToInteger(GetElement(config[i2][2+i3],1))>0)  
         if((signal<0)&&(MathAbs(signal)>19))
           {
            //for(i1=0;i1<1;i1++){
               res=-1;while(res==-1){
                  res=OrderSend(config[i2][1],OP_BUY,Lot,MarketInfo(config[i2][1],MODE_ASK),3,MarketInfo(config[i2][1],MODE_BID)-stoplossb,MarketInfo(config[i2][1],MODE_ASK)+takeprofitb,s2,0,0,Blue);
                  //res=OrderSend(config[i2][1],OP_SELLSTOP,0.01,MarketInfo(config[i2][1],MODE_BID)-StringToInteger(GetElement(config[i2][2+i3],4))*Point*2,3,0,MarketInfo(config[i2][1],MODE_BID)-StringToInteger(GetElement(config[i2][2+i3],4))*Point*3,GetElement(config[i2][2+i3],5),0,TimeCurrent()+60*10,Red);
                  //res=OrderSend(cindex[i2][1],OP_BUYLIMIT,0.01,Ask-GetElement(config[i2][2+i3],4)*Point/2,3,0,Ask+GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Blue);
               //Print("ERROR: "+GetLastError());   
                 t1++;if(t1>5)res=0;Sleep(1000);}
                 //if(res>0)
                 PatchLastDateTFOrder(config[i2][1], GetElement(config[i2][2+i3],0)); 
                 Alert(acctype()+config[i2][1]+" TF " + GetElement(config[i2][2+i3],0)+" BUY SL "+ DoubleToString(MarketInfo(config[i2][1],MODE_BID)-stoplossb)+" TP "+DoubleToString(MarketInfo(config[i2][1],MODE_ASK)+takeprofitb) + " " + vers);

            //}
            //Alert(config[i2][1]+" BUY");
           }
         }
     }

  }

//+------------------------------------------------------------------+
