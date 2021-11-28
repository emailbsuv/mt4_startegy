#property copyright "Copyright 2021, Suvorov Bogdan"
#property link      "https://t.me/markettraderoptimizer"
#property description "MarketTrader expert advisor"

input bool TradeSundaySaturday=true;
input double Lot=0.01;

int lastorder,firststart;
string config[200][9];
int cindex,cindex1=0,cindex2=1;
double prevbar;
double mulsl,multp;
int OnInit()
  {
   int filehandle;
   string str1;
   prevbar=iOpen(NULL,0,0);
   firststart=1;
   filehandle=FileOpen("settings.txt",FILE_READ|FILE_TXT);
   FileSeek(filehandle,0,SEEK_SET);
   str1=FileReadString(filehandle);
   FileClose(filehandle);
   mulsl=StringToDouble(GetElement(str1,4));
   multp=StringToDouble(GetElement(str1,5));
   filehandle=FileOpen(GetElement(str1,2),FILE_READ|FILE_TXT);
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
int CheckRestriction(string symbol, string timeout){
   timeout="17280"+StringSubstr(timeout,StringLen(timeout)-1,1);
   if(IsTesting()&&(symbol!=Symbol()))return 1;
   for(int i1=0;i1<OrdersTotal();i1++)
     {
      OrderSelect(i1,SELECT_BY_POS,MODE_TRADES);
      if(OrderSymbol()==symbol)       
         if(OrderComment()==timeout) return (1);
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
int DeltaMasLength(string symbol, int tf, int period_ma_slow, int period_ma_fast, int cci_period)
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
int Delta3MasLength(string symbol, int tf, int period_ma_slow, int period_ma_fast, int cci_period)
  {
   int i,tmp2=1;
   double tmp1,tmp3=iCCI2(symbol,tf,1,period_ma_slow,period_ma_fast,cci_period);
   double prevtmp1;
   prevtmp1=9999999;
   if(tmp3<0)tmp2=-1;else tmp2=1;
   //tmp3=MathAbs(tmp3);

  // if(tmp3>MathAbs(iCCI2(symbol,tf,0,period_ma_slow,period_ma_fast,cci_period)))
      for(i=0; i<200; i++)
        {
         tmp1=iCCI2(symbol,tf,i,period_ma_slow,period_ma_fast,cci_period);
         
         tmp1=MathAbs(tmp1);
         if(prevtmp1<tmp1)
            return (i*tmp2);
         prevtmp1=tmp1;
        }
   return 0;
  }
int Delta2MasLength(string symbol, int tf, int period_ma_slow, int period_ma_fast, int cci_period, int tf2, int period_ma_slow2, int period_ma_fast2, int cci_period2)
  {
    int masperiod1=DeltaMasLength(symbol, tf, period_ma_slow, period_ma_fast, cci_period);
    int masperiod2=Delta3MasLength(symbol, tf2, period_ma_slow2, period_ma_fast2, cci_period2);
    //if( ((masperiod1<0) && (masperiod2<0)) || ((masperiod1>0) && (masperiod2>0)) )
    //if( (MathAbs(masperiod1)>9) && (MathAbs(masperiod2)>13)&& (MathAbs(masperiod2)<MathAbs(masperiod1)) ) return 1;
    if( (MathAbs(masperiod1)>9) && (MathAbs(masperiod2)>9) ){
       if((masperiod1<0) && (masperiod2<0)) return OP_BUY;
       if((masperiod1>0) && (masperiod2>0)) return OP_SELL;
    }
    return -1;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void OnTick()
  {
   int i1,i2,i3;
   double tmp001;
   
   
   if(prevbar==iOpen(NULL,0,0))return;
   prevbar=iOpen(NULL,0,0);
   for(i1=0; i1<cindex; i1++)for(i2=0; i2<config[i1][0]; i2++) tmp001=iClose(config[i1][1],config[i1][StringToInteger(GetElement(config[i1][2+i2],0))],0);

   for(i2=0;i2<cindex;i2++)
   for(i3=0;i3<StringToInteger(config[i2][0]);i3++)
   for(i1=0;i1<OrdersTotal();i1++)
     {
      OrderSelect(i1,SELECT_BY_POS,MODE_TRADES);
      if(OrderSymbol()==config[i2][1]){       
            if(OrderOpenTime()<(TimeCurrent()-StringToInteger(OrderComment()) ))
              {
               if(OrderType()==OP_BUY)
                  {if(!TradeSundaySaturday && (DayOfWeek()<2 || DayOfWeek()==6)){;}else OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_BID),3,Violet);Alert(config[i2][1]+" BUY Close "+OrderProfit());}
               else
                  {if(!TradeSundaySaturday && (DayOfWeek()<2 || DayOfWeek()==6)){;}else OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_ASK),3,Violet);Alert(config[i2][1]+" SELL Close "+OrderProfit());}
              }
      }
     }
   int    res;
   
   if(OrdersTotal()<511)
   for(i2=0;i2<cindex;i2++)
   for(i3=0;i3<StringToInteger(config[i2][0]);i3++)   
   if(CheckRestriction(config[i2][1], GetElement(config[i2][2+i3],5) )==0)
   if(StringToInteger(GetElement(config[i2][2+i3],1))>0)
   //if(Delta2MasLength(config[i2][1],
   //    StringToInteger(GetElement(config[i2][2+i3],0)),
   //    StringToInteger(GetElement(config[i2][2+i3],1)),
   //    StringToInteger(GetElement(config[i2][2+i3],2)),
   //    StringToInteger(GetElement(config[i2][2+i3],3)),
   //    StringToInteger(GetElement(config[i2][2+i3+1],0)),
   //    StringToInteger(GetElement(config[i2][2+i3+1],1)),
   //    StringToInteger(GetElement(config[i2][2+i3+1],2)),
   //    StringToInteger(GetElement(config[i2][2+i3+1],3)))==1)
     {
      int signal=DeltaMasLength(config[i2][1],
       StringToInteger(GetElement(config[i2][2+i3],0)),
       StringToInteger(GetElement(config[i2][2+i3],1)),
       StringToInteger(GetElement(config[i2][2+i3],2)),
       StringToInteger(GetElement(config[i2][2+i3],3)));
      if(MathAbs(signal)<21)continue; 
      double takeprofits = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],4))*multp*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double takeprofitb = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],11))*multp*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double stoplosss = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],9))*mulsl*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      double stoplossb = NormalizeDouble(StringToInteger(GetElement(config[i2][2+i3],10))*mulsl*MarketInfo(config[i2][1],MODE_POINT),MarketInfo(config[i2][1],MODE_DIGITS));
      string s1=GetElement(config[i2][2+i3],5);
      string s2="17280"+StringSubstr(s1,StringLen(s1)-1,1);
      //double stoplevel =MarketInfo(config[i2][1],MODE_STOPLEVEL);
      int t1=0;
      
       //if((stoplevel<takeprofit)||(stoplevel<1))
       if(!TradeSundaySaturday && (DayOfWeek()==0 || DayOfWeek()==6)){;}else
       {
         if(signal>0)
           {
            for(i1=0;i1<1;i1++){
               res=-1;while(res==-1){
                  res=OrderSend(config[i2][1],OP_SELL,Lot,MarketInfo(config[i2][1],MODE_BID),3,MarketInfo(config[i2][1],MODE_ASK)+stoplosss,MarketInfo(config[i2][1],MODE_ASK)-takeprofits,s2,0,0,Red);
                  //res=OrderSend(config[i2][1],OP_BUYSTOP,0.01,MarketInfo(config[i2][1],MODE_ASK)+StringToInteger(GetElement(config[i2][2+i3],4))*Point*2,3,0,MarketInfo(config[i2][1],MODE_ASK)+StringToInteger(GetElement(config[i2][2+i3],4))*Point*3,GetElement(config[i2][2+i3],5),0,TimeCurrent()+60*10,Blue);
                  //res=OrderSend(cindex[i2][1],OP_SELLLIMIT,0.01,Bid+GetElement(config[i2][2+i3],4)*Point/2,3,0,Bid-GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Red);
               //Print("ERROR: "+GetLastError());
                  t1++;if(t1>5)res=0;Sleep(1000);}
            }
           // Alert(config[i2][1]+" SELL");
           }
         if(signal<0)
           {
            for(i1=0;i1<1;i1++){
               res=-1;while(res==-1){
                  res=OrderSend(config[i2][1],OP_BUY,Lot,MarketInfo(config[i2][1],MODE_ASK),3,MarketInfo(config[i2][1],MODE_BID)-stoplossb,MarketInfo(config[i2][1],MODE_BID)+takeprofitb,s2,0,0,Blue);
                  //res=OrderSend(config[i2][1],OP_SELLSTOP,0.01,MarketInfo(config[i2][1],MODE_BID)-StringToInteger(GetElement(config[i2][2+i3],4))*Point*2,3,0,MarketInfo(config[i2][1],MODE_BID)-StringToInteger(GetElement(config[i2][2+i3],4))*Point*3,GetElement(config[i2][2+i3],5),0,TimeCurrent()+60*10,Red);
                  //res=OrderSend(cindex[i2][1],OP_BUYLIMIT,0.01,Ask-GetElement(config[i2][2+i3],4)*Point/2,3,0,Ask+GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Blue);
               //Print("ERROR: "+GetLastError());   
                 t1++;if(t1>5)res=0;Sleep(1000);}
            }
           // Alert(config[i2][1]+" BUY");
           }
         }
     }

  }

//+------------------------------------------------------------------+
