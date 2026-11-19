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

## 結果（2026-11-19）

- 直書きの色 249 か所を `APP_COLOR_*` に置き換え、定数を 138 個足した。フォント名の直書き 3 か所は `AppFont` を通した（`getRegularFont` を追加）。余白は変更なし
- 色の値が変わっていないことを差分から機械的に確認し、空のプロジェクトの画面が作業前とピクセル単位で一致することを確認した
- 残した直書き: デバッグ表示（`PianoRollComponent.cpp` の GAME オーバーレイ、`OverviewPanel.cpp` の `showSegmentsDebug`、`PitchCurveRenderer.cpp` の F0 デバッグ曲線）、描画の計算に使う白黒（`MainComponent.cpp` の明るさ補正）、DAW から来るトラック色、SVG の置換元の黒、`noteRenderer` がないときの白、使われていない `FourierFilterDialog`、フォント名を指定しない既定フォント 4 か所

## M1-11 への引き継ぎ

- 配色が 2 系統ある。`DarkLookAndFeel` の既定（コンボボックス・入力欄・リスト・アラート・メニュー）は青みのある `SURFACE` / `BORDER` 系、独自の部品の多くはニュートラルな灰色。M1-11 で 1 系統にまとめる
- 古い青系の `PIANO_WHITE` / `PIANO_BLACK` / `PIANO_TEXT*` / `TIMELINE` / `NOTE_NORMAL` は PolyLab だけが使っている。`SELECTION_OVERLAY` は未使用
- 近い灰色が多数ある（`#171717`・`#191818`・`#191717`・`#151515` など）。役割ごとに別の定数にしてあるので、M1-11 で値を寄せる
- 差し色は実際にはオレンジ（`TOOLBAR_PROGRESS` = `#FF5600` など）で、`APP_COLOR_PRIMARY` の紫は画面にほぼ出ていない
- `CustomTitleBar.cpp` の `TitleBarColors` は、ファイルの外側で定数を値コピーしている（upstream 由来）。初期化順序によっては未初期化の値を拾う恐れがある
- `TimingHandler.cpp` の境界の色は、ハイライト時も選択時も白で、選択の分岐が効いていない
