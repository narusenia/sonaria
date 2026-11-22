# M3: ノート単位のエンジン選択

完了条件: ノートごとにエンジン（AI / Classic）を切り替えて聴き比べられ、境界でノイズが出ない。

仕様: `docs/specifications/engine.md`（ノート単位の選択）

| ID | 単位 | REQ | 規模 | 依存 | 状態 |
|---|---|---|---|---|---|
| M3-01 | `Note` に `engine` を追加、`ProjectSerializer` と ARA の保存・旧形式の読み込み | ENG-001 | M | — | 実装済み（`NoteEngine` は Default / AI / Classic。Spectral は M4 で追加。テストは `NoteEngineTests`。プロジェクト単位の既定は未） |
| M3-02 | `IncrementalSynthesizer` でエンジンごとに区間を分けて合成 | ENG-001 | L | M3-01 | 未着手 |
| M3-03 | 境界の等パワークロスフェード（無声フレームではしない）とテスト | ENG-001 | M | M3-02 | 未着手 |
| M3-04 | UI: ノートのエンジン選択（パラメータパネル・右クリック・キー）と undo | ENG-001 | S | M3-01 | 未着手 |
