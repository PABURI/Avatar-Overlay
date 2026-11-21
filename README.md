# Avatar Overlay

A transparent, always-on-top overlay that shows your VRChat avatar (Spout output) on your Windows screen.

VRChat 아바타(Spout 출력)를 투명한 항상-위 오버레이로 화면에 띄우는 Windows 프로그램입니다.

VRChatのアバター(Spout出力)を透過の最前面オーバーレイとして画面に表示するWindowsアプリです。

## Download

1. Get the latest zip from **[Releases](https://github.com/PABURI/Avatar-Overlay/releases)**.
2. Unzip it and run `AvatarOverlay.exe`.
3. Read the usage guide: [`AvatarOverlay/README.txt`](AvatarOverlay/README.txt) (English / 한국어 / 日本語).

> The file is not code-signed, so Windows SmartScreen may show a warning ("More info" > "Run anyway").

**Requirements:** Windows 10/11 (64-bit), a DirectX 11 GPU, and VRChat with Spout output enabled.

## Features

- Transparent avatar overlay from VRChat's Spout output, always on top, clicks pass through
- Drag to move, mouse wheel to resize, snap to screen edges and corners (hotkey to cycle corners)
- Flip vertical / horizontal, fade on mouse hover
- GPU rendering with low CPU usage, up to 90 fps
- Optional "exit when VRChat closes"
- English / 한국어 / 日本語 UI

<details>
<summary><b>한국어</b></summary>

### 다운로드

1. **[Releases](https://github.com/PABURI/Avatar-Overlay/releases)**에서 최신 zip을 받습니다.
2. 압축을 풀고 `AvatarOverlay.exe`를 실행합니다.
3. 사용법은 [`AvatarOverlay/README.txt`](AvatarOverlay/README.txt)를 참고하세요. (English / 한국어 / 日本語)

> 코드 서명이 없어서 Windows SmartScreen 경고가 뜰 수 있습니다 ("추가 정보" → "실행").

**필요 사항:** Windows 10/11 (64비트), DirectX 11 GPU, VRChat에서 Spout 출력 켜기

### 기능

- VRChat Spout 출력을 투명한 아바타 오버레이로 표시, 항상 위, 클릭 통과
- 드래그로 이동, 마우스 휠로 크기 조절, 화면 가장자리와 모서리에 붙이기 (단축키로 모서리 순환)
- 상하·좌우 반전, 마우스를 올리면 반투명
- GPU 렌더링, 낮은 CPU 사용, 최대 90fps
- 선택 옵션: VRChat 종료 시 함께 종료
- English / 한국어 / 日本語 UI

</details>

<details>
<summary><b>日本語</b></summary>

### ダウンロード

1. **[Releases](https://github.com/PABURI/Avatar-Overlay/releases)** から最新のzipをダウンロードします。
2. 解凍して `AvatarOverlay.exe` を実行します。
3. 使い方は [`AvatarOverlay/README.txt`](AvatarOverlay/README.txt) を参照してください。(English / 한국어 / 日本語)

> コード署名がないため、Windows SmartScreenの警告が表示される場合があります(「詳細情報」→「実行」)。

**必要環境:** Windows 10/11 (64ビット)、DirectX 11対応GPU、VRChatのSpout出力をオン

### 機能

- VRChatのSpout出力を透過のアバターオーバーレイとして表示、常に最前面、クリックは通過
- ドラッグで移動、マウスホイールでサイズ変更、画面の端や角に吸着(ショートカットで角を順に移動)
- 上下・左右反転、マウスを乗せると半透明
- GPU描画で低CPU使用率、最大90fps
- オプション: VRChat終了時に一緒に終了
- English / 한국어 / 日本語 UI

</details>

## Notice

VRChat is a trademark of VRChat Inc. This is an unofficial tool and is not affiliated with VRChat.
It only receives the Spout video that VRChat officially outputs, and does not modify or access the VRChat client.

<details>
<summary>한국어 / 日本語</summary>

- VRChat은 VRChat Inc.의 상표이며, 이 프로젝트는 VRChat과 관련 없는 비공식 도구입니다. VRChat이 공식 기능으로 내보내는 Spout 영상만 받아서 표시하고, VRChat 클라이언트를 수정하거나 접근하지 않습니다.
- VRChatはVRChat Inc.の商標であり、本プロジェクトはVRChatとは無関係の非公式ツールです。VRChatが公式機能として出力するSpout映像のみを受け取って表示し、VRChatクライアントの変更やアクセスは行いません。

</details>

## License

- Source code of this project: [MIT License](LICENSE)
- `third_party/Spout2`: BSD-2-Clause (Lynn Jarvis). The release zip includes its notice as `THIRD_PARTY_LICENSES.txt`.

---

## Development (개발자용)

### 폴더 구성

| 경로 | 내용 |
|---|---|
| `AvatarOverlay/` | **배포 폴더**: `AvatarOverlay.exe` + `README.txt` + `THIRD_PARTY_LICENSES.txt` (빌드하면 exe가 여기에 생성됨) |
| `releases/` | 배포용 zip (`AvatarOverlay-<버전>.zip`) |
| `src/` | 소스 (`main.cpp`)와 리소스 (`resources/` 아이콘, 버전 정보) |
| `tools/` | 보조 스크립트 (패키징, 아이콘 생성, 부하 측정용 가짜 Spout Sender) |
| `third_party/` | Spout2 소스 (BSD-2) |
| `VERSION` | 버전 번호 (여기만 고치면 exe 정보, zip 이름에 반영됨) |
| `build/` | 빌드 중간 산출물 (git 제외) |

### 빌드와 배포

```
build.bat                                   # AvatarOverlay\AvatarOverlay.exe 생성 (Visual Studio 2022 C++ 필요)
powershell -File tools\package.ps1          # releases\AvatarOverlay-<버전>.zip 생성
```

실행 중인 exe는 덮어쓸 수 없으므로 빌드 전에 트레이 메뉴에서 종료합니다.
