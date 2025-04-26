#!/usr/bin/env bash
shopt -s nullglob  # so *.c that matches nothing expands to empty

for src in *.c; do
  base="${src%.c}"
  out="${base}.txt"

  {
    echo "===== Source: $src ====="
    cat "$src"
    echo
    echo "===== Assembly (gcc -S) ====="
    # -fverbose-asm will interleave C comments if you like, or omit it otherwise
    gcc -S "$src" -o -  
  } > "$out"

  echo "  → Wrote source+asm to $out"
done
