bmake -DSYSTEM=GEN -DEMUL=E -DZORTECH=z -fdrmon.mak >err
if exist drmon.mak goto done
copy err ..
del err
cd ..
:done
type err

