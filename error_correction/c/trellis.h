/**
 * trellis.h — トレリス図（状態遷移テーブル・出力テーブル）
 *
 * 畳み込み符号器のパラメータから自動的にトレリス図を構築する。
 * ビタビ復号器が参照する核心データ。
 */

#ifndef TRELLIS_H
#define TRELLIS_H

#include "common.h"
#include "encoder.h"

/* ──────────── トレリス構造体 ──────────── */

typedef struct {
    int num_states;                         /* 状態数 = 2^m */
    int n_delay;                            /* 遅延素子数 m */
    int n_output;                           /* 出力ビット数 (= 2) */
    int next_state[MAX_STATES][2];          /* next_state[state][input] */
    int output[MAX_STATES][2];              /* output[state][input] (2ビットを整数で) */
} Trellis;

/**
 * 符号器パラメータからトレリス図を構築
 *   trellis: 出力先
 *   enc:     符号器
 */
void trellis_build(Trellis *trellis, const ConvEncoder *enc);

/**
 * トレリス図の状態遷移テーブルを表示（デバッグ用）
 */
void trellis_print(const Trellis *trellis);

#endif /* TRELLIS_H */
