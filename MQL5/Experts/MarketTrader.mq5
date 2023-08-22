#property copyright "https://t.me/markettraderoptimizer"
#property link      "https://t.me/markettraderoptimizer"
#property description "MarketTrader v0.11 TensorFlow"
#property strict
#property version   "0.11"
#import "MarketTraderTensorFlow.dll"
  int PythonTensorFlow(string& s, int len);
#import
input double lots=0.01;

string config[100][2];
string GetElement(string str, int index){

   string arr[];
   StringSplit(str,' ',arr);
   if(index<ArraySize(arr))return arr[index];
   else return "false";
}
int cindex;
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {

   
   cindex=0; 
   for(int u1=0;u1<SymbolsTotal(true);u1++){
      config[cindex][0]=SymbolName(u1,true);
      cindex++;
   }  
   for (int i = 0; i < ObjectsTotal(ChartID()); i++)
    {
        ObjectDelete(ChartID(),ObjectName(ChartID(),i));
    } 
    ChartRedraw(ChartID());
   isworking=false;
   isworking1=false;
   EventSetTimer(10);
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//---
   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
int timer1=2,timer2=3,timer1limit1=2;
bool barsupdated1=false;
bool barsupdated=false;
int bars=1400;
#define BARS_TO_SAVE 1398
bool isworking=false;
bool isworking1=false;
int r1=0;
void OnTick()
  {
  }
void OnTimer()
  {
//---
         string mode;
         timer1=timer1-1;
         
         if(timer1<1){
           timer1=timer1limit1;
            barsupdated=true;
            int i,i1,i2,i4,i5;double i3;int x=0,y=0;
            for(i=0;i<cindex;i++){
               i2=1440;
               {
                  mode=""+config[i][0]; SymbolSelect(mode,true);
                  if(ObjectFind(ChartID(),mode)<0)ObjectCreate(ChartID(),mode, OBJ_LABEL, 0,0,0);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_CORNER, 0);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_XDISTANCE, x*200);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_YDISTANCE, 120+y*15);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_BACK, true);
                  
                  i1=0;
                     for(i4=0;i4<=bars;i4++)i3=iClose(mode,PERIOD_D1,i4);
                     i5=iBars(mode,PERIOD_D1);
                     if(i5>=bars){ObjectSetString(ChartID(),mode,OBJPROP_TEXT,mode+" : "+i2+" "+i5);
                                    ObjectSetInteger(ChartID(),mode,OBJPROP_COLOR,clrGreen);

                     }
                     else {
                        ObjectSetString(ChartID(),mode,OBJPROP_TEXT,mode+" : "+i2+" "+i1);
                        ObjectSetInteger(ChartID(),mode,OBJPROP_COLOR,clrGray);
                        barsupdated=false;
                     }
               }
               i2=1440;
               {
                  mode=""+config[i][0]; SymbolSelect(mode,true);
                  if(ObjectFind(ChartID(),mode)<0)ObjectCreate(ChartID(),mode, OBJ_LABEL, 0,0,0);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_CORNER, 0);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_XDISTANCE, x*200);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_YDISTANCE, 120+y*15);
                  ObjectSetInteger(ChartID(),mode, OBJPROP_BACK, true);
                  
                  if(ObjectGetInteger(ChartID(),mode, OBJPROP_COLOR)==clrGreen){
                     i1=0;
                     for(i4=0;i4<=bars;i4++)i3=iClose(mode,PERIOD_D1,i4);
                     i1=iBars(mode,PERIOD_D1);
                     if(i1>=bars){ObjectSetString(ChartID(),mode,OBJPROP_TEXT,mode+" : "+i2+" "+i1);
                                 ObjectSetInteger(ChartID(),mode,OBJPROP_COLOR,clrGreen);
                     
                     }
                     else {
                        ObjectSetString(ChartID(),mode,OBJPROP_TEXT,mode+" : "+i2+" "+i1);
                        ObjectSetInteger(ChartID(),mode,OBJPROP_COLOR,clrGray);
                        barsupdated=false;
                     }
                  }
               }
               y=y+1; if(y==30){y=0;x=x+1;}
            }         
           if(barsupdated)barsupdated1=true;
           ChartRedraw(ChartID());
         }  
         timer2=timer2-1;
         if(timer2<1){
           timer2=3;
           if(barsupdated1&&!isworking&&!isworking1&&r1==0){
            if(isWorkday()){
               isworking=true;
               DumpOHLCVals();
               
               isworking1=true;
               r1=0;
               ReadPredictAndOpenOrders();
               }
           }
           //if(isworking&&!isworking1&&r1==1&&isWorkday1()){
           //  isworking1=true;
           //  r1=0;
           //  ReadPredictAndOpenOrders();             
           //}
         }            
  }
