# M4: Spectral エンジン

完了条件: 評価セットで、PSOLA より自然な場面が確認できる。確認できなければ、Spectral を既定の選択肢から外す判断を ADR に記録して終える（ADR-0004）。

仕様: `docs/specifications/engine.md`（Spectral エンジン）

| ID | 単位 | REQ | 規模 | 依存 | 状態 |
|---|---|---|---|---|---|
| M4-01 | 評価方法と評価セットを決める（ABX か指標か、音源のライセンス） | ENG-002 | S | — | 未着手 |
| M4-02 | 調査: 本家 PitchNet の git 履歴にあるフェーズボコーダー実装が使えるか | ENG-002 | S | — | 未着手 |
| M4-03 | `EnvelopeEstimator` と `TrueEnvelopeEstimator`、推定器の比較テスト | ENG-003 | M | — | 未着手 |
| M4-04 | `SpectralSynthesizer`（位相ロック・比率 1.0 で原音一致・`sourceFrame` 対応） | ENG-002 | L | M4-02, M4-03 | 未着手 |
| M4-05 | エンジン選択肢への追加（M3 の仕組みに乗せる） | ENG-001, ENG-002 | S | M4-04 | 未着手 |
| M4-06 | 評価と判定、結果を ADR に記録 | ENG-002 | M | M4-01, M4-05 | 未着手 |
