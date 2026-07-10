/**
 * encoder.h — 畳み込み符号器
 *
 * 1ビット入力・2ビット出力の畳み込み符号器
 * 生成多項式をパラメータとして受け取り、汎用的に符号化を行う。
 *
 * ビット並び規約:
 *   整数 g のビット i (LSBから数えて i 番目) を D^i のタップとする。
 *   すなわち (g >> i) & 1 が D^i の係数を返す。
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "common.h"

/* ──────────── 符号器構造体 ──────────── */

typedef struct {
    const char *name;       /* 符号器名 (例: "G1", "G2", "G3") */
    int n_output;           /* 出力ビット数 (= 2) */
    int n_delay;            /* 遅延素子数 m */
    int constraint_len;     /* 拘束長 K = m + 1 */
    int gen_poly[2];        /* 生成多項式 g1, g2（8進値を10進整数で格納）*/
} ConvEncoder;

/**
 * 符号器を初期化
 *   name:     符号器名
 *   n_delay:  遅延素子数 m
 *   g1, g2:   生成多項式（8進数を10進整数で。例: 07 → 7）
 */
void conv_encoder_init(ConvEncoder *enc, const char *name,
                       int n_delay, int g1, int g2);

/**
 * 畳み込み符号化を行う
 *   enc:          符号器
 *   info_bits:    入力情報ビット列（長さ info_len）
 *   info_len:     入力情報ビット数
 *   codeword:     出力符号語（長さ 2*(info_len + n_delay)）
 *   返り値:       符号語のビット数
 *
 * 末尾に m 個の 0 を自動追加して符号化する。
 */
int conv_encode(const ConvEncoder *enc, const uint8_t *info_bits,
                int info_len, uint8_t *codeword);

#endif /* ENCODER_H */
