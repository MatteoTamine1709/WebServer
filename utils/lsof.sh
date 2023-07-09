#!/bin/sh
ps -ax | grep WebServer | head -n 1 | awk '{print $1}' | xargs lsof -p
