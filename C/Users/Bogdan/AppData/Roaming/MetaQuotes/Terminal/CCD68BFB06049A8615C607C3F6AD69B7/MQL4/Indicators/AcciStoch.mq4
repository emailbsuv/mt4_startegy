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
double stoch_array[23][2],stoch1_array[23][2]; 
double cci_array[23][2],cci1_array[23][2];
double tmp_array[23][2];
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
     if(!ObjectCreate(current_chart_id,"Cci"+symbols_array[i],OBJ_LABEL,0,0,0)) 
      { 
       Print("Ошибка создания объекта: code #",GetLastError()); 
       return(0); 
      } 
   }   
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Stoch1"+symbols_array[i],OBJ_LABEL,0,0,0)) 
      { 
       Print("Ошибка создания объекта: code #",GetLastError()); 
       return(0); 
      } 
   }
//--- создаем объект типа label 
   for(i=0; i<23; i++){
     if(!ObjectCreate(current_chart_id,"Cci1"+symbols_array[i],OBJ_LABEL,0,0,0)) 
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
     stoch_array[i][1]=iStochastic(symbols_array[i],0,333,3,3,MODE_SMA,0,MODE_MAIN,0);
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
     
//================ C C I ===================

   for(i=0; i<23; i++){
     cci_array[i][0]=i;
     cci_array[i][1]=iRSI(symbols_array[i],0,333,PRICE_CLOSE,0);//iCCI(symbols_array[i],0,333,PRICE_TYPICAL,0);
   }
   double cci_array_tmp[23][2];
   for(i=0; i<23; i++){
     cci_array_tmp[i][0]=cci_array[i][0];
     cci_array_tmp[i][1]=cci_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( cci_array_tmp[i1][1] > i3){
         i3 = cci_array_tmp[i1][1];
         i2=i1;
       }
     }
     cci_array[i0][0]=cci_array_tmp[i2][0];
     cci_array[i0][1]=cci_array_tmp[i2][1];
     i0++;
     cci_array_tmp[i2][1]=-100000;
   }   
   

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Cci"+symbols_array[i],OBJPROP_TEXT,StringFormat("Rsi %s : %f",stoch_sorted_index(cci_array[i][0]),cci_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Cci"+symbols_array[i],OBJPROP_YDISTANCE,360+i*15); 
      ObjectSet("Cci"+symbols_array[i],OBJPROP_XDISTANCE,5);
