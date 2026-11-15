# 同梱・参照する ONNX モデルのライセンス

REQ-INFRA-003 / M0-04。調査日 2026-11-15。出典は各リポジトリのライセンス表示とリリースノートで、モデルの出所を直接確認したものではない（取り込んだ PitchNet に出典の記録がなかったため）。

## 一覧

| モデル | ファイル | 用途 | リポジトリ | 重みのライセンス | 配布 |
|---|---|---|---|---|---|
| PC-NSF-HiFiGAN | `pc_nsf_hifigan.onnx` | ボコーダー | [openvpi/vocoders](https://github.com/openvpi/vocoders)（コード AGPL-3.0） | **CC BY-NC-SA 4.0**（非商用） | 非商用に限り可 |
| GAME | `GAME/*.onnx` | ノート分割 | [openvpi/GAME](https://github.com/openvpi/GAME)（コード MIT） | **CC BY-NC-SA 4.0**（v1.0.0 リリースノート） | 非商用に限り可 |
| FCPE | `fcpe.onnx` | ピッチ検出 | [CNChTu/FCPE](https://github.com/CNChTu/FCPE)（MIT） | 重み単体の記述なし。リポジトリの MIT に従うと解釈 | 可（要確認） |
| RMVPE | `rmvpe.onnx` | ピッチ検出 | [yxlllc/RMVPE](https://github.com/yxlllc/RMVPE) | **ライセンスなし**（LICENSE も README の記述もない） | 不可 |
| SwiftF0 | `swift_f0.onnx` | ピッチ検出（PitchNet の既定） | [lars76/swift-f0](https://github.com/lars76/swift-f0)（MIT） | MIT | 可 |
| Basic Pitch | `basic_pitch_nmp.onnx` | ポリフォニックの音高証拠 | [spotify/basic-pitch](https://github.com/spotify/basic-pitch)（Apache-2.0） | Apache-2.0 | 可（NOTICE の表示が必要） |

## 分かったこと

- **`THIRD_PARTY_NOTICES.txt` の PC-NSF-HiFiGAN の項は不正確。** 「AGPL-3.0」と書いてあるのはリポジトリのコードのライセンスで、学習済み重みは CC BY-NC-SA 4.0 である。GAME も同じ。
- **非商用の重みは AGPL のコードと別のライセンスのまま配布する。** Sonaria は個人 OSS で商用配布をしない（ADR-0001）ので、非商用であることを明記すれば同梱できる。ただし重みを使う限り、Sonaria を商用製品としては出せない。
- **RMVPE は再配布できない。** `rmvpe.onnx` は PitchNet の時点でリポジトリに入っておらず、README も取得手順を書いていない。
- **CMake は GAME を設定時に GitHub のリリースからダウンロードする。** リポジトリには含まれないが、インストーラは同梱する。
- **`basic_pitch_nmp.onnx` と `GAME/` は `Resources/models` にない。** `PolyLab.cpp` が `basic_pitch_nmp.onnx` を読むため、ポリフォニック編集はこのファイルがないと動かない。入手元は未記録。

## 決めること

1. 商用配布をしない前提を固定するか。固定するなら ADR に書き、README と NOTICES に「モデルは非商用」と明記する。
2. RMVPE を外すか、作者に許諾を問い合わせるか。外すなら FCPE と SwiftF0 で足りるかを確認する（`EditorController.cpp` に RMVPE の選択肢がある）。
3. `basic_pitch_nmp.onnx` の入手元と、変換した方法を記録する。
