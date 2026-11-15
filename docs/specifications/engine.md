# 合成エンジン

関連: REQ-ENG-001〜004、ADR-0003・0004

## エンジン

| 名前 | 中身 | 得意 | 入力 | 状態 |
|---|---|---|---|---|
| AI | PC-NSF-HiFiGAN | 大きな補正、しゃくりの描き直し、ピッチカーブの大きな変更 | mel ＋ F0 | 既存 |
| Classic | PSOLA（時間領域） | 小さな補正、ロングトーン。比率 1.0 で原音と一致 | `DspSynthesisRequest` | 既存 |
| Spectral | STFT ＋ スペクトル包絡の保持 | 中程度の補正でのフォルマント保持（Vovious に近い） | `DspSynthesisRequest` | M4 で新規 |

## ノート単位の選択（M3）

- `Note` に `engine`（`Default` / `AI` / `Classic` / `Spectral`）を持たせる。`Default` はプロジェクトの既定、M5 以降は自動ルーティングに従う
- 旧プロジェクトはすべて `Default` として読み、プロジェクトの既定に旧 `SynthesisEngineType` を入れる
- `IncrementalSynthesizer` は、再合成する範囲をエンジンが同じ区間に分け、区間ごとに合成する
- 境界のクロスフェード: 長さ 10ms の等パワー。境界が無声フレームに入る場合は、原音を通すブレンドマスクに任せてクロスフェードしない
- 出力のブレンドはしない。2 つのエンジンが同じフレームを鳴らすのはクロスフェード中だけ

## Spectral エンジン（M4）

```
source ─► STFT ─► 包絡推定 E(f) ─► 平坦化 X/E ─► 周波数軸を pitchRatio で伸縮 ─► ×E(f)（formant 保持時）─► 位相再構成 ─► ISTFT
```

- 窓とホップ: `hopSize`（既定 512）に合わせる。窓長は F0 から決める（仮: 4 周期以上）
- 位相: 位相ロック型のフェーズボコーダー（ピーク周辺の位相を固定）。比率 1.0 のときは入力の位相をそのまま使い、原音と一致させる
- 時間の対応は `sourceFrame` に従う（タイミング編集にもそのまま対応）
- 本家 PitchNet の過去のフェーズボコーダー実装（git 履歴、ケプストラム包絡あり）を出発点にできるか、M4 の最初に確認する

### 包絡推定器

```cpp
// shortcut: interface sketch, finalized in the M4 plan
struct EnvelopeEstimator {
  virtual ~EnvelopeEstimator() = default;
  // magnitude: one STFT frame; f0: Hz (0 = unvoiced); writes envelope of same size
  virtual void estimate(const float* magnitude, int numBins, float f0, int sampleRate, float* envelope) = 0;
};
```

| 実装 | 方式 | M |
|---|---|---|
| `TrueEnvelopeEstimator` | ケプストラムの反復平滑化。次数は F0 から決める | M4 |
| `LpcEnvelopeEstimator` | LPC（比較用） | M4（必要なら） |
| ニューラル推定器 | ONNX | 未定（backlog） |

### 評価

- 評価セット: 公開ライセンスの歌声 3〜5 種（男女・地声と裏声）に、±1・±3・±7 半音の補正をかけたもの
- 判定方法は M4 の計画で決める（候補: ブラインド ABX、mel ケプストラム歪み）
- 打ち切り条件: PSOLA と AI のどちらよりも自然な場面がない場合、Spectral を既定の選択肢から外す

## 自動ルーティング（M5）

区間ごとに、次の特徴から決める。規則ベースで始め、同じ入力には常に同じ結果を返す。

| 特徴 | 仮の規則 |
|---|---|
| 補正量（cents） | 50 未満 → Classic、300 以上 → AI |
| ピッチカーブを描き直したか | 描き直した → AI |
| 遷移部（ノート境界 ±40ms） | 両側のノートのエンジンに合わせる |
| 中間 | Spectral（M4 で採用された場合）、なければ Classic |

しきい値は M4 の評価結果で決め直す。
