make -DDEBUG >err
if exist err goto done
cd ..
del anmsplit.exe

:done
checkout -e anmsplit
type err
