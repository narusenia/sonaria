# 要件定義書: Sonaria

## プロジェクト概要

Sonaria はボーカルのピッチ補正プラグイン（VST3 / AU / ARA2）とスタンドアロンアプリである。
PitchNet（HachiTune の派生）をハードフォークし、次の 3 点で差をつける。

- **作業速度**: VariAudio と Vovious の良いところを取り込み、ざっと補正 → ずれ探し → 修正 → 試聴の各段階を速くする
- **ハモリ**: 主旋律を複製して度数指定で一括シフトし、他トラックをゴースト表示しながら揃える
- **ハイブリッドエンジン**: ニューラルボコーダー・PSOLA・スペクトル包絡エンジンをノート単位で選び、のちに自動で振り分ける

個人の OSS として AGPL-3.0 で公開する（ADR-0001）。設計判断の経緯は `docs/adr/` に記録する。

## スコープ

### 対象範囲（v1）

- 既存機能の維持（解析・ピアノロール・ノート編集・ARA / 非 ARA・書き出し）
- 作業速度の改善: Quantize / Straighten、ずれの色分けとジャンプ、キーボード操作、スケール自動適用
- ハモリ: 度数シフト、他トラックのゴースト表示、主旋律へのタイミング揃え
- エンジン: ノート単位の選択、スペクトル包絡エンジン、自動ルーティング
- スマート分割（ピッチ変化での分割ツール）
- 対応 OS: macOS（Apple Silicon・主対象）、Windows（CI でビルドが通る）

### 対象外

- 複数エンジン出力の同時ブレンド（ADR-0003）
- Auto-Tune 型のリアルタイム補正モード（ノート単位の Retune Speed 相当で代替）
- WebView による UI（ADR-0005）
- 本家 PitchNet への自動追従（ADR-0001）
- 包絡推定用ニューラルモデルの自前学習（M4 の結果次第で再検討）
- Logic Pro での ARA 動作保証（ADR-0007）

## 要件一覧

| ID | タイトル | 優先度 | ステータス |
|----|---------|--------|-----------|
| REQ-INFRA-001 | PitchNet のハードフォークと名前変更 | Must | Draft |
| REQ-INFRA-002 | macOS 優先のビルドと CI | Must | Draft |
| REQ-INFRA-003 | ライセンス・帰属表示・モデルのライセンス確認 | Must | Draft |
| REQ-INFRA-004 | 検証ホスト | Must | Draft |
| REQ-WF-001 | スケールの自動適用 | Must | Draft |
| REQ-WF-002 | Quantize / Straighten | Must | Draft |
| REQ-WF-003 | ずれの色分け表示 | Must | Draft |
| REQ-WF-004 | ずれたノートへのジャンプ | Must | Draft |
| REQ-WF-005 | キーボードだけの編集 | Must | Draft |
| REQ-WF-006 | ノート単位の A/B 試聴 | Should | Draft |
| REQ-WF-007 | 補正プリセット | Should | Draft |
| REQ-WF-008 | スマート分割 | Should | Draft |
| REQ-HARM-001 | 度数シフト | Must | Draft |
| REQ-HARM-002 | 他トラックのゴースト表示 | Must | Draft |
| REQ-HARM-003 | 主旋律へのタイミング揃え | Should | Draft |
| REQ-ENG-001 | ノート単位のエンジン選択 | Must | Draft |
| REQ-ENG-002 | スペクトル包絡エンジン | Should | Draft |
| REQ-ENG-003 | 包絡推定器の差し替え | Should | Draft |
| REQ-ENG-004 | 自動ルーティング | Should | Draft |
| REQ-UI-001 | JUCE UI の継続と LookAndFeel の刷新 | Must | Draft |
| REQ-UI-002 | 操作応答 | Must | Draft |

## 用語集

| 用語 | 定義 |
|------|------|
| ノート | GAME が分割した、ピッチ編集の単位 |
| ずれ | ノートの平均ピッチと、スケール上で最も近い音との差（セント） |
| Quantize | ノートの平均ピッチを、スケール上の音へ寄せる操作（強さ 0〜100%） |
| Straighten | ノート内のピッチの揺れを平らにする操作（強さ 0〜100%） |
| 度数シフト | キーとスケールに沿って、ノートを n 度上下へ移す操作 |
| ゴースト | 他トラックのノートを、編集できない半透明で重ねた表示 |
| エンジン | 編集後の音を作る合成処理。AI（ニューラルボコーダー）/ Classic（PSOLA）/ Spectral（STFT ＋ 包絡） |
| 包絡 | スペクトル包絡。フォルマントの形を表す |
| ルーティング | 区間ごとにどのエンジンで合成するかを決めること |
