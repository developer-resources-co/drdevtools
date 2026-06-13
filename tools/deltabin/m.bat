make >err
if exist err goto :done
cd ..
del deltabin.exe
:done
type err
