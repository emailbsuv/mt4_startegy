//+------------------------------------------------------------------+
//|                                                   UpdateBars.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict
#property indicator_chart_window

string config[200];int cindex=0;
int tfs[5]={15,30,60,240,1440};
//int tfs[2]={1,5};
int bars=1500;
bool barsupdated=true;
bool filessaved=false;
//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- indicator buffers mapping
   int filehandle=FileOpen("symbolset.txt",FILE_READ|FILE_TXT);
   FileSeek(filehandle,0,SEEK_SET);
   int tmp02=0;
   if(filehandle!=INVALID_HANDLE)
     {
         while(!FileIsEnding(filehandle))
           {
            config[cindex]=FileReadString(filehandle);
            cindex++;
           }
     }
     FileClose(filehandle);   
//---
   EventSetTimer(1); 
   return(INIT_SUCCEEDED);
  }
void FilesDelete() 
  { 
   string   file_name;      // переменная для хранения имен файлов 
   string   filter="hst\\*.hst"; // фильтр для поиска файлов 
   datetime create_date;    // дата создания файла 
   string   files[];        // список имен файлов 
   int      def_size=2500;    // размер массива по умолчанию 
   int      size=0;         // количество файлов 
//--- выдели память для массива 
   ArrayResize(files,def_size); 
//--- получение хэндла поиска в корне локальной папки 
   long search_handle=FileFindFirst(filter,file_name); 
//--- проверим, успешно ли отработала функция FileFindFirst() 
   if(search_handle!=INVALID_HANDLE) 
     { 
      do 
        { 
         files[size]=file_name; 
         size++; 
         if(size==def_size) 
           { 
            def_size+=25; 
            ArrayResize(files,def_size); 
           } 
         ResetLastError(); 
         FileDelete("hst\\"+file_name); 

        } 
      while(FileFindNext(search_handle,file_name)); 
      FileFindClose(search_handle); 
     } 
   else 
     { 
      return; 
     } 
  }
void SaveHistory(string sm, int period0){
  int digits=MarketInfo(sm,MODE_DIGITS);
  int ExtHandle=-1;
SymbolSelect(sm,true);
   
   
   //return;
   
   ExtHandle=FileOpen("hst\\"+sm+period0+".hst",FILE_BIN|FILE_WRITE|FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_ANSI);
   FileWriteInteger(ExtHandle,digits);
   for(int i=bars-1;i>=0;i--){
      FileWriteInteger(ExtHandle,iTime(sm,period0,i));
      FileWriteDouble(ExtHandle,iOpen(sm,period0,i));
      FileWriteDouble(ExtHandle,iHigh(sm,period0,i));
      FileWriteDouble(ExtHandle,iLow(sm,period0,i));
      FileWriteDouble(ExtHandle,iClose(sm,period0,i));
      FileWriteDouble(ExtHandle,iVolume(sm,period0,i));
   }
   FileClose(ExtHandle);
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
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
  {
//---


   barsupdated=true;
   int i,i1,i2,i4;double i3;string mode;
   for(i=0;i<cindex;i++)
   for(i2=0;i2<ArraySize(tfs);i2++)
   {
      mode=""+config[i]; SymbolSelect(mode,true);
      if(ObjectFind(ChartID(),mode)<0)ObjectCreate(mode, OBJ_LABEL, 0,0,0);
      ObjectSet(mode, OBJPROP_CORNER, 0);
      ObjectSet(mode, OBJPROP_XDISTANCE, 10);
      ObjectSet(mode, OBJPROP_YDISTANCE, i*15);
      i1=0;
      //while(i1<bars){
         for(i4=0;i4<=bars;i4++)i3=iClose(mode,tfs[i2],bars+1);//iMA(mode,tfs[i2],3,0,MODE_SMA,PRICE_CLOSE,bars+1);
         i1=iBars(mode,tfs[i2]);
         if(i1>=bars)ObjectSetText(mode,mode+" : "+tfs[i2]+" "+i1, 8, "Arial Narrow", clrGreen);
         else {
            ObjectSetText(mode,mode+" : "+tfs[i2]+" "+i1, 8, "Arial Narrow", clrGray);
            barsupdated=false;
            break;
        //    Sleep(1000);
         }
      //}
   }
   if(barsupdated&&!filessaved){
     //FilesDelete();
     //for(i=0;i<cindex;i++)
     // for(i2=0;i2<ArraySize(tfs);i2++)
     //   SaveHistory(config[i],tfs[i2]); filessaved=true;
     
      mode="Status";
      if(ObjectFind(ChartID(),mode)<0)ObjectCreate(mode, OBJ_LABEL, 0,0,0);
      ObjectSet(mode, OBJPROP_CORNER, 0);
      ObjectSet(mode, OBJPROP_XDISTANCE, 110);
      ObjectSet(mode, OBJPROP_YDISTANCE, 0);
      ObjectSetText(mode,"saved "+TimeToStr(TimeLocal(),TIME_DATE|TIME_SECONDS), 8, "Arial Narrow", clrRed);
     
   }   
  }
//+------------------------------------------------------------------+
