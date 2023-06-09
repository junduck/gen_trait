#!/bin/bash
shopt -s nullglob

path="${1:-.}"
wd=$(realpath "$path")
py=$(dirname "$0")/gen_trait.py

for file in "$wd"/*.json; do
  filename=$(basename -- "$file")
  if [[ "$filename" != "gen_trait.schema.json" ]]; then
    python3 "$py" "$file" > "$wd/${filename%.*}.hpp"
  fi
done

if command -v clang-format &> /dev/null; then
  for file in "$wd"/*.hpp; do
    clang-format -i "$file"
  done
else
  echo "clang-format not found, skip formatting"
fi

shopt -u nullglob
