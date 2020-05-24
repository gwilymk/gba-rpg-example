#!/usr/bin/env bash

docker build -t gba-dev .
docker run --network="host" --rm --volume $PWD:/build -it --workdir "/build" gba-dev make "$@"
