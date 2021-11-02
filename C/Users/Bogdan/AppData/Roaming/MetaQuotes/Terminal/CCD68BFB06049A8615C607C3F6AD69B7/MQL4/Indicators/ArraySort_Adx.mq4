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
double adxp_array[23][2]; 
double adxm_array[23][2];
long current_chart_id;
int prev_h1_adxp,prev_h1_adxm;
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
int i;
current_chart_id=ChartID();
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Adxp"+symbols_array[i],OBJ_LABEL,0,0,0)) 
      { 
       Print("Ошибка создания объекта: code #",GetLastError()); 
       return(0); 
      } 
   }
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Adxm"+symbols_array[i],OBJ_LABEL,0,0,0)) 
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
     adxp_array[i][0]=i;
     adxp_array[i][1]=iADX(symbols_array[i],0,14,PRICE_HIGH,MODE_PLUSDI,0);//iCCI(symbols_array[i],0,14,PRICE_TYPICAL,0);//iMomentum(symbols_array[i],0,14,PRICE_CLOSE,0);//iStochastic(symbols_array[i],0,8,3,3,MODE_SMA,0,MODE_MAIN,0);
   }
   double adxp_array_tmp[23][2];
   for(i=0; i<23; i++){
     adxp_array_tmp[i][0]=adxp_array[i][0];
     adxp_array_tmp[i][1]=adxp_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( adxp_array_tmp[i1][1] > i3){
         i3 = adxp_array_tmp[i1][1];
         i2=i1;
       }
     }
     adxp_array[i0][0]=adxp_array_tmp[i2][0];
     adxp_array[i0][1]=adxp_array_tmp[i2][1];
     i0++;
     adxp_array_tmp[i2][1]=-100000;
   }   
   
    

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Adxp"+symbols_array[i],OBJPROP_TEXT,StringFormat("Adxp %s : %f",stoch_sorted_index(adxp_array[i][0]),adxp_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Adxp"+symbols_array[i],OBJPROP_YDISTANCE,i*15); 
      ObjectSet("Adxp"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(adxp_array[i][1]>=20)
     ObjectSetInteger(current_chart_id,"Adxp"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(adxp_array[i][1]<=10)
     ObjectSetInteger(current_chart_id,"Adxp"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Adxp"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }
     
//================ R V I ===================

   for(i=0; i<23; i++){
     adxm_array[i][0]=i;
     adxm_array[i][1]=iADX(symbols_array[i],0,14,PRICE_HIGH,MODE_MINUSDI,0);;//iRVI(symbols_array[i],0,10,MODE_MAIN,0);//iForce(symbols_array[i],0,13,MODE_SMA,PRICE_CLOSE,0);//iRSI(symbols_array[i],0,8,PRICE_CLOSE,0);
   }
   double adxm_array_tmp[23][2];
   for(i=0; i<23; i++){
     adxm_array_tmp[i][0]=adxm_array[i][0];
     adxm_array_tmp[i][1]=adxm_array[i][1];
   }
   i0=22;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( adxm_array_tmp[i1][1] > i3){
         i3 = adxm_array_tmp[i1][1];
         i2=i1;
       }
     }
     adxm_array[i0][0]=adxm_array_tmp[i2][0];
     adxm_array[i0][1]=adxm_array_tmp[i2][1];
     i0--;
     adxm_array_tmp[i2][1]=-100000;
   }   
   

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Adxm"+symbols_array[i],OBJPROP_TEXT,StringFormat("Adxm %s : %f",stoch_sorted_index(adxm_array[i][0]),adxm_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Adxm"+symbols_array[i],OBJPROP_YDISTANCE,360+i*15); 
      ObjectSet("Adxm"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(adxm_array[i][1]<=10)
     ObjectSetInteger(current_chart_id,"Adxm"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(adxm_array[i][1]>=20)
     ObjectSetInteger(current_chart_id,"Adxm"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Adxm"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     } 
          
  if(prev_h1_adxp != adxp_array[0][0]){
   prev_h1_adxp = adxp_array[0][0];
   if(Period()==60)PlaySound("h1_adxp.wav");else
   if(Period()==240)PlaySound("h4_adxp.wav");
  }    
  if(prev_h1_adxm != adxm_array[0][0]){
   prev_h1_adxm = adxm_array[0][0];
   if(Period()==60)PlaySound("h1_adxm.wav");else
   if(Period()==240)PlaySound("h4_adxm.wav");
  }      
      
  // Sleep(1);
  // return(0); 
  }