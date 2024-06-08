#!/bin/bash
cd /Users/aayanmaheshwari/Desktop/MATE/rang23-24/viewcam

# sleep 2
HOSTNAME="bob.local"

if [ "$1" == "-b" ]; then
    HOSTNAME="192.168.1.102"
    PYTHON_ARG="-b"
else
    PYTHON_ARG=""
fi

# Define a function to be called when SIGINT is received
function handle_sigint() {
    echo "Ctrl+C pressed, exiting gracefublly..."
    
    exit 0
}

# Use trap to catch SIGINT and call handle_sigint when it's received
trap handle_sigint SIGINT


# Wait for the specified hostname to respond to ping
while true; do
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

    # Run stream via ssh
    echo "ping successful, running stream..."
    
    # ssh -t pi@$HOSTNAME "sudo bash /home/pi/rang23-24/piScripts/picam.sh" & echo "stream started"
    # sleep 2

    echo "Starting the stream..."
    poetry run python3 view-errorTolerant.py $PYTHON_ARG
    # sleep 5
done