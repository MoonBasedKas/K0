#!/usr/bin/env bash
shopt -s nullglob # shell option to expand *.c that matches nothing to empty

# if the first argument is -c or --clean, clean the output directory
if [[ "$1" == "-c" || "$1" == "--clean" ]]; then
  echo "Cleaning ./output/*.out…"
  rm -f output/*.out
  exit 0
fi

mkdir -p output

for src in *.c; do
  base="${src%.c}"
  out="output/${base}.out"

  {
    echo "===== Source: $src ====="
    echo
    cat "$src"
    echo
    echo "===== Assembly (gcc -S) ====="
    echo
    gcc -S "$src" -o - 
  } > "$out"

  echo "  → Wrote source+asm to $out" # alt+26 numpad
done
