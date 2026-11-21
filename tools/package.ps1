# 배포용 zip 만들기:  powershell -File tools\package.ps1
# AvatarOverlay\ 폴더의 exe + README.txt + THIRD_PARTY_LICENSES.txt 만 releases\AvatarOverlay-<버전>.zip 으로 묶는다.
# (사용자 설정 파일(.ini)은 포함하지 않는다.)
$root = Split-Path $PSScriptRoot -Parent
$ver = (Get-Content (Join-Path $root 'VERSION') -TotalCount 1).Trim()
$src = Join-Path $root 'AvatarOverlay'
$files = 'AvatarOverlay.exe', 'README.txt', 'THIRD_PARTY_LICENSES.txt' | ForEach-Object { Join-Path $src $_ }
foreach ($f in $files) { if (-not (Test-Path $f)) { throw "없음: $f  (먼저 build.bat 을 실행하세요)" } }
$zip = Join-Path $root "releases\AvatarOverlay-$ver.zip"
New-Item -ItemType Directory -Force (Split-Path $zip) | Out-Null
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path $files -DestinationPath $zip
"{0}  ({1:N0} bytes)" -f $zip, (Get-Item $zip).Length
"SHA256: " + (Get-FileHash $zip -Algorithm SHA256).Hash
