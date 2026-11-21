@echo off
rem 코드 서명 스크립트 (인증서를 발급받은 뒤 사용).  사용법:  tools\sign.bat <서명할 exe 경로>
rem 인증서는 Windows 인증서 저장소(또는 하드웨어 토큰)에 설치되어 있어야 하며, 지문(thumbprint)을 환경변수로 지정한다.
rem   set SIGN_THUMBPRINT=<인증서 SHA1 지문>
rem 비밀번호나 개인키 파일을 이 스크립트에 적지 않는다. (토큰/저장소가 직접 처리)
setlocal
if "%~1"=="" (echo 사용법: sign.bat ^<exe 경로^> & exit /b 1)
if "%SIGN_THUMBPRINT%"=="" (echo SIGN_THUMBPRINT 환경변수가 필요합니다. & exit /b 1)
set ST=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe
if not exist "%ST%" (echo signtool.exe 를 찾을 수 없습니다: %ST% & exit /b 1)
rem 타임스탬프를 반드시 포함해야 인증서가 만료된 뒤에도 서명이 유효하다.
"%ST%" sign /sha1 %SIGN_THUMBPRINT% /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 /d "Avatar Overlay" "%~1" || exit /b 1
"%ST%" verify /pa /v "%~1"
