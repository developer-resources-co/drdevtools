make -a >err
if exist err goto :done
cd ..
del untab.exe
:done
type err
