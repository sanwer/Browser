@Echo Off
Echo Packaging binary
IF EXIST .\Bin ( PUSHD .\Bin ) else ( PUSHD %CD% )
IF EXIST Browser.zip DEL /F /Q /S Browser.zip
SET FILES=locales\en-US.pak locales\zh-CN.pak
IF EXIST swiftshader ( SET FILES=%FILES% swiftshader )
SET FILES=%FILES% cef.pak cef_100_percent.pak cef_200_percent.pak cef_extensions.pak
IF EXIST devtools_resources.pak ( SET FILES=%FILES% devtools_resources.pak )
SET FILES=%FILES% d3dcompiler_43.dll d3dcompiler_47.dll libcef.dll libEGL.dll libGLESv2.dll
IF EXIST chrome_elf.dll ( SET FILES=%FILES% chrome_elf.dll )
SET FILES=%FILES% icudtl.dat natives_blob.bin snapshot_blob.bin
IF EXIST v8_context_snapshot.bin ( SET FILES=%FILES% v8_context_snapshot.bin )
Rem 7z.exe a Browser.zip Browser.exe locales\en-US.pak locales\zh-CN.pak swiftshader cef.pak cef_100_percent.pak cef_200_percent.pak cef_extensions.pak chrome_elf.dll d3dcompiler_43.dll d3dcompiler_47.dll icudtl.dat libcef.dll libEGL.dll libGLESv2.dll natives_blob.bin snapshot_blob.bin v8_context_snapshot.bin
7z.exe a Browser.zip Browser.exe %FILES%
IF "%ERRORLEVEL%"=="1" GOTO ErrorPackage
GOTO EndPackage

:ErrorPackage
Echo [ERROR]: There is an error in packing binaries.
PAUSE

:EndPackage
POPD
