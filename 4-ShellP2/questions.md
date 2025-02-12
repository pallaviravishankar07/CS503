1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork() creates a child process, allowing the shell to keep running after executing a command. If we used only execvp(), the shell would be replaced by the command and terminate after running it. fork() lets the shell stay alive to handle more commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1 and sets 'errno' to indicate the error. We handle this by printing an error (e.g., "fork failed") and letting the shell continue so the user can try other commands. Common reasons for failure include hitting system process limits or running out of memory.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() uses the PATH environment variable to locate commands. It searches through directories listed in PATH (separated by colons) to find the executable file. For example, when you type ls, it searches /bin, /usr/bin, etc., until it finds /bin/ls.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() does two things:
    Prevents "zombie processes" by cleaning up finished child processes.
    Makes the shell wait for the current command to finish before prompting for the next one. Without wait(), the shell would move on immediately, and child processes would linger as zombies.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit code from the status returned by wait(). It filters out everything except the 8-bit exit code, so we can check if the command succeeded or failed.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  The build_cmd_buff() function uses an in_quotes flag to preserve spaces inside quotes. For example, echo "hello world" is treated as a single argument, not split into echo, hello, and world. This ensures commands with spaces in arguments work correctly.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Adding fork()/execvp() to run external commands.
    Improving argument parsing to handle quotes properly.
    Adding memory management with alloc()/free() functions.
The biggest challenge was adapting the parsing logic to work with the new cmd_buff structure while managing memory correctly.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are lightweight notifications for handling events like process termination or user interrupts. They’re simpler than other IPC methods but don’t carry data.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (9): Forcefully kills a process; cannot be ignored.
    SIGTERM (15): Requests graceful termination; can be caught for cleanup.
    SIGINT (2): Sent by Ctrl+C; can be caught for cleanup.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  Unlike SIGINT, SIGSTOP cannot be caught or ignored. It immediately pauses the process, enforced by the kernel, ensuring the system always has control.
