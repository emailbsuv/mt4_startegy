#include <windows.h>
#include <Python.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstdlib>

#pragma comment(lib, "python311.lib")

extern "C" __declspec(dllexport) int __stdcall PythonTensorFlow(wchar_t* msg,int len) {
	size_t convertedChars = 0;
	char paramsT[500];
	wcstombs_s(&convertedChars, paramsT, len+1, msg, _TRUNCATE);

  SetCurrentDirectory(paramsT);

  
  char* terminal = new char[500];memset(terminal,0,500);
  lstrcat(terminal,paramsT);
  lstrcat(terminal,"MarketTraderTensorFlow.exe");
  //lstrcat(terminal,"C:\\Users\\User\\AppData\\Roaming\\MetaQuotes\\Terminal\\9EB2973C469D24060397BB5158EA73A5\\MQL5\\Files\\
MarketTraderTensorFlow.exe");
 // _execlp(terminal, "cmd.exe", "pause", "", "", "", nullptr);
 // _execlp(terminal, terminal, "", "", "", "", nullptr);

    // int result = _spawnlp(P_WAIT, "cmd.exe", terminal, "/c", "", nullptr);
	 int result = system(terminal);
    
    if (result == 1) {
        return 1;
    }  
 
  return 0; 
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString("DLL_PROCESS_ATTACH");
        break;

    case DLL_THREAD_ATTACH:
        OutputDebugString("DLL_THREAD_ATTACH");
        break;

    case DLL_THREAD_DETACH:
        OutputDebugString("DLL_THREAD_DETACH");
        break;

    case DLL_PROCESS_DETACH:
        OutputDebugString("DLL_PROCESS_DETACH");
        break;
    }

    return TRUE;
} 