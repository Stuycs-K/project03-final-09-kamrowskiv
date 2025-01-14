# Dev Log:

This document must be updated daily by EACH group member.

## Victor Kamrowski

### 2025-01-06 - Created and started files, wrote md files + makefile + shared.h, made structs

In class:
- Wrote and finalized PROPOSAL.md
- Wrote README.md
- Created shared.h, server.c, client.c
- Wrote makefile

At home:
- Fixed makefile
- Created shared.h with position constants and PlayerState struct tracking position, lives, and score (if implemented)
- Started main for client, added basic print statements and struct for stats
- Started main for server, added basic print statements and placeholders for pipes. Made two structs for players

### 2025-01-07 - Added pipes, added stdin, both verified

In class:
- Edited md files for new project path of having a lobby
- Created new branch now that more code is being written
- Started pipes, stdin, and fgets
- Updated makefile

At home:
- Finished stdin for custom names and pipes
- Used fgets, planning on eventually deleting trailing \n
- Each client creates their unique pipe to connect to server
- Fixed pipe order (which caused errors)
- Added some error handling

### 2025-01-08 - Added lobby and functions, implemented into server, added client semaphores

In class:
- Added lobby struct
- Created lobby in main and made methods to initialize lobby, add to lobby, and remove from lobby

At home:
- Finished lobby methods
- Implemented lobby into server
- All clients shown in lobby and methods work
- Added semaphores to smoothly do one player process at a time

### 2025-01-09 - Added turn based functions, decided to switch pipes with sockets/networking

In class:
- Created function for processing moves
- Created function for notifying players to play

At home:
- Restarted base of project
- Commented out pipes, replaced with sockets

### 2025-01-10/11/12 - Added position and shooting choosing, added proper quitting, error catching, restarted lobby with better implement plan and setup

In class:
- Edited handle_game method in server to handle position choice, shooting choice, and errors such as invalid input
- Made slight edits to clients
- Tested game with 2 clients, worked, but game mechanics not as I want
- Error input properly catched

At home:
- Started lobby with lobby structs
- Devised how to properly implement lobby, with all methods listed
- Added broadcast lobby method which will show each time the lobby is edited

### 2025-01-13 - Continued editing old lobby methods

In class:
- Edited original lobby method to be used with new broadcast lobby method and structs
- Edited remove from lobby method, included broadcast when making
- Created basic base for handle client method (deals with disconnecting and inviting)

At home:
- Added remove when client disconnects
- Wrote first part of invite that invites based on inputted string