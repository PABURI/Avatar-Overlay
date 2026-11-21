@echo off
rem 부하 측정용 가짜 Spout Sender 빌드 (개발용).  결과물: build\tools\test_sender.exe
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1 || exit /b 1
set ROOT=%~dp0..\
set SP=%ROOT%third_party\Spout2\SPOUTSDK
if not exist "%ROOT%build\tools" mkdir "%ROOT%build\tools"
cl /nologo /std:c++17 /O2 /MT /EHsc /utf-8 /DNDEBUG /DSPOUT_BUILD_STATIC /DNOMINMAX /W3 ^
  /I"%SP%\SpoutDirectX\SpoutDX" /I"%SP%\SpoutGL" ^
  /Fo"%ROOT%build\tools\\" /Fe"%ROOT%build\tools\test_sender.exe" ^
  "%~dp0test_sender.cpp" "%SP%\SpoutDirectX\SpoutDX\SpoutDX.cpp" ^
  "%SP%\SpoutGL\SpoutCopy.cpp" "%SP%\SpoutGL\SpoutDirectX.cpp" "%SP%\SpoutGL\SpoutFrameCount.cpp" ^
  "%SP%\SpoutGL\SpoutSenderNames.cpp" "%SP%\SpoutGL\SpoutSharedMemory.cpp" "%SP%\SpoutGL\SpoutUtils.cpp" ^
  /link /IMPLIB:"%ROOT%build\tools\ts.lib" user32.lib gdi32.lib shell32.lib d3d11.lib dxgi.lib ole32.lib advapi32.lib winmm.lib version.lib shlwapi.lib comdlg32.lib
