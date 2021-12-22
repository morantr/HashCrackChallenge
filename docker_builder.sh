#!/bin/bash

scriptdir=$(cd $(dirname $0) && pwd)
workdir="/home/cracker"
image=hashcracker:latest

run() {
    echo "$*"
    "$@" || exit $?
}

# Build docker image if not exist
if [[ "$(docker image inspect ${image} 2> /dev/null)" == [] ]]; then
  run docker build -f  ${scriptdir}/tools/docker/Dockerfile . -t hashcracker
fi

main() {
    image="hashcracker:latest"
    # Default docker arguments
    docker_args=(
        # Automatically remove the container when it exits
        --rm
        
        # Working directory inside the container
        --workdir "${workdir}"

        # Bind mount a volume. Binds the host directory to the docker.
        -v "${scriptdir}:${workdir}"

        # Execute the following on container entry
        --entrypoint "./build.sh"
    )

    run docker run "${docker_args[@]}" -it hashcracker:latest
}

main "$@"
