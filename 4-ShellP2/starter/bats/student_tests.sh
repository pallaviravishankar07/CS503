#!/usr/bin/env bats

# Basic command execution tests
@test "Basic external command execution" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Command with arguments" {
    run ./dsh <<EOF
echo "hello world"
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "hello world" ]]
}

# Built-in command tests
@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    # OK_EXIT is -7, which in shell becomes 249 (256 + (-7))
    [ "$status" -eq 249 ]
}

@test "Dragon command" {
    run ./dsh <<EOF
dragon
EOF
    [ "$status" -eq 0 ]
    # Check for any non-empty output 
    [ -n "$output" ]
}

# Directory manipulation tests
@test "Change directory command" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "/tmp" ]]
}

@test "CD with no arguments" {
    local original_dir=$(pwd)
    run ./dsh <<EOF
cd
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "CD with invalid directory" {
    run ./dsh <<EOF
cd /nonexistent/directory
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "No such file or directory" ]]
}

# Input handling tests
@test "Empty command handling" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "Command with quoted arguments" {
    run ./dsh <<EOF
echo "  hello    world  "
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "  hello    world  " ]]
}

@test "Multiple spaces between arguments" {
    run ./dsh <<EOF
echo hello      world
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "hello world" ]]
}

# Error handling tests
@test "Invalid command handling" {
    run ./dsh <<EOF
nonexistentcommand
EOF
    [ "$status" -eq 0 ]  # Shell continues even after command fails
    [[ "${output}" =~ "not found" ]] || [[ "${output}" =~ "No such file" ]]
}

# Multiple command tests
@test "Multiple commands in sequence" {
    run ./dsh <<EOF
echo "test"
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "test" ]]
    [[ "${output}" =~ "/tmp" ]]
}

@test "Command with multiple arguments" {
    run ./dsh <<EOF
echo first second third
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "first second third" ]]
}

# Edge cases
@test "Command with only spaces" {
    run ./dsh <<EOF
     
EOF
    [ "$status" -eq 0 ]
}

@test "Command with mixed quotes" {
    run ./dsh <<EOF
echo "quoted text" unquoted "more quoted"
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "quoted text unquoted more quoted" ]]
}

@test "Long command handling" {
    run ./dsh <<EOF
echo $(printf 'a%.0s' {1..100})
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "a" ]]
}
