# M0: フォークと基盤

完了条件: Cubase と Studio One で、ARA として PitchNet と同じように動く。

計画ファイルは着手時にこのディレクトリへ作る（1 本 = 1 ファイル、冒頭に REQ-ID・状態・依存）。

| ID | 単位 | REQ | 規模 | 依存 | 状態 |
|---|---|---|---|---|---|
| M0-01 | PitchNet の取り込み（submodule 含む）、取り込み元コミットの記録 | INFRA-001 | S | — | 完了（`dc2c1b3` を全履歴で取り込み、本家は `upstream` リモート） |
| M0-02 | 名前変更: 製品名・プラグインコード・バンドル ID・ウィンドウタイトル・言語ファイル | INFRA-001 | M | M0-01 | 完了・ビルド未確認（外から見える部分のみ。会社名 Narusenia、`one.nxeu.sonaria`、`Nrsn` / `Snr1`（Dev は `Snd1`）、保存は `.sonaria`・`.pitchnet` も開ける、起動時の更新確認を停止。クラス名・CMake 変数・ターゲット名は PitchNet のまま） |
| M0-03 | README（英日）・LICENSE・THIRD_PARTY_NOTICES の更新（帰属表示を残す） | INFRA-003 | S | M0-01 | 完了（README を英日で書き直し（`README.ja.md` を追加）、THIRD_PARTY_NOTICES の冒頭を Sonaria に。LICENSE は AGPL 原文のため変更なし） |
| M0-04 | 同梱 ONNX モデルのライセンス確認、`docs/reference/models.md` | INFRA-003 | S | — | 完了（`docs/reference/models.md`。PC-NSF-HiFiGAN と GAME の重みは CC BY-NC-SA 4.0、RMVPE はライセンスなしで再配布不可。NOTICES を訂正。対応方針は models.md の「決めること」） |
| M0-05 | macOS arm64 で Standalone / VST3 / AU / ARA をビルド | INFRA-002 | M | M0-02 | 完了（macOS arm64、Xcode 26.3、cmake 3.31 でビルド成功。Standalone・VST3・AU、ARA 有効。Standalone は起動確認のみで、ホストでの動作は M0-07。`mise run build` で実行。macOS は `pc_nsf_hifigan.mlmodelc` が必須で、PitchNet の `models` リリースから取得した。`rmvpe.onnx` はなくてもビルドできる） |
| M0-06 | GitHub Actions（macOS / Windows のビルド） | INFRA-002 | M | M0-05 | 完了（run 43685306945 で macOS 約 20 分・Windows 約 10 分とも成功。macOS と Windows (CPU) の 2 ジョブに絞った。Linux・GPU・DirectML・リリースは削除。`mlmodelc` は PitchNet のリリースから SHA-256 付きで取得し、`rmvpe.onnx` は取得しない） |
| M0-07 | ホスト確認手順 `docs/dev/host-check.md`、Cubase と Studio One で確認 | INFRA-004 | S | M0-05 | 手順書作成済み・ホスト確認は未実施（`docs/dev/host-check.md`。Dev ビルドで確認し、結果は同ファイルの表に記録する） |
| M0-08 | インストーラ（`packaging/`）の名前変更。リソースの置き場所を `Narusenia/Sonaria` に合わせる | INFRA-001 | S | M0-02 | 未着手 |
