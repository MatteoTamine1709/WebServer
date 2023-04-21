#!/bin/bash

function cleanup {
    # send sigint to all screens
    screen -X -S WebServer quit
    screen -X -S HotReloader quit
}

trap cleanup INT

screen -dm -RR -S WebServer bash -c "./build/WebServer; exec bash"
screen -dm -RR -S HotReloader bash -c "./HotReloader/build/HotReloader; exec bash"
screen -r WebServer
