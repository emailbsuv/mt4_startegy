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
double stoch_array[23][2]; 
double rsi_array[23][2];
long current_chart_id;
int prev_h1_stoch,prev_h1_rsi;
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
prev_h1_stoch=0;prev_h1_rsi=0;
int i;
current_chart_id=ChartID();
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Stoch"+symbols_array[i],OBJ_LABEL,0,0,0)) 
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
     stoch_array[i][0]=i;
     stoch_array[i][1]=iStochastic(symbols_array[i],0,8,3,3,MODE_SMA,0,MODE_MAIN,0);
   }
   double stoch_array_tmp[23][2];
   for(i=0; i<23; i++){
     stoch_array_tmp[i][0]=stoch_array[i][0];
     stoch_array_tmp[i][1]=stoch_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( stoch_array_tmp[i1][1] > i3){
         i3 = stoch_array_tmp[i1][1];
         i2=i1;
       }
     }
     stoch_array[i0][0]=stoch_array_tmp[i2][0];
     stoch_array[i0][1]=stoch_array_tmp[i2][1];
     i0++;
     stoch_array_tmp[i2][1]=-100000;
   }   
   
    

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Stoch"+symbols_array[i],OBJPROP_TEXT,StringFormat("Stoch %s : %f",stoch_sorted_index(stoch_array[i][0]),stoch_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Stoch"+symbols_array[i],OBJPROP_YDISTANCE,i*15); 
      ObjectSet("Stoch"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(stoch_array[i][1]>=80)
     ObjectSetInteger(current_chart_id,"Stoch"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(stoch_array[i][1]<=20)
     ObjectSetInteger(current_chart_id,"Stoch"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Stoch"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }
     
//================ R S I ===================

   for(i=0; i<23; i++){
     rsi_array[i][0]=i;
     rsi_array[i][1]=iRSI(symbols_array[i],0,8,PRICE_CLOSE,0);
   }
   double rsi_array_tmp[23][2];
   for(i=0; i<23; i++){
     rsi_array_tmp[i][0]=rsi_array[i][0];
     rsi_array_tmp[i][1]=rsi_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( rsi_array_tmp[i1][1] > i3){
         i3 = rsi_array_tmp[i1][1];
         i2=i1;
       }
     }
     rsi_array[i0][0]=rsi_array_tmp[i2][0];
     rsi_array[i0][1]=rsi_array_tmp[i2][1];
     i0++;
     rsi_array_tmp[i2][1]=-100000;
   }   
   

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Rsi"+symbols_array[i],OBJPROP_TEXT,StringFormat("Rsi %s : %f",stoch_sorted_index(rsi_array[i][0]),rsi_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Rsi"+symbols_array[i],OBJPROP_YDISTANCE,360+i*15); 
      ObjectSet("Rsi"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(rsi_array[i][1]>=70)
     ObjectSetInteger(current_chart_id,"Rsi"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(rsi_array[i][1]<=30)
     ObjectSetInteger(current_chart_id,"Rsi"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Rsi"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }      
     
  if(prev_h1_stoch != stoch_array[0][0]){
   prev_h1_stoch = stoch_array[0][0];
   if(Period()==60)PlaySound("h1_stoch.wav");else
   if(Period()==240)PlaySound("h4_stoch.wav");
  }    
  if(prev_h1_rsi != rsi_array[0][0]){
   prev_h1_rsi = rsi_array[0][0];
   if(Period()==60)PlaySound("h1_rsi.wav");else
   if(Period()==240)PlaySound("h4_rsi.wav");
  }  
  // Sleep(1);
  // return(0); 
  }