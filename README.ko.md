# R PointScroll

[English](README.md)

Sony VAIO P 같은 포인팅 스틱 노트북을 위한 Haiku 입력 필터입니다. 가운데
버튼을 누른 채 스틱을 움직이면 커서를 움직이는 대신 문서를 상하 또는 좌우로
스크롤합니다.

## 동작

- 가운데 버튼을 누르고 이동: 휠 스크롤
- 가운데 버튼을 움직이지 않고 클릭: 원래의 가운데 클릭 전달
- 작은 흔들림: 설정한 데드존까지 무시
- 빠른 이동: 선택적으로 최대 2.5배 가속
- 설정 변경: input_server를 재시작하지 않고 1초 안에 반영

필터는 장치 이름을 추측하지 않습니다. Haiku의 마우스 이벤트에는 장치 이름이
포함되지 않으므로, 활성화하면 포인팅 스틱뿐 아니라 연결된 일반 마우스에서도
`가운데 버튼 + 이동`이 같은 방식으로 동작합니다.

## 빌드와 설치

Haiku에서 실행합니다. VAIO P의 x86_gcc2 이미지에서는 기본 `g++`를 사용해야
32비트 input_server가 필터를 불러올 수 있습니다.

```sh
chmod +x install.sh
./install.sh
```

설치 후 Deskbar의 **Preferences -> R PointScroll**에서 활성화, 가로 스크롤,
방향, 속도, 가속, 데드존을 바꿀 수 있습니다.

```sh
./install.sh --build-only
./install.sh --uninstall
```

필터는 다음 위치에 설치됩니다.

```text
~/config/non-packaged/add-ons/input_server/filters/RPointScrollFilter
```

## 구현 메모

입력이 데드존을 넘기 전에는 가운데 버튼 누름을 잠시 보관합니다. 이동 없이
버튼을 놓으면 저장한 누름과 놓음을 순서대로 다시 전달하므로 가운데 클릭 기능이
사라지지 않습니다. 데드존을 넘으면 저장한 클릭을 버리고 이후 상대 이동량
`x/y`를 `B_MOUSE_WHEEL_CHANGED` 메시지로 변환합니다.

관성 스크롤은 의도적으로 넣지 않았습니다. 버튼을 놓은 뒤에도 input_server가
합성 입력을 계속 보내게 만들면 대상 창이 바뀌었을 때 엉뚱한 창이 움직일 수
있기 때문입니다.
