/**
 * encoder.c — 畳み込み符号器の実装
 */

#include "encoder.h"

void conv_encoder_init(ConvEncoder *enc, const char *name,
                       int n_delay, int g1, int g2)
{
    enc->name = name;
    enc->n_output = 2;
    enc->n_delay = n_delay;
    enc->constraint_len = n_delay + 1;
    enc->gen_poly[0] = g1;
    enc->gen_poly[1] = g2;
}

int conv_encode(const ConvEncoder *enc, const uint8_t *info_bits,
                int info_len, uint8_t *codeword)
{
    int m = enc->n_delay;
    int K = enc->constraint_len;  /* K = m + 1 */
    int total_input = info_len + m;  /* 末尾ゼロ追加後の入力長 */
    int code_len = 0;

    /* シフトレジスタ (最大 MAX_DELAY+1 ビット分) を 0 初期化 */
    int shift_reg = 0;  /* bit i = D^i のタップ位置の値 */

    for (int t = 0; t < total_input; t++) {
        /* 入力ビット (末尾ゼロ部分は 0) */
        int input_bit = (t < info_len) ? info_bits[t] : 0;

        /* シフトレジスタ更新: 新入力を bit 0 に、古い値を上位へシフト */
        shift_reg = ((shift_reg << 1) | input_bit) & ((1 << K) - 1);

        /* 各生成多項式に対して出力ビットを計算 */
        for (int j = 0; j < 2; j++) {
            int g = enc->gen_poly[j];
            /* g と shift_reg の AND をとり、ビット数の偶奇で出力決定 */
            int tmp = g & shift_reg;
            int out = 0;
            for (int b = 0; b < K; b++) {
                out ^= (tmp >> b) & 1;
            }
            codeword[code_len++] = (uint8_t)out;
        }
    }

    return code_len;
}
