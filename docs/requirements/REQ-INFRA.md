# 基盤要件定義

## REQ-INFRA-001: PitchNet のハードフォークと名前変更

| 項目 | 内容 |
|------|------|
| ID | REQ-INFRA-001 |
| 優先度 | Must |
| ステータス | Draft |

### 説明

PitchNet をハードフォークし、製品名・プラグインコード・バンドル ID を Sonaria 固有のものにする。
本家の更新は必要なものだけ手で取り込む（ADR-0001・0002）。

### 受入条件

- [ ] DAW のプラグイン一覧に PitchNet と別製品として表示され、PitchNet と同時に読み込める
- [ ] 取り込み元の PitchNet のコミットが README に記録されている

### 依存要件

- REQ-INFRA-003

## REQ-INFRA-002: macOS 優先のビルドと CI

| 項目 | 内容 |
|------|------|
| ID | REQ-INFRA-002 |
| 優先度 | Must |
| ステータス | Draft |

### 説明

Apple Silicon の macOS を主対象とし、CoreML で推論する。Windows はビルドが通る状態を保つ。

### 受入条件

- [ ] macOS（arm64）で Standalone / VST3 / AU / ARA がビルドできる
- [ ] GitHub Actions で macOS と Windows のビルドが通る
- [ ] ビルド手順が README に書かれている

### 依存要件

- REQ-INFRA-001

## REQ-INFRA-003: ライセンス・帰属表示・モデルのライセンス確認

| 項目 | 内容 |
|------|------|
| ID | REQ-INFRA-003 |
| 優先度 | Must |
| ステータス | Draft |

### 説明

AGPL-3.0 で公開し、PitchNet と HachiTune の帰属表示を残す。
同梱する ONNX モデル（RMVPE・FCPE・GAME・PC-NSF-HiFiGAN）それぞれのライセンスを確認し、配布可否を記録する。

### 受入条件

- [x] `LICENSE` に AGPL-3.0 の原文があり、`THIRD_PARTY_NOTICES.txt` と README に Sonaria・PitchNet・HachiTune の著作権と帰属が書かれている
- [ ] 各モデルのライセンスと出典が `docs/reference/models.md` に記録されている
- [ ] 配布できないモデルがある場合、取得手順か代替が決まっている

### 依存要件

- なし

## REQ-INFRA-004: 検証ホスト

| 項目 | 内容 |
|------|------|
| ID | REQ-INFRA-004 |
| 優先度 | Must |
| ステータス | Draft |

### 説明

ARA の検証は Cubase と Studio One（Fender Studio Pro）で行う。Logic Pro は AU のビルド確認のみとする（ADR-0007）。

### 受入条件

- [ ] 各マイルストーンの完了時に、Cubase と Studio One で ARA の読み込み・編集・保存・再読み込みを確認している
- [ ] 確認手順が `docs/dev/host-check.md` にある

### 依存要件

- REQ-INFRA-002
