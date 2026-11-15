# ホスト確認手順

REQ-INFRA-004 / ADR-0007。各マイルストーンの完了時に、Cubase と Studio One（Fender Studio Pro）で ARA の読み込み・編集・保存・再読み込みを確認する。Logic Pro は AU がビルドできることだけを見る。

## 準備

Dev ビルドを使う。製品名 `Sonaria Dev`、バンドル ID `one.nxeu.sonaria.dev`、プラグインコード `Snd1` が別になるため、リリース版で保存したプロジェクトと混ざらない。

```bash
mise exec -- cmake -B build-dev -G Ninja -DCMAKE_BUILD_TYPE=Release -DPITCHNET_DEV_BUILD=ON
mise exec -- cmake --build build-dev --config Release
cp -R "build-dev/PitchNetPlugin_artefacts/Release/VST3/Sonaria Dev.vst3" ~/Library/Audio/Plug-Ins/VST3/
cp -R "build-dev/PitchNetPlugin_artefacts/Release/AU/Sonaria Dev.component" ~/Library/Audio/Plug-Ins/Components/

# リリースビルドはバンドル内の Contents/Resources/models からモデルを読む。ビルドはコピーしない
V="$HOME/Library/Audio/Plug-Ins/VST3/Sonaria Dev.vst3"
mkdir -p "$V/Contents/Resources/models"
cp -R Resources/models/. "$V/Contents/Resources/models/"
codesign --force --deep -s - "$V"
```

モデルを入れ忘れると「モデルファイルがありません」というダイアログが出る。AU は Logic のビルド確認だけなので、モデルは入れない。

`mlmodelc` が `Resources/models` にない場合は、先に `docs/implementation/M0-foundation/README.md` の M0-05 を見る。ビルドはプラグインを自動でインストールしない。CI の成果物（ZIP）を使うときは、展開後に `xattr -dr com.apple.quarantine <path>` で隔離属性を外す。

プラグインを入れたら、各ホストのプラグイン管理画面で再スキャンする。

## 基本確認（ホストごとに行う）

用意するもの: 歌声のオーディオクリップ 1 本（30 秒〜1 分）。

1. **読み込み**: ホストが `Sonaria Dev` を VST3 として認識する。ARA 拡張として割り当てられる。
2. **解析**: クリップを割り当てると解析が始まり、ノートがピアノロールに出る。
3. **編集**: ノートのピッチを動かし、タイミングを変え、ピッチカーブを描く。合成が終わるまで待つ。
4. **再生**: ホストのトランスポートで再生し、編集後の音になっている。元の音に戻る場合は失敗。
5. **保存**: プロジェクトを保存して、ホストを終了する。
6. **再読み込み**: プロジェクトを開き直す。エディタを開かずに再生して編集後の音が出る。エディタを開くとノートとカーブが残っていて、解析が再実行されない。
7. **編集後の保存**: 1 つ編集して保存し、手順 6 を繰り返す。

あわせて、ホストのコンソールやクラッシュレポートに異常がないことを確認する。

## 追加確認

機能を触ったマイルストーンでは、該当する確認を足す。

| 内容 | 手順書 |
|---|---|
| リージョンの分割・結合（ARA） | [ara-region-splits.md](../testing/ara-region-splits.md) |
| Cubase のトラックバージョン | [cubase-track-versions.md](../testing/cubase-track-versions.md) |
| 状態の保存（非 ARA 含む） | [plugin-state-archive.md](../testing/plugin-state-archive.md) |

ARA を使わない読み込みは、Cubase・Studio One ともに任意のトラックへの挿入として確認する（ARA の確認とは別に 1 回）。

## Logic Pro（ビルド確認のみ）

`Sonaria Dev.component` を `~/Library/Audio/Plug-Ins/Components/` に置き、`auval -v aufx Snd1 Nrsn` が通るか、Logic のプラグインマネージャが認識するかを見る。ARA の動作は見ない。

## 確認結果の記録

確認したら次の表に追記する。マイルストーンの完了条件にこの表の最新行を使う。

| 日付 | コミット | ホスト（バージョン） | 結果 | メモ |
|---|---|---|---|---|
