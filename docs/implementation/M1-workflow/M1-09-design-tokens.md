# M1-09: デザイン値の集約

- REQ: REQ-UI-001
- 状態: 計画表（`README.md`）を参照
- 依存: なし。M1-11（Sonaria のデザイン適用）がこれに依存する

## 目的

色・フォント・余白の値を 1 か所にまとめ、M1-11 で値を差し替えるだけで見た目を変えられるようにする。

## 現状（2026-11-19 調査）

- 置き場所はすでにある: 色は `Source/Utils/UI/Theme.h/.cpp`（`APP_COLOR_*`）、サイズは `UILayout.h`、フォントは `Source/UI/Components/AppFont.h`
- それを通らない直書きの色が 308 か所（値は 91 種類）。多いのは `StyledWidgets.h`・`ParameterPanel.cpp`・`PianoRollComponent.cpp`・`MainComponent.cpp`・`NoteRenderer.cpp`
- `Theme.cpp` の定数は青みのある色だが、実際の画面の多くは直書きのニュートラルな灰色で塗られている
- フォントは `"Montserrat"` を名前で直接指定している箇所がある

## 方針

- **見た目は変えない。** 直書きの値と同じ値の定数に置き換える。近い色の統合や配色の変更は M1-11 で行う
- 定数は既存の `APP_COLOR_*` に足す。名前は役割で付ける（例: 入力欄の背景、補助の文字）。値で名前を付けない
- `Colours::transparentBlack` は透明を表すだけなので残す
- フォントは `AppFont` を通す。太さと大きさは今と同じにする
- 余白は `UILayout` にある値を使う。描画コードの中の細かな数値（`reduced(4)` など）は、繰り返し使われるものだけ定数にする

## 確認

- 作業前後で Standalone の画面を撮り、差分がないことを確かめる
- macOS のビルドと CI が通る
