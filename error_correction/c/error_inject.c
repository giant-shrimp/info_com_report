/**
 * error_inject.c — 誤り付加の実装
 */

#include "error_inject.h"

void inject_errors_random(const uint8_t *codeword, uint8_t *received,
                          int code_len, int num_errors)
{
    /* まず符号語をコピー */
    memcpy(received, codeword, sizeof(uint8_t) * code_len);

    if (num_errors <= 0 || num_errors > code_len) return;

    /* 誤り位置をランダムに選択（重複なし）*/
    /* Fisher-Yates の部分シャッフルを使用 */
    int *indices = (int *)malloc(sizeof(int) * code_len);
    for (int i = 0; i < code_len; i++) indices[i] = i;

    for (int i = 0; i < num_errors; i++) {
        int j = i + rand() % (code_len - i);
        /* swap */
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    /* 選ばれた位置のビットを反転 */
    for (int i = 0; i < num_errors; i++) {
        received[indices[i]] ^= 1;
    }

    free(indices);
}

void inject_errors_at(const uint8_t *codeword, uint8_t *received,
                      int code_len, const int *positions, int num_errors)
{
    /* まず符号語をコピー */
    memcpy(received, codeword, sizeof(uint8_t) * code_len);

    /* 指定位置のビットを反転 */
    for (int i = 0; i < num_errors; i++) {
        if (positions[i] >= 0 && positions[i] < code_len) {
            received[positions[i]] ^= 1;
        }
    }
}
