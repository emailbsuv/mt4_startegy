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
int OnInit(){
   firststart=1;
   return(INIT_SUCCEEDED);
}
void OnDeinit(const int reason){

}
double iCCI2(int shift){
  double ma_fast,ma_slow;
  int i1;
     ma_fast=ma_slow=iCCI(NULL,0,cci_period,PRICE_TYPICAL,shift);
     for(i1=1; i1<period_ma_slow; i1++)ma_slow=ma_slow+iCCI(NULL,0,cci_period,PRICE_TYPICAL,i1+shift);
     ma_slow=ma_slow/period_ma_slow;
     for(i1=1; i1<period_ma_fast; i1++)ma_fast=ma_fast+iCCI(NULL,0,cci_period,PRICE_TYPICAL,i1+shift);
     ma_fast=ma_fast/period_ma_fast;
     return (MathAbs(ma_fast-ma_slow));
}
int DeltaMasLength(){
  int i;
  double tmp1,tmp2;
  double prevtmp1=iCCI2(1);
  tmp2=prevtmp1;
  
  if(tmp2>iCCI2(0))     
  for(i=2; i<200; i++){
     tmp1=iCCI2(i);
     if(prevtmp1<tmp1)return (i);
     prevtmp1=tmp1;
  }
  return (0);
} 

void OnTick()
  {
   if(OrdersTotal()>0){ 
    OrderSelect(0,SELECT_BY_POS,MODE_TRADES);
    if(OrderType()==OP_BUY)lastorder=OP_BUY;else lastorder=OP_SELL;
//    if( ((OrderOpenTime()<(TimeCurrent()-60*60*4*4)) && Period()<15) ||
//        ((OrderOpenTime()<(TimeCurrent()-60*60*4*4)) && Period()<15) || 
//        ((OrderOpenTime()<(TimeCurrent()-1440*60)) && (Period()<60 && Period()>5)) || 
//        ((OrderOpenTime()<(TimeCurrent()-1440*60*4)) && Period()==60) || 
//        ((OrderOpenTime()<(TimeCurrent()-1440*60*16*4)) && Period()==240) 
//        
//        )
    if((OrderOpenTime()<(TimeCurrent()-1440*60)) && Period()<60)
    //if((OrderOpenTime()<(TimeCurrent()-1440*60*2)) && Period()<1440)
    {
     if(OrderType()==OP_BUY)OrderClose(OrderTicket(),OrderLots(),Bid,3,Violet);else OrderClose(OrderTicket(),OrderLots(),Ask,3,Violet);
    }
    return;
   }
   int    res;
if((lastorder==OP_BUY) || (firststart==1))
{ 
  //if(iHighest(NULL,0,MODE_HIGH,25,0)<11)
  if((DeltaMasLength()>9) && (iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,1)>iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,0)) )
     {
      res=OrderSend(Symbol(),OP_SELL,0.01,Bid,3,0,Bid-6000*Point,"",0,0,Red);
      lastorder=OP_SELL;
      firststart=0;
      //res=OrderSend(Symbol(),OP_SELLLIMIT,0.01,Bid+30*Point,3,0,Bid-30*Point,"",0,TimeCurrent()+1440*60/2,Red);
     }

}

if((lastorder==OP_SELL) || (firststart==1))
{ 
  //if(iLowest(NULL,0,MODE_LOW,25,0)<11)
  if((DeltaMasLength()>9) && (iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,1)<iMA(NULL,0,3,0,MODE_SMA,PRICE_CLOSE,0)) )
     {
      res=OrderSend(Symbol(),OP_BUY,0.01,Ask,3,0,Ask+6000*Point,"",0,0,Blue);
      lastorder=OP_BUY;
      firststart=0;
      //res=OrderSend(Symbol(),OP_BUYLIMIT,0.01,Ask-30*Point,3,0,Ask+30*Point,"",0,TimeCurrent()+1440*60/2,Blue);
     }

}

  }

