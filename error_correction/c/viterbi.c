/**
 * viterbi.c — 硬判定ビタビ復号器の実装
 *
 * 末尾ゼロ終端を前提とし、最終状態は必ず状態0に収束する。
 * そのためトレースバックは状態0から開始する。
 */

#include "viterbi.h"

/* パスメトリックの初期値（∞の代わり） */
#define PM_INF  (999999)

int viterbi_decode(const Trellis *trellis, const uint8_t *received,
                   int received_len, uint8_t *decoded, int n_delay)
{
    (void)n_delay;  /* 末尾ゼロ終端前提のため直接は使用しない */
    int num_states = trellis->num_states;
    int total_input = received_len / 2;  /* トレリスの段数 */

    /* ──────── メモリ確保 ──────── */

    /* パスメトリック（現在・前回） */
    int *pm_curr = (int *)malloc(sizeof(int) * num_states);
    int *pm_prev = (int *)malloc(sizeof(int) * num_states);

    /* 生存パス記録: survivor_state[t][s] = 時刻 t で状態 s に来た前状態 */
    int **survivor_state = (int **)malloc(sizeof(int *) * total_input);
    int **survivor_input = (int **)malloc(sizeof(int *) * total_input);
    for (int t = 0; t < total_input; t++) {
        survivor_state[t] = (int *)malloc(sizeof(int) * num_states);
        survivor_input[t] = (int *)malloc(sizeof(int) * num_states);
    }

    /* ──────── 初期化 ──────── */
    for (int s = 0; s < num_states; s++) {
        pm_prev[s] = PM_INF;
    }
    pm_prev[0] = 0;  /* 初期状態: 状態0のみ PM=0 */

    /* ──────── 前向き処理 (ACS) ──────── */
    for (int t = 0; t < total_input; t++) {
        /* 受信した2ビットを整数に変換 (bit0, bit1 の順) */
        int rx = 0;
        rx |= (received[2 * t + 0] & 1) << 0;
        rx |= (received[2 * t + 1] & 1) << 1;

        /* 全状態の PM を初期化 */
        for (int s = 0; s < num_states; s++) {
            pm_curr[s] = PM_INF;
        }

        /* 各前状態 → 各入力 → 次状態 */
        for (int prev_s = 0; prev_s < num_states; prev_s++) {
            if (pm_prev[prev_s] == PM_INF) continue;  /* 到達不能 */

            for (int input = 0; input < 2; input++) {
                int next_s = trellis->next_state[prev_s][input];
                int expected_out = trellis->output[prev_s][input];

                /* ブランチメトリック: ハミング距離 */
                int bm = hamming_distance_int(rx, expected_out, 2);

                /* ACS: Add-Compare-Select */
                int new_pm = pm_prev[prev_s] + bm;
                if (new_pm < pm_curr[next_s]) {
                    pm_curr[next_s] = new_pm;
                    survivor_state[t][next_s] = prev_s;
                    survivor_input[t][next_s] = input;
                }
            }
        }

        /* pm_curr → pm_prev にスワップ */
        int *tmp = pm_prev;
        pm_prev = pm_curr;
        pm_curr = tmp;
    }

    /* ──────── トレースバック ──────── */
    /* 末尾ゼロ終端のため、最終状態は必ず状態0 */
    int state = 0;

    for (int t = total_input - 1; t >= 0; t--) {
        decoded[t] = (uint8_t)survivor_input[t][state];
        state = survivor_state[t][state];
    }

    /* ──────── メモリ解放 ──────── */
    for (int t = 0; t < total_input; t++) {
        free(survivor_state[t]);
        free(survivor_input[t]);
    }
    free(survivor_state);
    free(survivor_input);
    free(pm_curr);
    free(pm_prev);

    return total_input;
}
