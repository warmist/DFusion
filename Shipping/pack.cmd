@echo off
copy readme.txt tmp\
copy ..\test.script tmp\
copy ..\bin\release\* tmp\
copy ..\plugins tmp\plugins
copy ..\offsets* tmp\
copy ..\init.lua tmp\
xcopy /s ..\plugins tmp\plugins\

pause