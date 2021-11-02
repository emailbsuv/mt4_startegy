//+------------------------------------------------------------------+
//|                                                    ArraySort.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict
#property indicator_chart_window

string symbols_array[23]={"USDCHF","GBPUSD","EURUSD","USDJPY","USDCAD","AUDUSD","EURGBP","EURAUD","EURCHF","EURJPY","GBPCHF","CADJPY","GBPJPY","AUDNZD","AUDCAD","AUDCHF","AUDJPY","CHFJPY","EURNZD","EURCAD","CADCHF","NZDJPY","NZDUSD"}; 
int hilow_array[23][2]; 
long current_chart_id;
int prev_h1_hi,prev_h1_low;
string stoch_sorted_index(int i0){
  int i;
  for(i=0; i<23; i++)if(i==i0)return symbols_array[i];
  return 0;
}


//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- indicator buffers mapping
prev_h1_hi=0;prev_h1_low=0;
int i;
current_chart_id=ChartID();
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Hilow"+symbols_array[i],OBJ_LABEL,0,0,0)) 
      { 
       Print("Ошибка создания объекта: code #",GetLastError()); 
       return(0); 
      } 
   }
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Rsi"+symbols_array[i],OBJ_LABEL,0,0,0)) 
      { 
       Print("Ошибка создания объекта: code #",GetLastError()); 
       return(0); 
      } 
   }   
  EventSetTimer(1);    
//---
   return(INIT_SUCCEEDED);
  }
  void OnDeinit(){
    EventKillTimer();
  }
//+------------------------------------------------------------------+
//| Custom indicator iteration function                              |
//+------------------------------------------------------------------+
int OnCalculate(const int rates_total,
                const int prev_calculated,
                const datetime &time[],
                const double &open[],
                const double &high[],
                const double &low[],
                const double &close[],
                const long &tick_volume[],
                const long &volume[],
                const int &spread[])
  {
//---
   
//--- return value of prev_calculated for next call
   return(rates_total);
  }
//+------------------------------------------------------------------+
void OnTimer() 
  { 
   int i,i0,i1,i2; 
   
   for(i=0; i<23; i++){
     hilow_array[i][0]=i;
     hilow_array[i][1]=(int)(iClose(symbols_array[i],0,0)*MathPow(10,(int)MarketInfo(symbols_array[i],MODE_DIGITS))-iOpen(symbols_array[i],0,0)*MathPow(10,(int)MarketInfo(symbols_array[i],MODE_DIGITS)));//iStochastic(symbols_array[i],0,8,3,3,MODE_SMA,0,MODE_MAIN,0);
   }
   double hilow_array_tmp[23][2];
   for(i=0; i<23; i++){
     hilow_array_tmp[i][0]=hilow_array[i][0];
     hilow_array_tmp[i][1]=hilow_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( hilow_array_tmp[i1][1] > i3){
         i3 = hilow_array_tmp[i1][1];
         i2=i1;
       }
     }
     hilow_array[i0][0]=hilow_array_tmp[i2][0];
     hilow_array[i0][1]=hilow_array_tmp[i2][1];
     i0++;
     hilow_array_tmp[i2][1]=-100000;
   }   
   
    

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Hilow"+symbols_array[i],OBJPROP_TEXT,StringFormat("Hilow %s : %d",stoch_sorted_index(hilow_array[i][0]),hilow_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Hilow"+symbols_array[i],OBJPROP_YDISTANCE,i*15); 
      ObjectSet("Hilow"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(hilow_array[i][1]>0)
     ObjectSetInteger(current_chart_id,"Hilow"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(hilow_array[i][1]<0)
     ObjectSetInteger(current_chart_id,"Hilow"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Hilow"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }
     
     
  if(prev_h1_hi < hilow_array[0][1]){
   prev_h1_hi = hilow_array[0][1];
   PlaySound("hi.wav");
  }    
  if(prev_h1_low > hilow_array[22][1]){
   prev_h1_low = hilow_array[22][1];
   PlaySound("low.wav");
  }  
  // Sleep(1);
  // return(0); 
  }