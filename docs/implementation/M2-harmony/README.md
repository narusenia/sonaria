# M2: ハモリ

完了条件: 主旋律を複製して、3 度のハモリが数分で作れる（Cubase と Studio One）。

仕様: `docs/specifications/editing.md`（度数シフト・ゴースト表示）

| ID | 単位 | REQ | 規模 | 依存 | 状態 |
|---|---|---|---|---|---|
| M2-01 | 度数シフトの計算（`ScaleUtils`）とテスト（スケール外の音を含む） | HARM-001 | S | — | 未着手 |
| M2-02 | 度数シフトの操作（ダイアログ・キー H・undo 1 回・警告色） | HARM-001 | S | M2-01 | 未着手 |
| M2-03 | 調査: ARA で他の Audio Modification のノートを参照する方法と、Cubase / Studio One での挙動 | HARM-002 | M | — | 未着手 |
| M2-04 | ゴーストのデータ取得（`ARADocumentController`）と更新の通知 | HARM-002 | M | M2-03 | 未着手 |
| M2-05 | ゴースト描画（半透明・ヒットテストなし）と表示トラックの選択 | HARM-002, UI-002 | M | M2-04 | 未着手 |
| M2-06 | 非 ARA での手順の確認（度数シフト → WAV 書き出し） | HARM-001 | S | M2-02 | 未着手 |
