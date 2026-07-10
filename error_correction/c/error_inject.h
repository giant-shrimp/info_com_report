/**
 * error_inject.h — 誤り付加
 *
 * 符号語にランダムに指定個数のビット誤りを付加する。
 */

#ifndef ERROR_INJECT_H
#define ERROR_INJECT_H

#include "common.h"

/**
 * 符号語にランダムに誤りを付加して受信語を作成
 *   codeword:     元の符号語
 *   received:     誤りを付加した受信語（出力先、codeword とは別領域）
 *   code_len:     符号語のビット数
 *   num_errors:   付加する誤りの数
 *
 * codeword をコピーし、ランダムに num_errors 箇所のビットを反転する。
 */
void inject_errors_random(const uint8_t *codeword, uint8_t *received,
                          int code_len, int num_errors);

/**
 * 符号語の指定位置に誤りを付加して受信語を作成
 *   codeword:     元の符号語
 *   received:     誤りを付加した受信語（出力先）
 *   code_len:     符号語のビット数
 *   positions:    誤りを付加するビット位置の配列
 *   num_errors:   誤りの数
 */
void inject_errors_at(const uint8_t *codeword, uint8_t *received,
                      int code_len, const int *positions, int num_errors);

#endif /* ERROR_INJECT_H */
