#include <windows.h>
#include <Python.h>

int main() {

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