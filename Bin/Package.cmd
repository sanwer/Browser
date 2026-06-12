@Echo Off
SETLOCAL ENABLEDELAYEDEXPANSION
SET BIN_DIR=%~dp0
SET BIN_OUT=%1
IF "%BIN_OUT%"=="" (SET BIN_OUT="%~dp0Browser.zip")
Echo Packaging %BIN_OUT%
SET FILES=Browser.exe
SET FILES=%FILES% locales\
SET FILES=%FILES% chrome_100_percent.pak chrome_200_percent.pak resources.pak
SET FILES=%FILES% chrome_elf.dll d3dcompiler_47.dll dxcompiler.dll dxil.dll
SET FILES=%FILES% libcef.dll libEGL.dll libGLESv2.dll vk_swiftshader.dll vulkan-1.dll
SET FILES=%FILES% icudtl.dat v8_context_snapshot.bin vk_swiftshader_icd.json

IF EXIST %BIN_OUT% DEL /F /Q /S %BIN_OUT% > NUL
PUSHD "%BIN_DIR%x64\"
IF EXIST Browser.exe (
    "%BIN_DIR%7z.exe" a -bsp1 -bso0 %BIN_OUT% %FILES%
)
POPD
IF "%ERRORLEVEL%"=="1" GOTO ErrorPackage
GOTO EndPackage

:ErrorPackage
Echo [ERROR]: There is an error in packing binaries.
PAUSE
ENDLOCAL
EXIT /B 1

:EndPackage
POPD
Echo Packing completed.
ENDLOCAL
