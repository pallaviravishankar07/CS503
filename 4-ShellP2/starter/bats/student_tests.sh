#!/usr/bin/env bats

# Test for the exit built-in command
@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    # Check that the shell exits with the correct status
    [ "$status" -eq 0 ]
}

# Test for a non-existent command
@test "Non-existent command" {
    run ./dsh <<EOF
nonexistentcommand
rc
EOF
    # Check that the output contains the error message
    [[ "$output" =~ "Command not found in PATH" ]]
    # Check that the return code is 127 (standard for command not found)
    [[ "$output" =~ "127" ]]
}

# Test for permission denied
@test "Permission denied" {
    run ./dsh <<EOF
touch testfile
chmod 000 testfile
./testfile
rc
rm -f testfile
EOF
    # Check that the output contains the permission denied message
    [[ "$output" =~ "Permission denied" ]]
    # Check that the return code is 126 (standard for permission denied)
    [[ "$output" =~ "126" ]]
}

# Test for the rc built-in command
@test "Return code command" {
    run ./dsh <<EOF
false
rc
EOF
    # Check that the return code of the last command is reported correctly
    [[ "$output" =~ "1" ]]
}
