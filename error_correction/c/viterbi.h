/**
 * viterbi.h — 硬判定ビタビ復号器
 *
 * トレリス図に基づく硬判定 (Hard Decision) ビタビ復号を行う。
 * アルゴリズム:
 *   1. ブランチメトリック (BM) 計算 — ハミング距離
 *   2. パスメトリック (PM) 更新 — ACS (Add-Compare-Select)
 *   3. トレースバック — 最終状態0から遡り復号ビット列を復元
 */

#ifndef VITERBI_H
#define VITERBI_H

#include "common.h"
#include "trellis.h"

/**
 * ビタビ復号を行う
 *   trellis:      トレリス図
 *   received:     受信語（ビット配列、長さ received_len）
 *   received_len: 受信語のビット数（= 2 * total_input）
 *   decoded:      復号結果の出力先（情報ビット + 末尾ゼロ）
 *   n_delay:      遅延素子数（末尾ゼロ分を含むトレリス段数の計算に使用）
 *   返り値:       復号した情報ビット数（末尾ゼロを含む total_input）
 */
int viterbi_decode(const Trellis *trellis, const uint8_t *received,
                   int received_len, uint8_t *decoded, int n_delay);

#endif /* VITERBI_H */
