# LostAtSea

Language: C

Simulator: Boycott Advance

/**
 * GAME DETAILS
 * Title - Lost at Sea
 * Maker - Alex Freeney (with TA support)
 * Gameplay
 	- Scuba diver is swimming 'right' toward the cruise ship 
 	- Diver has to avoid sharks in water and stay underwater while pirate ship is on the screen
 	- *HINT*: best effect if right button is held down during swimming
 	- *HINT*: scuba diver is buoyant, so it will automatically float up
 * Bugs
 	- I don't really mind this bug, but all sprites appear before you go to win/lose. 
	- Sometimes collisions of scuba/shark seem inaccurate
 	* 
 * Extra Credit - mosaic effect on loss, flashing on win, and cool art!! 
* GENERAL
  * This game is actually fun!! Once I added shark row animations, it got cool
* IMPORTANT DISCLAIMER
  * One of my simulators is able to make the cheat (hiding the sprites) work
  * The other simulator can make the mosaic effect work
  * No simulator that I have can make both of them work at the same time, but I've seen both work separately. 
* IMPLEMENTED
  * 5 sprites
  * 2 backgrounds (ocean and gamebg) that utilize one palette
  * Splash Screen with menu and instructions
  * Ability to pause game
  * A way to get to one end state
  	* LOSE1: Be seen on the surface while there's a pirate ship there
  	* LOSE2: Be eaten by a shark by colliding with it
  	* WIN: get to cruise ship without lose1 or lose2
  * Cheat: hold A to make the sharks go away
*/

Music: Knee Deep by Zac Brown Band

Makefile and myLib.h were created by TAs with the intent of being manipulated by students

----- main.c contains most of my original methods and creative decisions!

If you are like most people and don't have a Gameboy simulator on your perosnal computer, go to this link to watch me play the game! https://www.youtube.com/watch?v=bgD0y59XGSQ
