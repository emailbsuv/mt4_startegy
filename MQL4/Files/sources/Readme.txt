1. Качаем и устанавливаем MingW 64 bit по ссылке 

http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download

При установке выбираем Seh / x86_64

2. Добавляем системную переменную среды окружения Path в Windows 

C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\

3. Открываем cmd.exe, переходим в папку MQL4/Files/

4. Компилируем исходник в экзешник командой make

5. Используем получившийся файл .exe в текущей папке.

Телеграм автора: @bogdansuvorov