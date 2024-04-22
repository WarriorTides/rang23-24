#!/bin/bash


HOSTNAME="192.168.1.101"

if [ "$1" == "-b" ]; then
    HOSTNAME="192.168.1.102"
    PYTHON_ARG="-b"
else
    PYTHON_ARG=""
fi

# Define a function to be called when SIGINT is received
function handle_sigint() {
    echo "Ctrl+C pressed, exiting gracefully..."
    exit 0
}

# Use trap to catch SIGINT and call handle_sigint when it's received
trap handle_sigint SIGINT

# Wait for the specified hostname to respond to ping

    while true; do #ping until the hostname responds
        ping -c 1 $HOSTNAME > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo "$HOSTNAME is reachable, proceeding..."
            break
        else
            echo "Waiting for $HOSTNAME to respond..."
            sleep 0.1
        fi
    done

    

ssh pi@$HOSTNAME