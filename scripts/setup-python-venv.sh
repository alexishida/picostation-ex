#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 -m venv "$project_dir/env"
"$project_dir/env/bin/python" -m pip install --upgrade pip
"$project_dir/env/bin/python" -m pip install -r "$project_dir/third_party/ps1-bare-metal/tools/requirements.txt"
