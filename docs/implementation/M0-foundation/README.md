# M0: フォークと基盤

完了条件: Cubase と Studio One で、ARA として PitchNet と同じように動く。

計画ファイルは着手時にこのディレクトリへ作る（1 本 = 1 ファイル、冒頭に REQ-ID・状態・依存）。

| ID | 単位 | REQ | 規模 | 依存 | 状態 |
|---|---|---|---|---|---|
| M0-01 | PitchNet の取り込み（submodule 含む）、取り込み元コミットの記録 | INFRA-001 | S | — | 未着手 |
| M0-02 | 名前変更: 製品名・プラグインコード・バンドル ID・ウィンドウタイトル・言語ファイル | INFRA-001 | M | M0-01 | 未着手 |
| M0-03 | README（英日）・LICENSE・THIRD_PARTY_NOTICES の更新（帰属表示を残す） | INFRA-003 | S | M0-01 | 未着手 |
| M0-04 | 同梱 ONNX モデルのライセンス確認、`docs/reference/models.md` | INFRA-003 | S | — | 未着手 |
| M0-05 | macOS arm64 で Standalone / VST3 / AU / ARA をビルド | INFRA-002 | M | M0-02 | 未着手 |
| M0-06 | GitHub Actions（macOS / Windows のビルド） | INFRA-002 | M | M0-05 | 未着手 |
| M0-07 | ホスト確認手順 `docs/dev/host-check.md`、Cubase と Studio One で確認 | INFRA-004 | S | M0-05 | 未着手 |
