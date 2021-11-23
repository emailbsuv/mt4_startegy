//+------------------------------------------------------------------+
//|                                               Moving Average.mq4 |
//|                   Copyright 2005-2014, MetaQuotes Software Corp. |
//|                                              http://www.mql4.com |
//+------------------------------------------------------------------+
#property copyright   "2005-2014, MetaQuotes Software Corp."
#property link        "http://www.mql4.com"
#property description "Moving Average sample expert advisor"

//--- Inputs
input int    period_ma_fast = 14;
input int    period_ma_slow = 111;
input int    cci_period = 55;

int lastorder,firststart;

string config[200][9];
int cindex,cindex1=0,cindex2=1;
//bool writeconfig=true;
bool writeconfig=false;

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int OnInit()
  {
   firststart=1;

   if(writeconfig)
      if(!IsOptimization())
        {
         int filehandle=FileOpen("contests.txt",FILE_READ|FILE_TXT);
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
                  if(FileIsEnding(filehandle))
                     tmp02=1;
                  cindex1++;
                  cindex2=1;

                 }
              }
            cindex=cindex1;
           }
         FileClose(filehandle);
        }


   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
string GetElement(string str, int index)
  {

   string arr[];
   StringSplit(str,' ',arr);
   return arr[index];
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
string SetElement(string str, int index, string str1)
  {
   int cnt1,i1;
   string arr[],str2="";
   cnt1=StringSplit(str,' ',arr);
   arr[index]=str1;
   for(i1=0; i1<cnt1; i1++)
     {
      str2=str2+arr[i1];
      if(i1!=(cnt1-1))
         str2=str2+" ";
     }
   return str2;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
   if(writeconfig)
      if(!IsOptimization())
        {
         int i1,i2;
         string tmp01;
         for(i1=0; i1<cindex; i1++)
            if(Symbol()==config[i1][1])
               for(i2=0; i2<config[i1][0]; i2++)
                 {
                  if(Period()==GetElement(config[i1][i2+2],0))
                    {
                     config[i1][i2+2]=SetElement(config[i1][i2+2],1,period_ma_fast);
                     config[i1][i2+2]=SetElement(config[i1][i2+2],2,period_ma_slow);
                     config[i1][i2+2]=SetElement(config[i1][i2+2],3,cci_period);
                    }
                 }
         int filehandle=FileOpen("contests.txt",FILE_READ|FILE_WRITE|FILE_TXT);
         FileSeek(filehandle,0,SEEK_SET);
         for(i1=0; i1<cindex; i1++)
           {
            FileWriteString(filehandle,config[i1][1]+ "\r\n");
            for(i2=0; i2<StrToInteger(config[i1][0]); i2++)
              {
               tmp01=config[i1][i2+2];
               if(i1!=(cindex-1))
                  tmp01=tmp01+"\r\n";
               if((i1==(cindex-1))&&(i2!=(StrToInteger(config[i1][0])-1)))
                  tmp01=tmp01+"\r\n";
               FileWriteString(filehandle,tmp01);
              }
            if(i1!=(cindex-1))
               FileWriteString(filehandle,"\r\n");
           }
         FileClose(filehandle);
        }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
double iCCI2(int shift)
  {
   double ma_fast,ma_slow;
   int i1;
   ma_fast=ma_slow=iCCI(NULL,0,cci_period,PRICE_TYPICAL,shift);
   for(i1=1; i1<period_ma_slow; i1++)
      ma_slow=ma_slow+iCCI(NULL,0,cci_period,PRICE_TYPICAL,i1+shift);
   ma_slow=ma_slow/period_ma_slow;
   for(i1=1; i1<period_ma_fast; i1++)
      ma_fast=ma_fast+iCCI(NULL,0,cci_period,PRICE_TYPICAL,i1+shift);
   ma_fast=ma_fast/period_ma_fast;
   return (MathAbs(ma_fast-ma_slow));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int DeltaMasLength()
  {
   int i;
   double tmp1,tmp2;
   double prevtmp1=iCCI2(1);
   tmp2=prevtmp1;

   if(tmp2>iCCI2(0))
      for(i=2; i<200; i++)
        {
         tmp1=iCCI2(i);
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
   if(OrdersTotal()>0)
     {
      OrderSelect(0,SELECT_BY_POS,MODE_TRADES);
      if(OrderType()==OP_BUY)
         lastorder=OP_BUY;
      else
         lastorder=OP_SELL;

      if(((OrderOpenTime()<(TimeCurrent()-1440*60)) && ((Period()==15) || (Period()==30))) ||
         ((OrderOpenTime()<(TimeCurrent()-1440*60*2)) && ((Period()==60) || (Period()==240))) ||
         ((OrderOpenTime()<(TimeCurrent()-1440*60*5)) && (Period()==1440)))
        {
         if(OrderType()==OP_BUY)
            OrderClose(OrderTicket(),OrderLots(),Bid,3,Violet);
         else
            OrderClose(OrderTicket(),OrderLots(),Ask,3,Violet);
        }
      return;
     }
   int    res;
   if((lastorder==OP_BUY) || (firststart==1))
     {
      if((DeltaMasLength()>9) && (iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,1)>iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,0)))
        {
         res=OrderSend(Symbol(),OP_SELL,0.01,Bid,3,0,Bid-600000*Point,"",0,0,Red);
         lastorder=OP_SELL;
         firststart=0;
        }

     }

   if((lastorder==OP_SELL) || (firststart==1))
     {
      if((DeltaMasLength()>9) && (iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,1)<iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,0)))
        {
         res=OrderSend(Symbol(),OP_BUY,0.01,Ask,3,0,Ask+600000*Point,"",0,0,Blue);
         lastorder=OP_BUY;
         firststart=0;
        }

     }

  }

//+------------------------------------------------------------------+
