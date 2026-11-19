#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
variant=${PICOSTATION_VARIANT:-picostation_pico2}
cmake -S "$project_dir" -B "$project_dir/build/$variant" -DPICOSTATION_VARIANT="$variant"
cmake --build "$project_dir/build/$variant"
