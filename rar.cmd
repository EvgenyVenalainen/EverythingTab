rem Берем имя родительского каталога без полного пути.
for %%I in (.) do set CWD=%%~nI
rem Архивируем.
"C:\Program Files\WinRAR\WinRAR.exe" a -v -r -s -ag- -x*.rar -x*.7z -x*.obj -x*.obj -x*.idb -x*.aps -x*.ncb -x*.pdb -x*.bsc -x*.exp -x*.pch -x*.res -x*.ilk -x*.sbr -x\Debug*\*.* -x*Debug\*.exe -x\Debug*\*.dll %CWD%