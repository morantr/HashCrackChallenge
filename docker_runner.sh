#!/bin/bash

scriptdir=$(cd $(dirname $0) && pwd)
workdir="/home/cracker"
image=hashcracker:latest

run() {
    echo "$*"
    "$@" || exit $?
}

usage() {
    echo "usage: $(basename "$0") [OPTIONS]"
    echo "  options:"
    echo "  -h|--help       - Show this help menu"
    echo "  -t|--terminal   - Open the terminal on the container"
    echo "  -r|--run        - Run the program"
    echo ""
}

main() {

    if ! OPTS=$(getopt -o 'htr' --long help,terminal,run -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)               usage; exit 0; shift ;;
            -t | --terminal)           TERMINAL="-it"; shift ;;
            -r | --run)                RUN="--entrypoint ./build/hashCracker"; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done



    image="hashcracker:latest"
    # Default docker arguments
    docker_args=(
        # Automatically remove the container when it exits
        --rm

        # Working directory inside the container
        --workdir "${workdir}"

        # Bind mount a volume. Binds the host directory to the docker.
        # It is possible to replace "${scriptdir}" with "/" and get access to all files on
        # the machine.
        -v "${scriptdir}:${workdir}"
    )

    if [ -n "$TERMINAL" ]; then
        docker_args+=("${TERMINAL}")
    fi

    if [ -n "$RUN" ]; then
        docker_args+=(${RUN})
    fi

    run docker run "${docker_args[@]}" hashcracker:latest
}

TERMINAL=""
RUN=""

main "$@"
