[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Vh67aNdh)
# Terminal Paintball

### Solo

Victor Kamrowski

### Project Description:

Terminal paintball is a copy of GamePigeon's paintball game. It is a turn-based two player game. Each player takes turn choosing a hiding spot (left, middle, right), and a shooting spot (left, middle, right: mirrored so if you shoot left, your opponent sees as if you shot right). Each player has three lives, and each hit shot reduces a life. First player to reduce their opponent to zero lives wins.

### Instructions:

The user will git clone the repository. Going to the terminal and using make compile will create a server and client. This will make the clientfile, which will be run through make play. The server will also do the same and can be run through make server if wished. After make play, they will wait for an opponent.

Once there is an opponent, the terminal will show and graphic of three hiding and shooting spots and text asking questions for the player to choose their turn choices. There should be two imputs here: first for hiding spot (L, M, R) and shooting spot (L, M, R). Once the user inputs, they will wait for their opponent to do the same.

The player can forfeit anytime during the game through CTRL-C. When a player has won, a screen will pop up with a prompt asking to continue game (yes/no) or to exit client (CTRL-C).
