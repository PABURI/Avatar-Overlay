# Avatar Overlay

**A transparent, always-on-top overlay that shows your VRChat avatar (Spout output) on your Windows screen.**
**VRChat 아바타(Spout 출력)를 투명한 항상-위 오버레이로 화면에 띄우는 Windows 프로그램.**
**VRChatのアバター(Spout出力)を透過の最前面オーバーレイとして画面に表示するWindowsアプリ。**

## Download / 다운로드 / ダウンロード

Get the latest zip from **[Releases](https://github.com/PABURI/avatar-overlay/releases)**, unzip, and run `AvatarOverlay.exe`.
Usage guide: [`AvatarOverlay/README.txt`](AvatarOverlay/README.txt) (English / 한국어 / 日本語).
The file is not code-signed, so Windows SmartScreen may show a warning ("More info" > "Run anyway").

## Features / 기능 / 機能

- Transparent avatar overlay from VRChat's Spout output, always on top, clicks pass through / 투명 아바타 오버레이, 항상 위, 클릭 통과
- Drag to move, mouse wheel to resize, snap to screen edges and corners (hotkey to cycle corners) / 이동, 크기 조절, 모서리 붙이기
- Flip vertical / horizontal, fade on mouse hover / 상하·좌우 반전, 마우스를 올리면 반투명
- GPU rendering, low CPU usage, up to 90 fps / GPU 렌더링, 낮은 CPU 사용
- Optional "exit when VRChat closes" / VRChat 종료 시 함께 종료 옵션
- English / 한국어 / 日本語 UI

Keywords: VRChat, Spout, Spout2, overlay, avatar, Windows, DirectX 11.

## Notice

VRChat은 VRChat Inc.의 상표이며, 이 프로젝트는 VRChat과 관련 없는 비공식 도구입니다. (VRChat is a trademark of VRChat Inc. This is an unofficial tool, not affiliated with VRChat.)
이 프로그램은 VRChat이 공식 기능으로 내보내는 Spout 영상만 받아서 표시하고, VRChat 클라이언트를 수정하거나 접근하지 않습니다.

## 라이선스

- 이 프로젝트의 소스 코드: [MIT License](LICENSE)
- `third_party\Spout2`: BSD-2-Clause (Lynn Jarvis). 배포본에는 `THIRD_PARTY_LICENSES.txt`로 고지문을 포함합니다.

배포 파일은 코드 서명이 되어 있지 않아서 Windows SmartScreen 경고가 뜰 수 있습니다.

## 폴더 구성

| 경로 | 내용 |
|---|---|
| `AvatarOverlay\` | **배포 폴더**: `AvatarOverlay.exe` + `README.txt` + `THIRD_PARTY_LICENSES.txt` (빌드하면 exe가 여기에 생성됨) |
| `releases\` | 배포용 zip (`AvatarOverlay-<버전>.zip`) |
| `src\` | 소스 (`main.cpp`) 와 리소스 (`resources\` 아이콘, 버전 정보) |
| `tools\` | 보조 스크립트 (패키징, 아이콘 생성, 부하 측정용 가짜 Spout Sender) |
| `third_party\` | Spout2 소스 (BSD-2) |
| `VERSION` | 버전 번호 (여기만 고치면 exe 정보, zip 이름에 반영됨) |
| `build\` | 빌드 중간 산출물 (git 제외) |

## 빌드와 배포

```
build.bat                                   # AvatarOverlay\AvatarOverlay.exe 생성 (Visual Studio 2022 C++ 필요)
powershell -File tools\package.ps1          # releases\AvatarOverlay-<버전>.zip 생성
```

실행 중인 exe는 덮어쓸 수 없으므로 빌드 전에 트레이 메뉴에서 종료한다.
