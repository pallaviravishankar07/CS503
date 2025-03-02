1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_answer here_ My shell makes sure all child processes finish before taking new commands by keeping track of their PIDs and using waitpid() on each one. If I didn’t do this, leftover processes would pile up, and the shell might show the prompt too soon, even though commands are still running.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_answer here_ After using dup2() to redirect input/output, I close any pipe ends that aren’t needed. If I don’t, extra open pipes can waste system resources or cause commands to get stuck waiting for input that never comes.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_answer here_ 'cd' is built-in because it needs to change the shell’s own directory. If it were an external command, it would only change the directory in a separate process, so the shell itself wouldn’t actually move to the new location.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_answer here_ To allow unlimited piped commands, I would use dynamic memory (realloc()) instead of a fixed-size array. The trade-off is that managing memory this way is trickier and could slow things down if resizing happens too often.
