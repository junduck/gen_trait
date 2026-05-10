#!/bin/bash
shopt -s nullglob

json_dir="${1:-json}"
wd=$(realpath "$json_dir")
out_dir="$(dirname "$wd")/generated"
script_dir=$(dirname "$0")

mkdir -p "$out_dir"

for file in "$wd"/*.json; do
  filename=$(basename -- "$file")
  if [[ "$filename" != "gen_trait.schema.json" ]]; then
    python3 -m gen_trait "$file" > "$out_dir/${filename%.*}.hpp"
  fi
done

if command -v clang-format &> /dev/null; then
  for file in "$out_dir"/*.hpp; do
    clang-format -i "$file"
  done
else
  echo "clang-format not found, skip formatting"
fi

shopt -u nullglob
