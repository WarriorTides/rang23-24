#!/bin/bash


HOSTNAME="bob"

if [ "$1" == "-b" ]; then
    HOSTNAME="bobalina"
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
while true; do
    while true; do #ping until the hostname responds
        ping -c 1 $HOSTNAME.local > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo "$HOSTNAME.local is reachable, proceeding..."
            break
        else
            echo "Waiting for $HOSTNAME.local to respond..."
            sleep 0.1
        fi
    done

    ssh pi@$HOSTNAME.local 
done