[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Vh67aNdh)
# 4 Player Terminal Paintball

### Team Name: CS-V

 Group Members: Victor Kamrowski

### Project Description:

4 player Terminal Paintball takes 4 clients and concurrently connects them to a single game of paintball. Each player starts with 3 lives, and each player takes turns choosing whether to shoot a specific position (to lower another player's life by 1) or to switch their hiding position (0, 1, or 2). The last player standing wins (either by all opponents eliminated or forfeited). Additionally, multiple groups of players can play games at the same time. Used: working with files, processes (fork), signals, and sockets

### Instructions:

**Git Clone**:
```git clone git@github.com:Stuycs-K/project03-final-09-kamrowskiv.git```

**Compile (after every clean)**:
```make compile```

**Server**:
```make server```

**Client**:
```make play```

**Clean (after every server disconnect)**:
```make clean```

Each player will go in turns receiving this message:

"Enter command (POSITION <0|1|2> or SHOOT <player_id> <0|1|2>) (type 'quit' to exit): Enter your command:"

**Examples commands**:
```POSITION 0``` (player chooses position 0)
```SHOOT 4 1``` (player shoots player 4's position 1)
```quit``` (player quits the game. only allowed during turn.)
```CTRL-C``` (player disconnects from server. can be used anytime)

- Game continues until 3 players die/quit
- Server is open until CTRL-C closes all open clients

Make clean may have to be run multiple times in terminal. Make compile WILL always have to be run after make clean. 

If you get error:

bind failed: Address already in use

KEEP USING MAKE CLEAN



