#include <windows.h>
#include <Python.h>

#pragma comment(lib, "python311.lib")

int main() {
  char* terminal = new char[500];memset(terminal,0,500);
  lstrcat(terminal,"paramsT");
  Py_Initialize();
  
  PyObject *pName, *pModule, *pFunc;

  // Импортируем модуль Python
  pName = PyUnicode_FromString("ohlc-LSTM_D1");
  pModule = PyImport_Import(pName);

  // Получаем функцию из модуля
  pFunc = PyObject_GetAttrString(pModule, "predict"); 
  
  // Вызываем функцию
  PyObject_CallObject(pFunc, NULL);

  Py_Finalize();

  return 1;
}