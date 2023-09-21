#!/bin/bash

set -e

proj_dir=$(pwd)

function custom_wait()
{
    read -p "Press enter to continue"
    return 0
}

function rm_build_dirs()
{
    rm -rf "$proj_dir/host_side/build"
    rm -rf "$proj_dir/pico_side/build"
    return 0
}

function configure()
{
    mkdir -p "$proj_dir/host_side/build"
    cd "$proj_dir/host_side/build"
    cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w ..
    custom_wait

    mkdir -p "$proj_dir/pico_side/build"
    cd "$proj_dir/pico_side/build"
    cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w ..
    custom_wait


    cd "$proj_dir"

    return 0
}

function build()
{
    cd "$proj_dir/host_side/build"
    make -j$(nproc) all
    custom_wait

    cd "$proj_dir/pico_side/build"
    make -j$(nproc) all
    custom_wait

    cd "$proj_dir"

    return 0
}

function main()
{
    rm_build_dirs
    configure
    build
    return 0
}

main