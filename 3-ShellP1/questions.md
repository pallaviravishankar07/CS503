1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it safely reads input from the user, ensuring we don’t accidentally read too much data and cause a buffer overflow. It also handles newlines correctly, making it perfect for reading commands line by line.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() lets us allocate memory dynamically, which is more flexible. A fixed-size array might waste memory if it’s too big or cause crashes if it’s too small. With malloc(), we can adjust the memory size based on what’s needed, making the program more efficient and reliable.

3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces is important because extra spaces can mess up how commands are interpreted. For example, a command like " ls " might not work correctly if the spaces aren’t removed. Trimming ensures the shell processes commands cleanly and avoids unnecessary errors.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Example 1: command > "file" (saving output to a file).
    Challenge: We need to make sure we don’t accidentally overwrite an important file.

    Example 2: command 2> "error.log" (saving errors to a file).
    Challenge: We have to handle errors separately so they don’t get mixed up with regular output.

    Example 3: command < "input.txt" (reading input from a file).
    Challenge: We need to check if the file exists and is readable before using it.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is about sending input/output to or from files, like saving output to a file or reading input from one. Piping, on the other hand, connects the output of one command directly to the input of another, letting us join the commands together.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR (for errors) and STDOUT (for regular output) separate is important because it helps us easily identify and debug issues. If they were mixed, it would be harder to spot errors in the middle of normal output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  When a command fails, the shell should capture both its regular output (STDOUT) and error messages (STDERR). If we want to combine them, we can use something like 2>&1 to merge STDERR into STDOUT. However, it’s often better to keep them separate so users can clearly see what went wrong without sorting through all the output.
