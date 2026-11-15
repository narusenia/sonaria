# アーキテクチャ

関連: REQ-INFRA-001, REQ-UI-001, REQ-ENG-001

## 概要

PitchNet の構成をそのまま引き継ぐ。JUCE（C++17）の単一コードベースから Standalone / VST3 / AU / ARA を作り、推論は ONNX Runtime（macOS は CoreML）で行う。
Sonaria の追加は、既存の層に新しいファイルを足す形で入れ、層の分け方は変えない。

```
Plugin/  (PluginProcessor, ARADocumentController, NonAraCaptureController)
   │
UI/      (MainComponent, PianoRoll/*, ToolbarComponent, ParameterPanel)   ← JUCE 依存
   │
Audio/   (EditorController, AudioEngine, Synthesis/*, 検出器)
   │
Models/  (Project, Note, ProjectSerializer)   Undo/   Utils/ (ScaleUtils, PitchToolOperations …)
```

## 解析と合成の流れ（既存）

1. 読み込み → リサンプル（r8brain）
2. ピッチ検出（FCPE 既定 / RMVPE）→ ノート分割（GAME）→ ブレス・歯擦音検出
3. 編集はノートとピッチカーブ（delta）への操作として `Project` に保存し、`Undo/` のアクションで戻せる
4. `IncrementalSynthesizer` が編集された範囲だけを再合成する。無声フレームはブレンドマスクで原音を通す
5. 合成は `SynthesisEngineType`（Vocoder / Psola）をプロジェクト単位で切り替える

## Sonaria で足すもの

| 場所 | 追加 | M |
|---|---|---|
| `Utils/ScaleUtils.h` | 解析後の自動適用、度数シフト計算 | M1 / M2 |
| `Utils/PitchToolOperations` | Quantize / Straighten（既存の factor 操作を使う） | M1 |
| `UI/PianoRoll/NoteRenderer` | ずれの色分け、ゴースト描画 | M1 / M2 |
| `UI/Commands.h` | ジャンプ・キーボード操作・度数シフトのコマンド ID | M1 / M2 |
| `Plugin/ARADocumentController` | 他トラックのノート参照 | M2 |
| `Models/Note` | ノート単位のエンジン選択 | M3 |
| `Audio/Synthesis/IncrementalSynthesizer` | エンジンごとの区間分割と境界のクロスフェード | M3 |
| `Audio/Synthesis/SpectralSynthesizer`（新規） | STFT ＋ 包絡エンジン | M4 |
| `Audio/Synthesis/EnvelopeEstimator`（新規） | 包絡推定器の差し替え口 | M4 |
| `Audio/Synthesis/EngineRouter`（新規） | 自動ルーティング | M5 |
| `UI/PianoRoll/NoteSplitter` | スマート分割 | M5 |

## 規則

- エンジンは `DspSynthesisRequest` を受け取り、同じ範囲のサンプルを返す（`engine.md`）。ニューラルボコーダーだけは mel 入力のため既存の経路を使う
- 新しい編集操作はすべて `Undo/` のアクションとして実装し、1 操作 = undo 1 回にする
- `Models/` は UI に依存しない
- プロジェクト形式を変えるときは、`ProjectSerializer` で旧形式を読めるようにする
