/**
 * trellis.c — トレリス図の構築
 */

#include "trellis.h"

void trellis_build(Trellis *trellis, const ConvEncoder *enc)
{
    int m = enc->n_delay;
    int K = enc->constraint_len;
    int num_states = 1 << m;

    trellis->num_states = num_states;
    trellis->n_delay = m;
    trellis->n_output = 2;

    /*
     * シフトレジスタの表現 (encoder.c と同一):
     *   shift_reg = (old_shift_reg << 1) | input_bit
     *   bit 0 = 現在の入力 u_k (最新)
     *   bit 1 = u_{k-1}, ..., bit m = u_{k-m} (最古)
     *
     * 状態 = 遅延素子の内容 = shift_reg の bit 1..m
     *       = state = shift_reg >> 1
     *
     * 次状態 = 時刻 t の shift_reg の下位 m ビット
     *        = (u_k, u_{k-1}, ..., u_{k-m+1})
     *        → next = shift_reg & (num_states - 1)
     */
    for (int state = 0; state < num_states; state++) {
        for (int input = 0; input < 2; input++) {
            /* シフトレジスタ全体を構成: bit0=input, bit1..m=state */
            int shift_reg = (state << 1) | input;

            /* 出力ビットを計算 */
            int out = 0;
            for (int j = 0; j < 2; j++) {
                int g = enc->gen_poly[j];
                int tmp = g & shift_reg;
                int bit = 0;
                for (int b = 0; b < K; b++) {
                    bit ^= (tmp >> b) & 1;
                }
                out |= (bit << j);  /* bit0 = g1の出力, bit1 = g2の出力 */
            }

            /* 次状態: shift_reg の下位 m ビット */
            int next = shift_reg & (num_states - 1);

            trellis->next_state[state][input] = next;
            trellis->output[state][input] = out;
        }
    }
}

void trellis_print(const Trellis *trellis)
{
    printf("=== Trellis (states=%d, delay=%d) ===\n",
           trellis->num_states, trellis->n_delay);
    printf("State | Input | Next | Output\n");
    printf("------+-------+------+-------\n");

    for (int s = 0; s < trellis->num_states; s++) {
        for (int in = 0; in < 2; in++) {
            int next = trellis->next_state[s][in];
            int out = trellis->output[s][in];
            /* 出力を2ビット表示 (bit0, bit1) */
            printf("  %*d  |   %d   |  %*d  |  %d%d\n",
                   (trellis->n_delay > 2) ? 2 : 1, s,
                   in,
                   (trellis->n_delay > 2) ? 2 : 1, next,
                   (out >> 0) & 1, (out >> 1) & 1);
        }
    }
    printf("\n");
}
