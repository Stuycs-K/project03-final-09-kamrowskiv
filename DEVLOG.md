# Dev Log:

This document must be updated daily by EACH group member.

## Victor Kamrowski (GROUP ID: 11)


### 2025-01-06 - Created and started files, wrote md files + makefile + shared.h, made structs

In class:
- Wrote and finalized PROPOSAL.md (15 minutes)
- Wrote README.md (15 minutes)
- Created shared.h, server.c, client.c (5 minutes)
- Wrote makefile (5 minutes)

At home:
- Fixed makefile  (5 minutes)
- Created shared.h with position constants and PlayerState struct tracking position, lives, and score (if implemented) (10 minutes)
- Started main for client, added basic print statements and struct for stats (10 minutes)
- Started main for server, added basic print statements and placeholders for pipes. Made two structs for players (10 minutes)

### 2025-01-07 - Added pipes, added stdin, both verified

In class:
- Edited md files for new project path of having a lobby (10 minutes)
- Created new branch now that more code is being written (3 minutes)
- Started pipes, stdin, and fgets (20 minutes)
- Updated makefile (5 minutes)

At home:
- Finished stdin for custom names and pipes (5 minutes)
- Used fgets, planning on eventually deleting trailing \n (5 minutes)
- Each client creates their unique pipe to connect to server (25 minutes)
- Fixed pipe order (which caused errors) (15 minutes)
- Added some error handling (10 minutes)

### 2025-01-08 - Added lobby and functions, implemented into server, added client semaphores

In class:
- Added lobby struct (5 minutes)
- Created lobby in main and made methods to initialize lobby, add to lobby, and remove from lobby (35 minutes)

At home:
- Finished lobby methods (25 minutes)
- Implemented lobby into server (15 minutes)
- All clients shown in lobby and methods work (5 minutes)
- Added semaphores to smoothly do one player process at a time (20 minutes)

### 2025-01-09 - Added turn based functions, decided to switch pipes with sockets/networking

In class:
- Created function for processing moves (20 minutes)
- Created function for notifying players to play (20 minutes)

At home:
- Restarted base of project (30 minutes)
- Commented out pipes, replaced with sockets (75 minutes)

### 2025-01-10/11/12 - Added position and shooting choosing, added proper quitting, error catching, restarted lobby with better implement plan and setup

In class:
- Edited handle_game method in server to handle position choice, shooting choice, and errors such as invalid input (20 minutes)
- Made slight edits to clients (5 minutes)
- Tested game with 2 clients, worked, but game mechanics not as I want (5 minutes)
- Error input properly catched (5 minutes)

At home:
- Started lobby with lobby structs (10 minutes)
- Devised how to properly implement lobby, with all methods listed (15 minutes)
- Added broadcast lobby method which will show each time the lobby is edited (35 minutes)

### 2025-01-13 - Continued editing old lobby methods, started method to handle client and lobby (handle_client)

In class:
- Edited original lobby method to be used with new broadcast lobby method and structs (20 minutes)
- Edited remove from lobby method, included broadcast when making (10 minutes)
- Created basic base for handle client method (deals with disconnecting and inviting) (10 minutes)

At home:
- Added remove when client disconnects (20 minutes)
- Wrote first part of invite that invites based on inputted string (20 minutes)

### 2025-01-14 - Finished handle_client (which deals with lobby), started working on client code for lobby

In class:
- Added invite code that removes clients from lobby, plays the game, then adds them back to the lobby (30 minutes)
- Added else/error cases for invitation declined, player not found, and invalid command (10 minutes)

At home:
- Edited client by removing all old unnecessary methods (10 minutes)
- Planned how to add methods and structs to shared.h to make everything more efficient (20 minutes)

### 2025-01-15 - Finished client.c code for lobby in main, started shared memory for certain variables

In class:
- Added ask for name in client using stdin (10 minutes)
- Client main added code for when you are invited (15 minutes)
- Client main added code for when you invite someone (15 minutes)

At home:
- Started shared memory for lobby count variable and lobby array (75 minutes)

### 2025-01-16 - Scrapped lobby, made a 4 player game, fully works

In class:
- Continued working on lobby (40 minutes)

At home:
- Scrapped lobby, using old model pushed to Main (10 minutes)
- Added 4 player game (30 minutes)
- Added game ends when one player Remaining (15 minutes)
- Game ends when all players quit (15 minutes)

### 2025-01-17/18/19/20 - Extensive testing and added signals, recorded video, finalized readme

In class:
- Extensive testing, everything works so far (40 minutes)

At home:
- Added signal catcher for CTRL-C, everything caught properly (20 minutes)
- More extensive testing, everything works (30 minutes)
- Recorded video (15 minutes)
- Finalized READ.ME (15 minutes)