void ReadPredictAndOpenOrders(){
   MqlTradeRequest request={};
   MqlTradeResult  result={};
         int file_handle=FileOpen("predict.txt",FILE_READ|FILE_TXT|FILE_ANSI); 
         string buy_vals=FileReadString(file_handle); 
         string sell_vals=FileReadString(file_handle); 
         FileClose(file_handle);
         string symbol="";int i=0;
         while((symbol=GetElement(buy_vals,i))!="false"){
            i++;
            request.action   =TRADE_ACTION_DEAL;
            request.symbol   =symbol;
            request.volume   =lots;
            request.type     =ORDER_TYPE_BUY;
            request.price    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_ASK),(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.sl    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_ASK)-pow(10,-(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS))*1250,(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.tp    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_ASK)+pow(10,-(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS))*250,(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.deviation=15;
            if(!OrderSend(request,result))PrintFormat("OrderSend error %d",GetLastError());
            //else
               //ticket =result.order;
         }
         i=0;
         while((symbol=GetElement(sell_vals,i))!="false"){
            i++;
            request.action   =TRADE_ACTION_DEAL;
            request.symbol   =symbol;
            request.volume   =lots;
            request.type     =ORDER_TYPE_SELL;
            request.price    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_BID),(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.sl    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_BID)+pow(10,-(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS))*1250,(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.tp    =NormalizeDouble(SymbolInfoDouble(symbol,SYMBOL_BID)-pow(10,-(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS))*250,(int)SymbolInfoInteger(symbol,SYMBOL_DIGITS));
            request.deviation=15;
            if(!OrderSend(request,result))PrintFormat("OrderSend error %d",GetLastError());
            //else
               //ticket =result.order;
         } 
         //isworking=false;
         //isworking1=false;
}
bool isWorkday() {
   datetime tc = TimeCurrent();
   MqlDateTime  dt_struct;
   TimeToStruct(tc,dt_struct);
  int dayOfWeek = dt_struct.day_of_week;
//  return ((dt_struct.hour==23 && dt_struct.min>=0 && dt_struct.min<=15)&&(dayOfWeek == 1 || dayOfWeek == 2 || dayOfWeek == 3 || dayOfWeek == 4 || dayOfWeek == 5));
  return ((dt_struct.hour==23 || dt_struct.hour==22)&&(dayOfWeek == 1 || dayOfWeek == 2 || dayOfWeek == 3 || dayOfWeek == 4 || dayOfWeek == 5));
}
bool isWorkday1() {
   datetime tc = TimeCurrent();
   MqlDateTime  dt_struct;
   TimeToStruct(tc,dt_struct);
  int dayOfWeek = dt_struct.day_of_week;
//  return ((dt_struct.hour==0 && dt_struct.min>=20 && dt_struct.min<=40)&&(dayOfWeek == 1 || dayOfWeek == 2 || dayOfWeek == 3 || dayOfWeek == 4 || dayOfWeek == 5));
  return ((dt_struct.hour==23 || dt_struct.hour==22)&&(dayOfWeek == 1 || dayOfWeek == 2 || dayOfWeek == 3 || dayOfWeek == 4 || dayOfWeek == 5));
}
void DumpOHLCVals(){

    for (int i = 0; i < cindex; i++)
    {
        string _Symbol1 = config[i][0];
        ENUM_TIMEFRAMES  _Period1 = PERIOD_D1;
if(iBars(_Symbol1, _Period1)>=bars){         
    
    
   // Открытие файла для записи
   int file_handle = FileOpen(_Symbol1+"_D1.txt", FILE_WRITE|FILE_TXT|FILE_ANSI);
   if (file_handle == INVALID_HANDLE)
     {
      Print("Ошибка при открытии файла!");
     }


   // Цикл для сохранения данных
   for (int i = BARS_TO_SAVE+1; i >= 1; i--)
     {
      double open   = iOpen(_Symbol1, _Period1, i);
      double high   = iHigh(_Symbol1, _Period1, i);
      double low    = iLow(_Symbol1, _Period1, i);
      double close  = iClose(_Symbol1, _Period1, i);

      // Запись данных в файл
      FileWrite(file_handle, ""+DoubleToString(open,5)+", "+DoubleToString(high,5)+", "+DoubleToString(low,5)+", "+DoubleToString(close,5)+"");
     }
      FileWriteString(file_handle, ""+DoubleToString(iClose(_Symbol1, _Period1, 0),5)+", "+DoubleToString(iClose(_Symbol1, _Period1, 0),5)+", "+DoubleToString(iClose(_Symbol1, _Period1, 0),5)+", "+DoubleToString(iClose(_Symbol1, _Period1, 0),5)+"");
   // Закрытие файла
   FileClose(file_handle);

}
}

   string s="";
   s=TerminalInfoString(TERMINAL_DATA_PATH)+"\\MQL5\\Files\\";int len = StringLen(s);

   r1 = PythonTensorFlow(s,len);
   //Print(r1," complete...");
   //

}
//+------------------------------------------------------------------+
