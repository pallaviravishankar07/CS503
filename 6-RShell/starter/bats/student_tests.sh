#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Start server and check if it runs" {
    run ./dsh -s -i 0.0.0.0 -p 1234
    # Check if the server starts without errors
    [ "$status" -eq 0 ]
    # Check if the output contains "socket server mode"
    [[ "$output" == *"socket server mode"* ]]
}

@test "Client connects to server and executes 'exit'" {
    # Start the server in the background
    run ./dsh -s -i 0.0.0.0 -p 1234 &
    server_pid=$!

    # Give the server a moment to start
    sleep 1

    # Start the client and send the 'exit' command
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
exit
EOF

    # Check if the client exits successfully
    [ "$status" -eq 0 ]
    # Check if the output contains "cmd loop returned"
    [[ "$output" == *"cmd loop returned"* ]]

    # Kill the server after the test
    kill $server_pid
}

@test "Client connects to server and executes 'cd'" {
    # Start the server in the background
    run ./dsh -s -i 0.0.0.0 -p 1234 &
    server_pid=$!

    # Give the server a moment to start
    sleep 1

    # Start the client and send the 'cd' command
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
cd /tmp
pwd
EOF

    # Check if the client exits successfully
    [ "$status" -eq 0 ]
    # Check if the output contains the path to /tmp
    [[ "$output" == *"/tmp"* ]]

    # Kill the server after the test
    kill $server_pid
}

@test "Client connects to server and executes 'ls'" {
    # Start the server in the background
    run ./dsh -s -i 0.0.0.0 -p 1234 &
    server_pid=$!

    # Give the server a moment to start
    sleep 1

    # Start the client and send the 'ls' command
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
ls
EOF

    # Check if the client exits successfully
    [ "$status" -eq 0 ]
    # Check if the output contains expected files (modify as needed)
    [[ "$output" == *"dshlib.h"* ]] || [[ "$output" == *"dshlib.c"* ]]

    # Kill the server after the test
    kill $server_pid
}