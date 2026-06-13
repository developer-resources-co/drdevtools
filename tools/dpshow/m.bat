make >err
if exist err goto done
cd ..
del dpshow.exe

:done
type err
