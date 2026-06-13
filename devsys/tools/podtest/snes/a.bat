sbr
spasm -m -s -d podtest.asm
if errorlevel 1 goto end
snesmon podtest.scr
:end

