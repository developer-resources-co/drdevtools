set lib=\borlandc\lite286\bc3\lib;\borlandc\lib
set include=\borlandc\include;\borlandc\lite286\inv

make -DSYSTEM=GEN -DPHARLAP=p -fdrmon.mak 
if exist drmon.mak goto done
copy err ..
del err
cd ..
:done
rem type err
