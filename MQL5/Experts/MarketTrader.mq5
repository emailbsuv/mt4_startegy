#property copyright "https://t.me/markettraderoptimizer"
#property link      "https://t.me/markettraderoptimizer"
#property description "MarketTrader v0.1 TensorFlow"
#property strict
#property version   "1.00"
#import "MarketTraderTensorFlow.dll"
  void PythonTensorFlow(string& s, int len);
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//---
   string s="";
   s=TerminalInfoString(TERMINAL_DATA_PATH)+"\\MQL5\\Files\\";int len = StringLen(s);
   PythonTensorFlow(s,len);
   Print("complete...");
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
void OnTick()
  {
//---
   
  }
//+------------------------------------------------------------------+
