#!/bin/bash

set -e

PWV_PATH_defined=$(echo $PWV_PATH)
if [ -z "$PWV_PATH_defined" ]; then
    full_path=$(readlink -f "$0")
    this_script_path=$(dirname "$full_path")
    echo "Assuming PWV_PATH is $this_script_path/../"
    export PWV_PATH="$this_script_path/../"
fi

PICO_SDK_PATH_defined=$(echo $PICO_SDK_PATH)
if [ -z "$PICO_SDK_PATH_defined" ]; then
    echo "PICO_SDK_PATH is not defined. Please define it in your .bashrc file or in your current session."
    exit 1
fi

find $PICO_SDK_PATH/src -name '*.h' -exec grep -A10 -B1 --color=no PICO_CONFIG {} \; | awk -F ' ' -e '{if ($1~"//" || $0~"--") {print $0}}' > $PWV_PATH/PICO_CONFIG.c 