//--- устанавливаем цвет Red 
     if(cci_array[i][1]>=52)
     ObjectSetInteger(current_chart_id,"Cci"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(cci_array[i][1]<=48)
     ObjectSetInteger(current_chart_id,"Cci"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Cci"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }      
 
 
//==================== P R E V B A R =================

   for(i=0; i<23; i++){
     stoch1_array[i][0]=i;
     stoch1_array[i][1]=iStochastic(symbols_array[i],0,333,3,3,MODE_SMA,0,MODE_MAIN,1);
   }
   double stoch1_array_tmp[23][2];
   for(i=0; i<23; i++){
     stoch1_array_tmp[i][0]=stoch1_array[i][0];
     stoch1_array_tmp[i][1]=stoch1_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( stoch1_array_tmp[i1][1] > i3){
         i3 = stoch1_array_tmp[i1][1];
         i2=i1;
       }
     }
     stoch1_array[i0][0]=stoch1_array_tmp[i2][0];
     stoch1_array[i0][1]=stoch1_array_tmp[i2][1];
     i0++;
     stoch1_array_tmp[i2][1]=-100000;
   }   
//---- sort -----------------
   for(i=0; i<23; i++)for(i1=0; i1<23; i1++)if(stoch1_array[i1][0]==stoch_array[i][0]){
    tmp_array[i][1]=stoch_array[i][1]-stoch1_array[i1][1];
    tmp_array[i][0]=stoch_array[i][0];
   }
   for(i=0; i<23; i++){
    stoch1_array[i][0]=tmp_array[i][0];
    stoch1_array[i][1]=tmp_array[i][1];
   }
    

//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Stoch1"+symbols_array[i],OBJPROP_TEXT,StringFormat("%.2f",stoch1_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Stoch1"+symbols_array[i],OBJPROP_YDISTANCE,i*15); 
      ObjectSet("Stoch1"+symbols_array[i],OBJPROP_XDISTANCE,180+5);
//--- устанавливаем цвет Red 
     if(stoch1_array[i][1]>0)
     ObjectSetInteger(current_chart_id,"Stoch1"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(stoch1_array[i][1]<0)
     ObjectSetInteger(current_chart_id,"Stoch1"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Stoch1"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }
     
//================ C C I ===================

   for(i=0; i<23; i++){
     cci1_array[i][0]=i;
     cci1_array[i][1]=iRSI(symbols_array[i],0,333,PRICE_CLOSE,1);//iCCI(symbols_array[i],0,333,PRICE_TYPICAL,1);
   }
   double cci1_array_tmp[23][2];
   for(i=0; i<23; i++){
     cci1_array_tmp[i][0]=cci1_array[i][0];
     cci1_array_tmp[i][1]=cci1_array[i][1];
   }
   i0=0;
   for(i=0; i<23; i++){
     double i3=-99999;
     for(i1=0; i1<23; i1++){
       if( cci1_array_tmp[i1][1] > i3){
         i3 = cci1_array_tmp[i1][1];
         i2=i1;
       }
     }
     cci1_array[i0][0]=cci1_array_tmp[i2][0];
     cci1_array[i0][1]=cci1_array_tmp[i2][1];
     i0++;
     cci1_array_tmp[i2][1]=-100000;
   }   
//---- sort -----------------
   for(i=0; i<23; i++)for(i1=0; i1<23; i1++)if(cci1_array[i1][0]==cci_array[i][0]){
    tmp_array[i][1]=cci_array[i][1]-cci1_array[i1][1];
    tmp_array[i][0]=cci_array[i][0];
   }
   
   for(i=0; i<23; i++){
    cci1_array[i][0]=tmp_array[i][0];
    cci1_array[i][1]=tmp_array[i][1];
   }   
//--- перемещение объекта label и изменение его текста 
   for(i=0; i<23; i++) 
     { 
      //-- устанавливаем текст объекта 
      ObjectSetString(current_chart_id,"Cci1"+symbols_array[i],OBJPROP_TEXT,StringFormat("%.3f",cci1_array[i][1])); 
      //-- устанавливаем значение координаты Y 
      ObjectSet("Cci1"+symbols_array[i],OBJPROP_YDISTANCE,360+i*15); 
      ObjectSet("Cci1"+symbols_array[i],OBJPROP_XDISTANCE,180+5);
//--- устанавливаем цвет Red 
     if(cci1_array[i][1]>0)
     ObjectSetInteger(current_chart_id,"Cci1"+symbols_array[i],OBJPROP_COLOR,clrAqua); else
     if(cci1_array[i][1]<0)
     ObjectSetInteger(current_chart_id,"Cci1"+symbols_array[i],OBJPROP_COLOR,clrRed); else
     ObjectSetInteger(current_chart_id,"Cci1"+symbols_array[i],OBJPROP_COLOR,clrYellow); 
      //-- перерисовка графика 
      ChartRedraw(current_chart_id); 
     }      
 


//==================================================== 
 
 
 
 
     
  //if(prev_h1_stoch != stoch_array[0][0]){
  // prev_h1_stoch = stoch_array[0][0];
  // if(Period()==60)PlaySound("h1_stoch.wav");else
  // if(Period()==240)PlaySound("h4_stoch.wav");
  //}    
  //if(prev_h1_rsi != cci_array[0][0]){
  // prev_h1_rsi = cci_array[0][0];
  // if(Period()==60)PlaySound("h1_rsi.wav");else
  // if(Period()==240)PlaySound("h4_rsi.wav");
  //}  
  // Sleep(1);
  // return(0); 
  }