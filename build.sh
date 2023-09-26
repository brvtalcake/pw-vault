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
    if [ -d "$proj_dir/host_side/build" ]; then
        echo "Removing host build directory"
        rm -rf "$proj_dir/host_side/build"
    fi
    if [ -d "$proj_dir/pico_side/build" ]; then
        echo "Removing pico build directory"
        rm -rf "$proj_dir/pico_side/build"
    fi
    return 0
}

function configure()
{
    if [ $# -eq 0 ]; then
        echo "Configuring all"
        mkdir -p "$proj_dir/host_side/build"
        cd "$proj_dir/host_side/build"
        cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w ..
        custom_wait
    elif [ $# -eq 1 ]; then
        if [ "$1" == "host" ]; then
            echo "Configuring host"
            mkdir -p "$proj_dir/host_side/build"
            cd "$proj_dir/host_side/build"
            cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w ..
            custom_wait
        elif [ "$1" == "pico" ]; then
            echo "Configuring pico"
            mkdir -p "$proj_dir/pico_side/build"
            cd "$proj_dir/pico_side/build"
            cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w ..
            custom_wait
        else
            echo "Invalid argument"
            cd "$proj_dir"
            return 1
        fi
    else
        echo "Invalid number of arguments"
        cd "$proj_dir"
        return 1
    fi

    cd "$proj_dir"

    return 0
}

function build()
{
    if [ $# -eq 0 ]; then
        echo "Building all"
        cd "$proj_dir/host_side/build"
        make all
        custom_wait
        cd "$proj_dir/pico_side/build"
        make all
        custom_wait
    elif [ $# -eq 1 ]; then
        if [ "$1" == "host" ]; then
            echo "Building host"
            cd "$proj_dir/host_side/build"
            make all
            custom_wait
        elif [ "$1" == "pico" ]; then
            echo "Building pico"
            cd "$proj_dir/pico_side/build"
            make all
            custom_wait
        else
            echo "Invalid argument"
            cd "$proj_dir"
            return 1
        fi
    else
        echo "Invalid number of arguments"
        cd "$proj_dir"
        return 1
    fi

    cd "$proj_dir"

    return 0
}

function main()
{
    rm_build_dirs
    if [ $# -eq 0 ]; then
        echo "Processing all"
        configure
        build
    elif [ $# -eq 1 ]; then
        if [ "$1" == "host" ]; then
            echo "Processing host"
            configure host
            build host
        elif [ "$1" == "pico" ]; then
            echo "Processing pico"
            configure pico
            build pico
        else
            echo "Invalid argument"
            return 1
        fi
    else
        echo "Invalid number of arguments"
        return 1
    fi
    return 0
}

main "$@"