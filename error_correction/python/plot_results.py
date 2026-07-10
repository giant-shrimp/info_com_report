#!/usr/bin/env python3
"""
plot_results.py — 畳み込み符号 誤り訂正能力評価グラフ作成

results.csv を読み込み、各符号器の誤り数 vs 訂正成功率をプロットする。
"""

import csv
import sys
import os

import matplotlib
matplotlib.use('Agg')  # GUIなし環境対応
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm


def load_results(csv_path):
    """CSVファイルを読み込み、符号器ごとにデータを整理"""
    data = {}
    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            name = row['encoder']
            if name not in data:
                data[name] = {
                    'n_delay': int(row['n_delay']),
                    'errors': [],
                    'rates': [],
                    'trials': [],
                    'successes': []
                }
            data[name]['errors'].append(int(row['num_errors']))
            data[name]['rates'].append(float(row['success_rate']))
            data[name]['trials'].append(int(row['num_trials']))
            data[name]['successes'].append(int(row['num_success']))
    return data


def plot_results(data, output_path='../output/error_correction_results.png'):
    """誤り数 vs 訂正成功率のグラフを作成"""
    # フォント設定（日本語対応を試みる）
    jp_fonts = [f.name for f in fm.fontManager.ttflist
                if 'Gothic' in f.name or 'Noto Sans CJK' in f.name or 'IPAGothic' in f.name]
    if jp_fonts:
        plt.rcParams['font.family'] = jp_fonts[0]
    else:
        plt.rcParams['font.family'] = 'DejaVu Sans'

    fig, ax = plt.subplots(figsize=(10, 6))

    colors = {'G1': '#e74c3c', 'G2': '#3498db', 'G3': '#2ecc71'}
    markers = {'G1': 'o', 'G2': 's', 'G3': '^'}

    for name in sorted(data.keys()):
        d = data[name]
        label = f"{name} (m={d['n_delay']}, K={d['n_delay']+1})"
        ax.plot(d['errors'], d['rates'],
                marker=markers.get(name, 'o'),
                color=colors.get(name, '#333333'),
                linewidth=2, markersize=6,
                label=label, alpha=0.9)

    ax.set_xlabel('Number of Errors in Codeword', fontsize=12)
    ax.set_ylabel('Correction Success Rate', fontsize=12)
    ax.set_title('Error Correction Capability of Convolutional Codes\n'
                 '(1-input, 2-output, Hard Decision Viterbi Decoding)',
                 fontsize=13, fontweight='bold')

    ax.set_ylim(-0.05, 1.05)
    ax.set_xlim(left=0)
    ax.axhline(y=1.0, color='gray', linestyle='--', alpha=0.3)
    ax.axhline(y=0.0, color='gray', linestyle='--', alpha=0.3)
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=11, loc='best')

    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    print(f"Graph saved to {output_path}")
    plt.close()


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else '../output/results.csv'

    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found. Run ./error_eval first.", file=sys.stderr)
        sys.exit(1)

    data = load_results(csv_path)
    plot_results(data)

    # 概要表示
    print("\n=== Summary ===")
    for name in sorted(data.keys()):
        d = data[name]
        # 成功率が初めて1.0を下回る誤り数
        first_fail = None
        for i, r in enumerate(d['rates']):
            if r < 1.0:
                first_fail = d['errors'][i]
                break
        # 成功率が初めて0.0になる誤り数
        first_zero = None
        for i, r in enumerate(d['rates']):
            if r == 0.0:
                first_zero = d['errors'][i]
                break
        print(f"  {name} (m={d['n_delay']}): "
              f"first failure at e={first_fail}, "
              f"first zero at e={first_zero}")


if __name__ == '__main__':
    main()
