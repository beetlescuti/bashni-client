###    ___           __        _ 
###   / _ )___ ____ / /  ___  (_)
###  / _  / _ `(_-</ _ \/ _ \/ / 
### /____/\_,_/___/_//_/_//_/_/  
                             

### Baschni Game Client

This clients connects to a dedicated game server and plays a game of Bashni. The Project was developed as part of the Systempraktikum in the winter semester of 20/21 at Ludwig-Maximilians-University Munich by Lukas Jost, Svea Meyer and Soren Little.

## Instructions

Connection to the game server requires a vpn connection (available only with valid LRZ account). The server is accesible over: http://sysprak.priv.lab.nm.ifi.lmu.de

After creating a game on the game server one can either set an environmental variable named GAME_ID to the game ID from the server or enter the id over the -g flag from the command line.

The environmental variable PLAYER is not mandatory (the first available player will be assigned automatically if you don't specify) and accepts a value of 1 or 2.

Compilation can be automated through command **make** while **make play** executes the program using the environmental variables specified. If using flags, execution using the format ./sysprak-client -g -p -c is necessary. 

    -g game ID
    -p player number
    -c specify a config file [not necessary]

After succesfull execution the command **make clean** will remove the object files from the directory.

