# ロードマップ

順序は ADR-0009。状態は各マイルストーンの `README.md` だけで管理する。

| M | 内容 | 完了条件 |
|---|---|---|
| [M0](M0-foundation/README.md) | フォーク・名前変更・ビルド・ライセンス確認 | Cubase と Studio One で、ARA として PitchNet と同じように動く |
| [M1](M1-workflow/README.md) | スケール自動適用・Quantize / Straighten・ずれの色分けとジャンプ・キーボード操作 | 1 曲のざっと補正が PitchNet より明らかに速い |
| [M2](M2-harmony/README.md) | 度数シフト・他トラックのゴースト表示 | 主旋律を複製して、3 度のハモリが数分で作れる |
| [M3](M3-engine-select/README.md) | ノート単位のエンジン選択（AI / Classic） | ノートごとにエンジンを切り替えて聴き比べられる |
| [M4](M4-spectral-engine/README.md) | Spectral エンジン（STFT ＋ 古典的な包絡推定） | PSOLA より自然な場面が確認できる。なければ打ち切りを ADR に記録する |
| [M5](M5-advanced/README.md) | 自動ルーティング・タイミング揃え・スマート分割・A/B 試聴・プリセット | 各機能の受入条件を満たす |
