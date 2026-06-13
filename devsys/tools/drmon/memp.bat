set lib=x:\bc3\lib;t:\lib
set include=t:\include;x:\inc

bmake -DSYSTEM=GEN -DEMUL=E -DPHARLAP=p -fdrmon.mak
if exist drmon.mak goto done
copy err ..
del err
cd ..
:done
