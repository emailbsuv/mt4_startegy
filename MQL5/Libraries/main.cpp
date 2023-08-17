#include <windows.h>
#include <Python.h>
#include <iostream>
#include <unistd.h>
#include <fstream>

#pragma comment(lib, "python311.lib")

extern "C" __declspec(dllexport) void __stdcall PythonTensorFlow(wchar_t* msg,int len) {
	size_t convertedChars = 0;
	char paramsT[500];
	wcstombs_s(&convertedChars, paramsT, len+1, msg, _TRUNCATE);

  SetCurrentDirectory(paramsT);

  
  char* terminal = new char[500];memset(terminal,0,500);lstrcat(terminal,paramsT);lstrcat(terminal,"MarketTraderTensorFlow.exe");
 // _execlp(terminal, "cmd.exe", "/c", "", "", "", nullptr);
 // _execlp(terminal, terminal, "", "", "", "", nullptr);

    int result = _spawnlp(P_WAIT, terminal, terminal, "/c", "", nullptr);
    
    if (result == -1) {
        // Обработка ошибок
        //perror("Error running command");
    } 
 
  return; 
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