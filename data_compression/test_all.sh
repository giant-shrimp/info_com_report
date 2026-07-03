#!/bin/bash
set -e

# Directories
BASE_DIR="$(pwd)"
C_DIR="${BASE_DIR}/c"
PY_DIR="${BASE_DIR}/python"
TEXT_DIR="${BASE_DIR}/textdata"
C_OUT_DIR="${C_DIR}/output"
PY_OUT_DIR="${PY_DIR}/output"

# Test data file
INPUT_FILE="${TEXT_DIR}/bible.txt"

# Ensure output directories exist
mkdir -p "${C_OUT_DIR}"
mkdir -p "${PY_OUT_DIR}"

echo "Compiling C files..."
cd "${C_DIR}"
gcc -O2 arith.c -o "${C_OUT_DIR}/arith_c"
gcc -O2 huffman.c -o "${C_OUT_DIR}/huffman_c"
gcc -O2 slide.c -o "${C_OUT_DIR}/slide_c"
gcc -O2 squeeze.c -o "${C_OUT_DIR}/squeeze_c"

echo "--- ARITH ---"
"${C_OUT_DIR}/arith_c" e "${INPUT_FILE}" "${C_OUT_DIR}/bible_arith_c.enc"
python3 "${PY_DIR}/arith.py" e "${INPUT_FILE}" "${PY_OUT_DIR}/bible_arith_py.enc"
cmp "${C_OUT_DIR}/bible_arith_c.enc" "${PY_OUT_DIR}/bible_arith_py.enc" && echo "Arith Match!"

echo "--- HUFFMAN ---"
"${C_OUT_DIR}/huffman_c" e "${INPUT_FILE}" "${C_OUT_DIR}/bible_huffman_c.enc"
python3 "${PY_DIR}/huffman.py" e "${INPUT_FILE}" "${PY_OUT_DIR}/bible_huffman_py.enc"
cmp "${C_OUT_DIR}/bible_huffman_c.enc" "${PY_OUT_DIR}/bible_huffman_py.enc" && echo "Huffman Match!"

echo "--- SLIDE ---"
"${C_OUT_DIR}/slide_c" e "${INPUT_FILE}" "${C_OUT_DIR}/bible_slide_c.enc"
python3 "${PY_DIR}/slide.py" e "${INPUT_FILE}" "${PY_OUT_DIR}/bible_slide_py.enc"
cmp "${C_OUT_DIR}/bible_slide_c.enc" "${PY_OUT_DIR}/bible_slide_py.enc" && echo "Slide Match!"

echo "--- SQUEEZE ---"
"${C_OUT_DIR}/squeeze_c" e "${INPUT_FILE}" "${C_OUT_DIR}/bible_squeeze_c.enc"
python3 "${PY_DIR}/squeeze.py" e "${INPUT_FILE}" "${PY_OUT_DIR}/bible_squeeze_py.enc"
cmp "${C_OUT_DIR}/bible_squeeze_c.enc" "${PY_OUT_DIR}/bible_squeeze_py.enc" && echo "Squeeze Match!"

echo "All tests passed successfully!"
