#!/usr/bin/env bash

sudo docker build -t gba-dev .
sudo docker run --network="host" --rm --volume $PWD:/build -it --workdir "/build" gba-dev make "$@"
