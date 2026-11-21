# 배포용 zip 만들기:  powershell -File tools\package.ps1
# exe + README.txt + THIRD_PARTY_LICENSES.txt + LICENSE.txt 를 releases\AvatarOverlay-<버전>.zip 으로 묶는다.
# (사용자 설정 파일(.ini)은 포함하지 않는다.)
$root = Split-Path $PSScriptRoot -Parent
$ver = (Get-Content (Join-Path $root 'VERSION') -TotalCount 1).Trim()
$src = Join-Path $root 'AvatarOverlay'
foreach ($n in 'AvatarOverlay.exe', 'README.txt', 'THIRD_PARTY_LICENSES.txt') {
    if (-not (Test-Path (Join-Path $src $n))) { throw "없음: $n  (먼저 build.bat 을 실행하세요)" }
}
$stage = Join-Path $root 'build\stage'
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory $stage | Out-Null
foreach ($n in 'AvatarOverlay.exe', 'README.txt', 'THIRD_PARTY_LICENSES.txt') { Copy-Item (Join-Path $src $n) $stage }
Copy-Item (Join-Path $root 'LICENSE') (Join-Path $stage 'LICENSE.txt')
$zip = Join-Path $root "releases\AvatarOverlay-$ver.zip"
New-Item -ItemType Directory -Force (Split-Path $zip) | Out-Null
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip
Remove-Item $stage -Recurse -Force
"{0}  ({1:N0} bytes)" -f $zip, (Get-Item $zip).Length
