/**
 * main.c — 畳み込み符号の誤り訂正能力評価メインプログラム
 *
 * 情報通信論 課題 (2026)
 *
 * 使い方:
 *   ./error_eval [--max-errors E] [--seed S] [--verbose]
 *
 * 出力:
 *   results.csv — 各符号器の誤り数ごとの訂正成功率
 *   標準出力   — 進捗表示
 */

#include "common.h"
#include "encoder.h"
#include "trellis.h"
#include "viterbi.h"
#include "error_inject.h"

/* ──────────── 設定 ──────────── */
#define DEFAULT_MAX_ERRORS    15
#define RANDOM_TRIALS         10000  /* e>=4 でのランダム試行回数 */
#define ZERO_STREAK_LIMIT     3      /* 成功率0が連続したら打ち切り */

/* ──────────── 全列挙で誤り訂正を評価 (e=1, e=2) ──────────── */

/**
 * 誤り数 num_errors の全パターンを列挙して訂正成功率を求める
 *   再帰的に誤り位置の組み合わせを生成
 */
static int enum_success_count;
static int enum_total_count;

static void enumerate_errors_recursive(
    const uint8_t *codeword, int code_len,
    const uint8_t *info_bits, int info_len,
    const Trellis *trellis, int n_delay,
    int *positions, int num_errors, int depth, int start)
{
    if (depth == num_errors) {
        /* この組み合わせで誤りを付加して復号 */
        uint8_t received[MAX_CODEWORD];
        uint8_t decoded[INFO_BITS + MAX_DELAY];

        inject_errors_at(codeword, received, code_len, positions, num_errors);
        viterbi_decode(trellis, received, code_len, decoded, n_delay);

        /* 情報100ビットで一致判定 */
        if (bits_equal(decoded, info_bits, info_len)) {
            enum_success_count++;
        }
        enum_total_count++;
        return;
    }

    for (int i = start; i < code_len; i++) {
        positions[depth] = i;
        enumerate_errors_recursive(codeword, code_len, info_bits, info_len,
                                   trellis, n_delay,
                                   positions, num_errors, depth + 1, i + 1);
    }
}

static void evaluate_enumerate(
    const uint8_t *codeword, int code_len,
    const uint8_t *info_bits, int info_len,
    const Trellis *trellis, int n_delay,
    int num_errors, int *out_success, int *out_total)
{
    int positions[16];  /* num_errors <= 3 を想定 */
    enum_success_count = 0;
    enum_total_count = 0;

    enumerate_errors_recursive(codeword, code_len, info_bits, info_len,
                               trellis, n_delay,
                               positions, num_errors, 0, 0);

    *out_success = enum_success_count;
    *out_total = enum_total_count;
}

/* ──────────── ランダム抽出で誤り訂正を評価 (e>=3) ──────────── */

static void evaluate_random(
    const uint8_t *codeword, int code_len,
    const uint8_t *info_bits, int info_len,
    const Trellis *trellis, int n_delay,
    int num_errors, int trials,
    int *out_success, int *out_total)
{
    int success = 0;
    uint8_t received[MAX_CODEWORD];
    uint8_t decoded[INFO_BITS + MAX_DELAY];

    for (int trial = 0; trial < trials; trial++) {
        inject_errors_random(codeword, received, code_len, num_errors);
        viterbi_decode(trellis, received, code_len, decoded, n_delay);

        if (bits_equal(decoded, info_bits, info_len)) {
            success++;
        }
    }

    *out_success = success;
    *out_total = trials;
}

/* ──────────── メイン ──────────── */

