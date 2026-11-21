Avatar Overlay 0.1.0
=====================

[English]
Avatar Overlay shows your VRChat avatar (Spout output) as a transparent, always-on-top overlay on your screen.

Requirements
  Windows 10/11 (64-bit), a DirectX 11 GPU, and VRChat with Spout output enabled.

Getting started
  1. Run AvatarOverlay.exe. A tray icon appears. The first run starts in edit mode (blue frame).
  2. Drag the window to move it. Use the mouse wheel or drag the bottom-right corner to resize.
  3. Press Ctrl+Alt+F9 to leave edit mode. Right-click the tray icon (or the window in edit mode) for all options.

Behavior
  - The overlay is always on top and does not take focus. Outside edit mode, mouse clicks pass through it.
  - The Spout sender is selected automatically. The overlay is drawn at up to 90 frames per second.
  - Position, size and all options below are saved automatically.

Menu / settings
  Edit mode (Ctrl+Alt+F9)     Move and resize the overlay. Shows a blue frame and a resize handle.
                              While dragging, the window snaps to screen edges (hold Shift to disable).
  Show avatar (Ctrl+Alt+F10)  Show or hide the overlay.
  Flip vertical               Flip the image upside down.
  Flip horizontal             Mirror the image left/right.
  Snap to corner              Move to a screen corner: top left / top right / bottom left / bottom right.
                              "Cycle corners" (Ctrl+Alt+F11) moves clockwise to the next corner.
  Transparency                How transparency is handled:
                                Standard           Normal. Use this first.
                                Effects / edge fix Use if edges look dark or glowing effects disappear.
                                Show alpha         Shows only the alpha channel in grayscale (for troubleshooting).
  Fade on mouse hover         Fades the overlay to 25% while the mouse is over it so it never gets in the way.
  Exit when VRChat closes     Quits Avatar Overlay automatically after VRChat has closed. It only triggers once
                              VRChat has been seen running, so starting the overlay before VRChat is fine.
                              Avatar Overlay does not start by itself: start it each time you play.
  Language                    Auto / English / Korean / Japanese.
  Exit                        Quit the application.

Hotkeys
  Ctrl+Alt+F9 edit mode / Ctrl+Alt+F10 show-hide / Ctrl+Alt+F11 cycle corners.
  If another program already uses a hotkey, it will not work; use the tray menu instead.

Settings file
  AvatarOverlay.ini next to the exe (or in %APPDATA%\Avatar Overlay if that folder is not writable).
  Delete it to reset everything.

Uninstall
  Exit from the tray menu, then delete the folder. (Nothing else is installed.)

Note
  The file is not code-signed, so Windows SmartScreen may show a warning ("More info" -> "Run anyway").

[한국어]
Avatar Overlay는 VRChat 아바타(Spout 출력)를 투명한 항상-위 오버레이로 화면에 표시합니다.

필요 사항
  Windows 10/11 (64비트), DirectX 11 GPU, VRChat에서 Spout 출력 켜기

시작하기
  1. AvatarOverlay.exe를 실행하면 트레이 아이콘이 생깁니다. 처음에는 편집 모드(파란 테두리)로 시작합니다.
  2. 창을 드래그해서 이동하고, 마우스 휠 또는 오른쪽 아래 모서리 드래그로 크기를 조절합니다.
  3. Ctrl+Alt+F9로 편집 모드를 끕니다. 트레이 아이콘(또는 편집 모드의 창)을 우클릭하면 모든 옵션이 있습니다.

동작
  - 오버레이는 항상 위에 표시되고 포커스를 가져가지 않습니다. 편집 모드가 아닐 때는 마우스 클릭이 통과합니다.
  - Spout Sender는 자동으로 선택됩니다. 최대 초당 90프레임으로 그립니다.
  - 위치, 크기, 아래 모든 옵션이 자동으로 저장됩니다.

메뉴 / 설정
  편집 모드 (Ctrl+Alt+F9)       오버레이를 이동하고 크기를 조절합니다. 파란 테두리와 크기 조절 손잡이가 표시됩니다.
                                드래그 중 화면 가장자리에 자동으로 붙습니다 (Shift를 누르면 해제).
  아바타 표시 (Ctrl+Alt+F10)    오버레이를 표시하거나 숨깁니다.
  상하 반전                     이미지를 위아래로 뒤집습니다.
  좌우 반전                     이미지를 좌우로 뒤집습니다.
  모서리에 붙이기               화면 모서리로 이동합니다: 왼쪽 위 / 오른쪽 위 / 왼쪽 아래 / 오른쪽 아래.
                                "모서리 순환"(Ctrl+Alt+F11)은 시계 방향으로 다음 모서리로 이동합니다.
  투명 처리                     투명도 처리 방식입니다.
                                  기본                  일반 상태입니다. 먼저 이것을 사용하세요.
                                  이펙트·가장자리 보정  가장자리가 어둡거나 빛나는 이펙트가 사라질 때 사용합니다.
                                  알파 확인             알파 채널만 흑백으로 보여줍니다 (문제 확인용).
  마우스를 올리면 반투명        마우스가 오버레이 위에 있는 동안 25%로 흐려져서 방해가 되지 않습니다.
  VRChat 종료 시 함께 종료      VRChat이 종료되면 Avatar Overlay도 자동으로 종료합니다. VRChat이 실행 중인 것을 확인한
                                뒤에만 동작하므로, VRChat보다 먼저 오버레이를 켜도 괜찮습니다.
                                Avatar Overlay는 스스로 시작되지 않으므로 플레이할 때마다 실행하세요.
  언어                          자동 / English / 한국어 / 日本語
  종료                          프로그램을 종료합니다.

