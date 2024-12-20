## Final Lab 13: Voting Machine

## overview
The voting system is a client-server application that will allow the user to cast votes for three canidates and ensures fairness by prventing duplicate votes from the same user. The server maintains the vote counts and voter list using shared memory, while the client provies an interface for users to vote.

## Features
- Three Candidates: User can vote Canidate 1,2,3 using A,B,C
- Duplicate Vote Prevention: Tracks voter uersernames to ensure fairness
- Shared Memory Communication: Uses shared memory to allow inter-process communication between client and server
- Signal Handling: Server gracefully handles shutdown with SIGINT, ensuring data cleanup.
- Vote Persistence: Server writes final results to a file (voting_results.txt) upon shutdown.
- Dynamic Memory: Employs shared memory for efficient data storage and communication.

## Files in Repository:
- client.c: The client program for submitting votes.
- server.c: The server program for managing the voting  system.
- Makefile: Automates the compilation of client and server programs.
- README.md: Documentation of the project.
- .gitignore: Specifies files to ignore in the repository (e.g., compiled binaries).
- voting_results.txt: Stores voting results after server shutdown.

## Compliation
- make: complies both client and server programs
- make clean: removes compiled binaries and intermediate files
## Usage
Starting the Server
Run the server to initialize the voting system: ./server
The server will:
 - Create shared memory for storing votes and voter data.
 - Continuously monitor for new votes.
Write results to voting_results.txt upon shutdown (Ctrl+C).

## Casting a Vote
Run the client to submit a vote: ./client -u <username> -c <candidate>
Parameters:
-u <username>: Specify the voter's username.
-c <candidate>: Specify the candidate to vote for (A, B, or C).

Example:
./client -u bob -c A
Output:
Vote for Candidate A submitted successfully. Thank you, bob!

## Server Shutdown
Press Ctrl+C to terminate the server. The server will:
    - Display final voting results.
    - Write results to voting_results.txt.
    - Clean up shared memory.

## Sample Results File
voting_results.txt (generated by the server):
(A): 5
(B): 3
(C): 2

## Dependencies
- GCC compiler
- POSIX shared memory libraries (<sys/mman.h>)
- POSIX signals (<signal.h>)

## Error Handling
- Invalid Candidate: The client program validates candidate input and prompts the user to choose a valid option (A, B, or C).
- Duplicate Votes: The server checks if a username has already voted and prevents multiple votes.
- Shared Memory Issues: Both client and server handle errors related to shared memory access and provide descriptive error messages.
