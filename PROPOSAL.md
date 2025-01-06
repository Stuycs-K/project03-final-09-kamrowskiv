# Final Project Proposal

## Group Members:

Victor Kamrowski

# Intentions:

Terminal Paintball

Turn-based two player game. Two players connect to a central server and take turns choosing a hiding position (left, middle, right) and targeting an opponent's position (same options). If a player successfully shoots their opponent's spot, the opponent loses one of their three lives. The first to reduce their opponent's lives to zero wins.

(This game is a copy of GamePigeon's Paintball game if you want to see the expected outcome.)

# Intended usage:

1. Server side: Server program listens for clients. Forks a subserver process for each client pair.

2. Client side: Players connect to server and state their choices via text through terminal. Each turn players choose position (left, middle, right) and where to shoot (left, middle, right). Server evaluates player input and sees if hits were successful. Game continues until one player loses all lives. Client displays game state and final results.

# Technical Details:

Class concepts used:
- Processes (forking) - server forks subservers to handle client connections
- Pipes(Named pipes) - communication between server and clients
- Shared memory - game state being shared between processes
- Semaphores - to handle how many users in server
- Signals - handle server shutdown and client disconnections
- Working with files - mulitple files to be used to handle clients and server

Breaking down project by:
Main server:
- Waits for client connection with named pipes
- Forks a subserver process for each client session

Subserver:
- Completes 3-way handshake with clients
- Handles player moves/turns, updates shared memory
- Send results to clients

Client:
- Connects to server through named pipes
- Sends position and shooting choices to server
- Recieves game updates and results from server

Algorithms/data structures
- Shared memory structs including player lives, positions, targets, turns, gameover variable, and winner
- Player turn algorithm (makes sure valid input and based on outcome will print certain statements)
- Proper turn based algorithm through pipes
- Game over algorithm (just checking if lives are equal to 0)
- Signal handling algorithm (if sigint, remove named pipes, detach shared memory, exit server. if sigpipe, deal with client disconnect.)

# Intended pacing:

(With subject to change if necessary)

- Jan 6: Create files, create readme + makefile + devlog, set up shared memory structs
- Jan 7: Implement named pipes, confirm message works inbetween server/client
- Jan 8: Implement player input (L, M, R), write turn order logic, start semaphores
- Jan 9: Implement turn logic (comparing positions, validating moves, update lives), add error handling, add signals
- Jan 10/11/12: Add game over logic, display winner at end, start testing game + shared memory + pipe communication, add prompts and messages
- Jan 13: Signal handling should be finalized, proper client and server exiting
- Jan 14: Test more with error catching
- Jan 15: Start finalizing the readme and clean anything in makefile
- Jan 16: Run more tests, start preparing for video demo
- Jan 17/18/19/20: Double check all documentation and md files, have general video script, record video and upload, final check that everything is done

(With multiple commits and devlog updates each day)
