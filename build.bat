@echo off
rem Avatar Overlay 빌드.  사용법: build.bat
rem   결과물: AvatarOverlay\AvatarOverlay.exe   (README.txt 와 같은 폴더에 생성됨)
rem   버전:   VERSION 파일 (예: 0.1.0) 한 곳에서만 관리
setlocal
set ROOT=%~dp0
set /p VER=<"%ROOT%VERSION"
for /f "tokens=1-3 delims=." %%a in ("%VER%") do set VN=%%a,%%b,%%c,0
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1 || exit /b 1

set SP=%ROOT%third_party\Spout2\SPOUTSDK
set OBJ=%ROOT%build\obj
set OUT=%ROOT%AvatarOverlay
if not exist "%OBJ%" mkdir "%OBJ%"
if not exist "%OUT%" mkdir "%OUT%"

> "%ROOT%build\version.h" echo #define VER_NUM %VN%
>> "%ROOT%build\version.h" echo #define VER_STR "%VER%"
rc /nologo /i "%ROOT%build" /fo "%OBJ%\app.res" "%ROOT%src\resources\app.rc" || exit /b 1

cl /nologo /std:c++17 /O2 /MT /EHsc /utf-8 /DNDEBUG /DSPOUT_BUILD_STATIC /DUNICODE /D_UNICODE /DNOMINMAX /W3 ^
  /I"%SP%\SpoutDirectX\SpoutDX" /I"%SP%\SpoutGL" ^
  /Fo"%OBJ%\\" /Fe"%OUT%\AvatarOverlay.exe" ^
  "%ROOT%src\main.cpp" "%OBJ%\app.res" ^
  "%SP%\SpoutDirectX\SpoutDX\SpoutDX.cpp" ^
  "%SP%\SpoutGL\SpoutCopy.cpp" "%SP%\SpoutGL\SpoutDirectX.cpp" "%SP%\SpoutGL\SpoutFrameCount.cpp" ^
  "%SP%\SpoutGL\SpoutSenderNames.cpp" "%SP%\SpoutGL\SpoutSharedMemory.cpp" "%SP%\SpoutGL\SpoutUtils.cpp" ^
  /link /SUBSYSTEM:WINDOWS /MANIFEST:EMBED /IMPLIB:"%OBJ%\ao.lib" ^
  user32.lib gdi32.lib shell32.lib d3d11.lib d3dcompiler.lib dxgi.lib dcomp.lib ole32.lib advapi32.lib winmm.lib version.lib shlwapi.lib comdlg32.lib
