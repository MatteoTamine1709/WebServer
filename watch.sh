#!/bin/bash

screen -d -m "./HotReloader/build/HotReloader" && ./build/WebServer ; killall screen