make -DSYSTEM=GEN -DEMUL=E -fdrmon.mak >err
if exist err goto done
cd ..
del genemon.exe
:done
type err
