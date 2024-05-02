#!/bin/bash

# Start a new tmux session
tmux new-session -d -s my_session

# Split the window vertically
tmux split-window -h -t my_session

# Split the window horizontally
tmux split-window -v -t my_session

# Run your scripts in each pane
tmux send-keys -t my_session:0.0 'ssh pi@192.168.1.1 "python3 -m http.server" ' Enter
tmux send-keys -t my_session:0.1 'echo lolol2' Enter
tmux send-keys -t my_session:0.2 'echo lolol3' Enter

# Attach to the session
tmux attach-session -t my_session
