bcc >err -ml -c -v -It:\include\;s:\ -Lt:\lib iffdump.cpp
tlink >>err /v /c t:\lib\c0l.obj iffdump.obj,iffdump.exe,,s:\pclib\pclibbl.lib t:\lib\cl.lib
type err
