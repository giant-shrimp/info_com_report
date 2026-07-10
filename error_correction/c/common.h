/**
 * common.h — 共通定義・ユーティリティ
 *
 * 畳み込み符号の誤り訂正能力評価プログラム
 * 情報通信論 課題 (2026)
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* ──────────── 定数 ──────────── */
#define INFO_BITS       100   /* 情報ビット数 */
#define MAX_DELAY       4     /* 遅延素子数の上限 */
#define MAX_STATES      (1 << MAX_DELAY)  /* 状態数の上限 = 16 */
#define MAX_CODEWORD    (2 * (INFO_BITS + MAX_DELAY))  /* 符号語の最大長 */

/* ──────────── 乱数生成 ──────────── */

/**
 * 0/1がほぼ等分の100ビット乱数を生成 (Fisher-Yates shuffle)
 * bits: 出力先配列 (少なくとも INFO_BITS 要素)
 */
static inline void generate_random_bits(uint8_t *bits)
{
    int i;
    /* 前半50個を0、後半50個を1で初期化 */
    for (i = 0; i < INFO_BITS / 2; i++) bits[i] = 0;
    for (i = INFO_BITS / 2; i < INFO_BITS; i++) bits[i] = 1;

    /* Fisher-Yates shuffle */
    for (i = INFO_BITS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        uint8_t tmp = bits[i];
        bits[i] = bits[j];
        bits[j] = tmp;
    }
}

/* ──────────── ハミング距離 ──────────── */

/**
 * 2つのビット配列間のハミング距離を計算
 */
static inline int hamming_distance_bits(const uint8_t *a, const uint8_t *b, int len)
{
    int dist = 0;
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) dist++;
    }
    return dist;
}

/**
 * 2つの整数のハミング距離（ビット単位、n_bits ビット分）
 */
static inline int hamming_distance_int(int a, int b, int n_bits)
{
    int x = a ^ b;
    int dist = 0;
    for (int i = 0; i < n_bits; i++) {
        dist += (x >> i) & 1;
    }
    return dist;
}

/* ──────────── ビット配列表示 ──────────── */

static inline void print_bits(const uint8_t *bits, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%d", bits[i]);
        if ((i + 1) % 10 == 0 && i + 1 < len) printf(" ");
    }
    printf("\n");
}

/**
 * 2つのビット配列の先頭 len ビットが一致するか
 */
static inline int bits_equal(const uint8_t *a, const uint8_t *b, int len)
{
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

#endif /* COMMON_H */
