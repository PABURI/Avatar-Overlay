# Avatar Overlay

VRChat 아바타(Spout 출력)를 투명한 항상-위 오버레이로 화면에 띄우는 Windows 프로그램.
사용자용 설명은 `AvatarOverlay\README.txt` (영어 / 한국어 / 日本語).

VRChat은 VRChat Inc.의 상표이며, 이 프로젝트는 VRChat과 관련 없는 비공식 도구입니다.
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
