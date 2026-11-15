# ADR-0007: ARA の検証は Cubase と Studio One で行う

- 状態: 採用
- 日付: 2026-11-15

## 背景

メンテナが普段使う DAW は Cubase と Studio One（Fender Studio Pro）である。使っている Logic Pro は、12 から Rosetta が使えなくなり ARA を使えないバージョンである。ARA の挙動、特に他トラックの参照は DAW ごとに違う。

## 決定

- ARA の検証は Cubase と Studio One で行う（VST3 ＋ ARA2）
- AU はビルドが通ることだけを確認する
- macOS（Apple Silicon）を主対象とし、Windows は CI でビルドが通る状態を保つ

## 結果

- Logic Pro での ARA 動作は保証しない
