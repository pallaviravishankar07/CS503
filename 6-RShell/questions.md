1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_answer here_ The client knows a command's output is fully received by looking for a special marker, a set message length, or an EOF signal. Since data might come in chunks, the client keeps reading until it gets everything, making sure nothing is missing.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_answer here_ Since TCP is like a continuous stream of data, a shell protocol needs a way to mark where a command starts and ends. This can be done by adding message length at the beginning, using special symbols, or sending an EOF signal. Without this, messages might get mixed up or cut off, causing errors.

3. Describe the general differences between stateful and stateless protocols.

_answer here_ Stateful protocols remember past interactions, like a conversation where each reply depends on what was said before (e.g., a logged-in session). Stateless protocols handle each request separately, like asking for directions without expecting the person to remember you later (e.g., a simple web request).

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_answer here_ We use UDP when speed matters more than reliability, like in online games, voice calls, or live video. It skips extra checks, making communication faster, but sometimes messages might get lost or arrive out of order.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_answer here_ The operating system provides sockets, which let programs talk over the network. Apps use them to send and receive data, just like making a phone call—dialing (connect), speaking (send), and listening (receive).