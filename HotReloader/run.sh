#!/bin/bash

ps -ef | grep 'WebServer' | grep -v grep | awk '{print $2}' | xargs -r ./build/HotReloader ../endpoints/