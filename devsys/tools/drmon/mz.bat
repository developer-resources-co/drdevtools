bmake -DSYSTEM=GEN -DZORTECH=z -fdrmon.mak
if exist drmon.mak goto done
copy err ..
del err
cd ..
:done
