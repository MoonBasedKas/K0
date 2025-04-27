#!/usr/bin/env bash
shopt -s nullglob # shell option to expand *.c that matches nothing to empty

# Cleaner
if [[ "$1" == "-c" || "$1" == "--clean" ]]; then
  echo "Cleaning ./output/*.out…"
  rm -f output/*.out
  exit 0
fi

# Autobots assemble!
if [[ "$1" == "-m" || "$1" == "--merge" ]]; then
  mkdir -p output
  merged="output/Lab11_Output.out"

  # README first
  {
    echo "===== README ====="
    cat output/README
    echo
  } > "$merged"

  #  Tried for complexity but I'm dumb so it doesn't matter
  merge_order=(
    sooperBasic
    simpFunc
    multiFunc
    cmplxFunc
    assignments
    ifCF
    elseCF
    whileCF
    expressions
    expressions2
    expressions3
    literals
    literals2
    local_add
    global_add
    arrayBool
    arrayInt
    arrayStr
    builtIns_io
    builtIns_string
    builtIns_math
    returns
  )

  # loop in order
  for base in "${merge_order[@]}"; do
    file="output/${base}.out"
    if [[ -f "$file" ]]; then
      {
        echo
        cat "$file"
        echo
      } >> "$merged"
    else
      echo "Warning: $file not found" >&2
    fi
  done

  echo "→ Merged into $merged"
  exit 0
fi

# .out files
mkdir -p output

for src in *.c; do
  base="${src%.c}"
  out="output/${base}.out"

  {
    echo "===== Source: $src ====="
    echo
    cat "$src"
    echo
    echo
    echo "===== Assembly (gcc -S) ====="
    echo
    gcc -S "$src" -o - 

    echo
    echo "===== Discussion ====="
    echo 
  } > "$out"

  echo "  → Wrote source+asm to $out" # alt+26 numpad
done