int main(int argc, char *argv[])
{
    /* ── コマンドライン引数解析 ── */
    int max_errors = DEFAULT_MAX_ERRORS;
    unsigned int seed = (unsigned int)time(NULL);
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--max-errors") == 0 && i + 1 < argc) {
            max_errors = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [--max-errors E] [--seed S] [--verbose]\n", argv[0]);
            return 0;
        }
    }

    srand(seed);
    printf("=== 畳み込み符号 誤り訂正能力評価 ===\n");
    printf("Seed: %u, Max errors: %d\n\n", seed, max_errors);

    /* ── 入力情報ビットの生成 ── */
    uint8_t info_bits[INFO_BITS];
    generate_random_bits(info_bits);

    if (verbose) {
        printf("Input info bits (%d bits): ", INFO_BITS);
        print_bits(info_bits, INFO_BITS);
    }

    /* ── 符号器の定義 ── */
    ConvEncoder encoders[4];
    conv_encoder_init(&encoders[0], "G1", 1, 01, 03);    /* (1, 3)_8 */
    conv_encoder_init(&encoders[1], "G2", 2, 05, 07);    /* (5, 7)_8 */
    conv_encoder_init(&encoders[2], "G3", 3, 05, 013);   /* (5, 13)_8 */
    conv_encoder_init(&encoders[3], "G4_opt", 3, 015, 017); /* (15, 17)_8 最適符号 */
    int num_encoders = 4;

    /* ── CSV出力ファイル ── */
    FILE *csv = fopen("../output/results.csv", "w");
    if (!csv) {
        fprintf(stderr, "Error: Cannot open ../output/results.csv for writing\n");
        return 1;
    }
    fprintf(csv, "encoder,n_delay,num_errors,num_trials,num_success,num_fail,success_rate\n");

    /* ── 各符号器について評価 ── */
    for (int enc_idx = 0; enc_idx < num_encoders; enc_idx++) {
        ConvEncoder *enc = &encoders[enc_idx];
        int m = enc->n_delay;

        printf("--- %s (delay=%d, K=%d, states=%d) ---\n",
               enc->name, m, enc->constraint_len, 1 << m);
        printf("  Generator polynomials: (%o, %o)_8\n",
               enc->gen_poly[0], enc->gen_poly[1]);

        /* トレリス構築 */
        Trellis trellis;
        trellis_build(&trellis, enc);

        if (verbose) {
            trellis_print(&trellis);
        }

        /* 符号化 */
        uint8_t codeword[MAX_CODEWORD];
        int code_len = conv_encode(enc, info_bits, INFO_BITS, codeword);

        printf("  Codeword length: %d bits\n", code_len);

        /* 誤りなしでの復号テスト (ラウンドトリップ検証) */
        {
            uint8_t decoded[INFO_BITS + MAX_DELAY];
            viterbi_decode(&trellis, codeword, code_len, decoded, m);
            if (bits_equal(decoded, info_bits, INFO_BITS)) {
                printf("  Round-trip test: PASS (error-free decode OK)\n");
            } else {
                printf("  Round-trip test: FAIL!\n");
                fprintf(stderr, "ERROR: Round-trip test failed for %s\n", enc->name);
                /* 失敗しても続行して問題箇所を確認 */
            }
        }

        /* 誤り数を増やして評価 */
        int zero_streak = 0;
        printf("  Errors | Trials |  Success |   Fail | Rate\n");
        printf("  -------+--------+----------+--------+---------\n");

        for (int e = 1; e <= max_errors && e <= code_len; e++) {
            int success = 0, total = 0;

            if (e <= 3) {
                /* 全パターン列挙 */
                evaluate_enumerate(codeword, code_len, info_bits, INFO_BITS,
                                   &trellis, m, e, &success, &total);
            } else {
                /* ランダム抽出 */
                evaluate_random(codeword, code_len, info_bits, INFO_BITS,
                                &trellis, m, e, RANDOM_TRIALS,
                                &success, &total);
            }

            double rate = (total > 0) ? (double)success / total : 0.0;
            printf("  %6d | %6d | %8d | %6d | %.4f\n",
                   e, total, success, total - success, rate);

            fprintf(csv, "%s,%d,%d,%d,%d,%d,%.6f\n",
                    enc->name, m, e, total, success, total - success, rate);

            /* 成功率0が連続したら打ち切り */
            if (success == 0) {
                zero_streak++;
                if (zero_streak >= ZERO_STREAK_LIMIT) {
                    printf("  (success rate = 0 for %d consecutive error counts, stopping)\n",
                           ZERO_STREAK_LIMIT);
                    break;
                }
            } else {
                zero_streak = 0;
            }
        }
        printf("\n");
    }

    fclose(csv);
    printf("Results written to ../output/results.csv\n");

    return 0;
}
