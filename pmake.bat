@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=*" %%i in (`%cd%/tools/win/vswhere -latest -property catalog_productLineVersion`) do (
  set CONFIG=vs%%i
)

tools\win\premake5 %CONFIG%
timeout /t 3 /nobreak

endlocal