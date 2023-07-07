#!/bin/sh
ps -a | grep WebServer | awk '{print $1}' | xargs lsof -p | grep __middleware__.*\\.so