단축키
  Ctrl+Alt+F9 편집 모드 / Ctrl+Alt+F10 표시·숨김 / Ctrl+Alt+F11 모서리 순환
  다른 프로그램이 같은 단축키를 쓰고 있으면 동작하지 않습니다. 이때는 트레이 메뉴를 이용하세요.

설정 파일
  exe 옆의 AvatarOverlay.ini (폴더에 쓸 수 없으면 %APPDATA%\Avatar Overlay)에 저장됩니다.
  삭제하면 모든 설정이 초기화됩니다.

삭제
  트레이 메뉴에서 종료한 뒤 폴더를 삭제하세요. (다른 곳에 설치되는 것은 없습니다.)

참고
  코드 서명이 없어서 Windows SmartScreen 경고가 뜰 수 있습니다 ("추가 정보" → "실행").

[日本語]
Avatar Overlayは、VRChatのアバター(Spout出力)を透過の最前面オーバーレイとして画面に表示します。

必要環境
  Windows 10/11 (64ビット)、DirectX 11対応GPU、VRChatのSpout出力をオン

はじめに
  1. AvatarOverlay.exe を起動するとタスクトレイにアイコンが表示されます。初回は編集モード(青い枠)で始まります。
  2. ウィンドウをドラッグして移動し、マウスホイールまたは右下の角のドラッグでサイズを変更します。
  3. Ctrl+Alt+F9 で編集モードを終了します。トレイアイコン(または編集モード中のウィンドウ)を右クリックで全オプション。

動作
  - オーバーレイは常に最前面に表示され、フォーカスを奪いません。編集モード以外ではクリックが通過します。
  - Spout送信元は自動で選択されます。最大で毎秒90フレームで描画します。
  - 位置、サイズ、以下の全オプションは自動保存されます。

メニュー / 設定
  編集モード (Ctrl+Alt+F9)      オーバーレイを移動・サイズ変更します。青い枠とサイズ変更ハンドルが表示されます。
                                ドラッグ中は画面の端に吸着します (Shiftキーを押すと無効)。
  アバターを表示 (Ctrl+Alt+F10) オーバーレイの表示/非表示を切り替えます。
  上下反転                      画像を上下に反転します。
  左右反転                      画像を左右に反転します。
  角に吸着                      画面の角に移動します: 左上 / 右上 / 左下 / 右下。
                                「角を順に移動」(Ctrl+Alt+F11)は時計回りに次の角へ移動します。
  透過処理                      透明度の処理方法です。
                                  標準                  通常の状態です。まずこれを使ってください。
                                  エフェクト・縁の補正  縁が暗い、または発光エフェクトが消えるときに使います。
                                  アルファ表示          アルファチャンネルだけをグレースケールで表示します (確認用)。
  マウスを乗せると半透明        マウスがオーバーレイ上にある間、25%まで薄くなり邪魔になりません。
  VRChat終了時に一緒に終了      VRChatが終了するとAvatar Overlayも自動的に終了します。VRChatが実行中なのを確認した後にのみ
                                動作するため、VRChatより先にオーバーレイを起動しても問題ありません。
                                Avatar Overlayは自動では起動しないため、プレイのたびに起動してください。
  言語                          自動 / English / 한국어 / 日本語
  終了                          アプリを終了します。

ショートカット
  Ctrl+Alt+F9 編集モード / Ctrl+Alt+F10 表示・非表示 / Ctrl+Alt+F11 角を順に移動
  他のアプリが同じキーを使っている場合は動作しません。その場合はトレイメニューを使ってください。

設定ファイル
  exe と同じフォルダの AvatarOverlay.ini (書き込めない場合は %APPDATA%\Avatar Overlay)に保存されます。
  削除するとすべての設定が初期化されます。

アンインストール
  トレイメニューから終了し、フォルダを削除してください。(他の場所にインストールされるものはありません。)

注意
  コード署名がないため、Windows SmartScreenの警告が表示される場合があります(「詳細情報」→「実行」)。
