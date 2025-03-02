#!/usr/bin/env bats

@test "Pipes" {
    run ./dsh <<EOF
ls | grep dshlib.c
EOF

    [ "$status" -eq 0 ]
    [[ "$output" == *"dshlib.c"* ]]
}

@test "Redirection: Output to file" {
    run ./dsh <<EOF
echo "hello, class" > out.txt
EOF

    run cat out.txt
    [ "$output" = "hello, class" ]
    [ "$status" -eq 0 ]
}

@test "Redirection: Append to file" {
    run ./dsh <<EOF
echo "this is line 2" >> out.txt
EOF

    run cat out.txt
    [ "$output" = $'hello, class\nthis is line 2' ]
    [ "$status" -eq 0 ]
}

@test "Redirection: Input from file" {
    echo "input test" > input.txt
    run ./dsh <<EOF
cat < input.txt
EOF

    [ "$output" = "input test" ]
    [ "$status" -eq 0 ]
}

@test "Dragon command" {
    run ./dsh <<EOF
dragon
exit
EOF

    [ "$status" -eq 0 ]
    [[ "$output" == *"@%%%%"* ]]
    [[ "$output" == *"%%%%%%@ "* ]]
}
