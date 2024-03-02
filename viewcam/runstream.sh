#!/bin/bash

#kill what is using my port

kill -9 $(lsof -ti:1234)

# Define a function to be called when SIGINT is received
function handle_sigint() {
    echo "Ctrl+C pressed, exiting gracefully..."
    exit 0
}

# Use trap to catch SIGINT and call handle_sigint when it's received
trap handle_sigint SIGINT

# Start the infinite loop
while true; do
   poetry run python3 view_flask.py
done


