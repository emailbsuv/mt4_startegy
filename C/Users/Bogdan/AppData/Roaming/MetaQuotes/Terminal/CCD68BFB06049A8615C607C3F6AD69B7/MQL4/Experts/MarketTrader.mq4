//+------------------------------------------------------------------+
//|                                               Moving Average.mq4 |
//|                   Copyright 2005-2014, MetaQuotes Software Corp. |
//|                                              http://www.mql4.com |
//+------------------------------------------------------------------+
#property copyright   "2005-2014, MetaQuotes Software Corp."
#property link        "http://www.mql4.com"
#property description "Moving Average sample expert advisor"

int lastorder,firststart;
string config[200][9];
int cindex,cindex1=0,cindex2=1;
double prevbar;
int OnInit()
  {
   prevbar=Open[0];
   firststart=1;
   int filehandle=FileOpen("contests1.txt",FILE_READ|FILE_TXT);
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
   ma_fast=ma_slow=iCCI(symbol,tf,cci_period,PRICE_TYPICAL,shift);
   for(i1=1; i1<period_ma_slow; i1++)
      ma_slow=ma_slow+iCCI(symbol,tf,cci_period,PRICE_TYPICAL,i1+shift);
   ma_slow=ma_slow/period_ma_slow;
   for(i1=1; i1<period_ma_fast; i1++)
      ma_fast=ma_fast+iCCI(symbol,tf,cci_period,PRICE_TYPICAL,i1+shift);
   ma_fast=ma_fast/period_ma_fast;
   return (MathAbs(ma_fast-ma_slow));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int DeltaMasLength(string symbol, int tf, int period_ma_slow, int period_ma_fast, int cci_period)
  {
   int i;
   double tmp1;
   double prevtmp1=999999.0;

   if(iCCI2(symbol,tf,1,period_ma_slow,period_ma_fast,cci_period)>iCCI2(symbol,tf,0,period_ma_slow,period_ma_fast,cci_period))
      for(i=2; i<200; i++)
        {
         tmp1=iCCI2(symbol,tf,i,period_ma_slow,period_ma_fast,cci_period);
         if(prevtmp1<tmp1)
            return (i);
         prevtmp1=tmp1;
        }
   return (0);
  }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void OnTick()
  {
   int i1,i2,i3;
   double tmp001;
   
   for(i1=0; i1<cindex; i1++)for(i2=0; i2<config[i1][0]; i2++) tmp001=iClose(config[i1][1],config[i1][StringToInteger(GetElement(config[i1][2+i2],0))],0);
   
   if(prevbar==Open[0])return;
   prevbar=Open[0];

   for(i2=0;i2<cindex;i2++)
   for(i3=0;i3<StringToInteger(config[i2][0]);i3++)
   for(i1=0;i1<OrdersTotal();i1++)
     {
      OrderSelect(i1,SELECT_BY_POS,MODE_TRADES);
      if(OrderSymbol()==config[i2][1]){       
            if(OrderOpenTime()<(TimeCurrent()-StringToInteger(OrderComment()) ))
              {
               if(OrderType()==OP_BUY)
                  {OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_BID),3,Violet);Alert(config[i2][1]+" BUY Close");}
               else
                  {OrderClose(OrderTicket(),OrderLots(),MarketInfo(config[i2][1],MODE_ASK),3,Violet);Alert(config[i2][1]+" SELL Close");}
              }
      }
     }
   int    res;
   
   for(i2=0;i2<cindex;i2++)
   for(i3=0;i3<StringToInteger(config[i2][0]);i3++)   
   if(CheckRestriction(config[i2][1], GetElement(config[i2][2+i3],5) )==0)
   if(DeltaMasLength(config[i2][1],
       StringToInteger(GetElement(config[i2][2+i3],0)),
       StringToInteger(GetElement(config[i2][2+i3],1)),
       StringToInteger(GetElement(config[i2][2+i3],2)),
       StringToInteger(GetElement(config[i2][2+i3],3)))>9)
     {

      double tmp04=iMA(config[i2][1],StringToInteger(GetElement(config[i2][2+i3],0)),3,0,MODE_SMA,PRICE_CLOSE,1);
      double tmp05=iMA(config[i2][1],StringToInteger(GetElement(config[i2][2+i3],0)),3,0,MODE_SMA,PRICE_CLOSE,0);
      if(tmp04>tmp05)
        {
         res=-1;while(res==-1){
            res=OrderSend(config[i2][1],OP_SELL,0.01,MarketInfo(config[i2][1],MODE_BID),3,0,MarketInfo(config[i2][1],MODE_BID)-StringToInteger(GetElement(config[i2][2+i3],4))*MarketInfo(config[i2][1],MODE_POINT),GetElement(config[i2][2+i3],5),0,0,Red);
            //res=OrderSend(cindex[i2][1],OP_SELLLIMIT,0.01,Bid+GetElement(config[i2][2+i3],4)*Point/2,3,0,Bid-GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Red);
         //Print("ERROR: "+GetLastError());
            Sleep(1000);}
         Alert(config[i2][1]+" SELL");
        }

      if(tmp04<tmp05)
        {
         res=-1; while(res==-1){
            res=OrderSend(config[i2][1],OP_BUY,0.01,MarketInfo(config[i2][1],MODE_ASK),3,0,MarketInfo(config[i2][1],MODE_ASK)+StringToInteger(GetElement(config[i2][2+i3],4))*MarketInfo(config[i2][1],MODE_POINT),GetElement(config[i2][2+i3],5),0,0,Blue);
            //res=OrderSend(cindex[i2][1],OP_BUYLIMIT,0.01,Ask-GetElement(config[i2][2+i3],4)*Point/2,3,0,Ask+GetElement(config[i2][2+i3],4)*Point/2,"",0,TimeCurrent()+1440*60/2,Blue);
         //Print("ERROR: "+GetLastError());   
            Sleep(1000);}
         Alert(config[i2][1]+" BUY");
        }
     }

  }

//+------------------------------------------------------------------+
//if(OrderComment()==GetElement(config[i2][2+i3],5))