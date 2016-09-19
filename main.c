//Files utilized in gameplay
#include "myLib.h"
#include "splash.h"
#include "instruction.h"
#include "win.h"
#include "lose1.h"
#include "lose2.h"
#include "gamebg.h"
#include "pause.h"
#include "skybg.h"
#include "pikachu.h"
#include "ocean.h"
#include "story1.h"
#include "story2.h"
#include "story3.h"
#include "pregame.h"
#include "keys.h"
#include "KneeDeep.h"
#include "water.h"

/**
 * GAME DETAILS
 * Title - Lost at Sea
 * Maker - Alex Freeney
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

//Structure Variables
unsigned int buttons;
unsigned int oldButtons;

//GamePlay Variables
int hOff;
int h1Off;
int vOff;
int journeyCount;
int state;
int loseTimer;
short num = 0;
int loseValue = 0;

OBJ_ATTR shadowOAM[128];

//int collMapSize = 256;

#define ROWMASK 0xFF
#define COLMASK 0x1FF

//Sprite Structure Definition
//Defines variables needed to manipulate a sprite
typedef struct  
{
	int row;
	int col;
	int bigRow;
	int bigCol;
	int rdel;
	int cdel;
	int width;
	int height;
	int aniCounter;
	int aniState;
	int prevAniState;
	int frameDirection;
    int currFrame;
    int hide;
    int isActive;
} SPRITE;

//Types of sprites used in gameplay
SPRITE pikachu;
SPRITE ship;
SPRITE pirate;
SPRITE shark;
SPRITE shark2;
SPRITE shark3;

//Game Methods
void initializeGame();
void initializePikachu();
void initializeShip();
void initializePirate();
void initializeShark();
void initializeShark2();
void initializeShark3();
void checkCollisions();
void hidePirate();
void hideShip();
void hidePikachu();
void goToLose1State();
void goToLose2State();
void goToWinState();
//void initSplash();

//Switch Methods
void splash();
void instruction();
void game();
void pause();
void win();
void dying();
void lose();
void story1();
void story2();
void story3();
void keys();
void pregame();
void winning();

//Sprite Methods
void hideSprites();
void hideSharks();
void animate();
void updateOAM();

//Enums for states and sprite animation
enum {SPLASHSCREEN, INSTRUCTIONSCREEN, STORY1SCREEN, STORY2SCREEN, STORY3SCREEN, KEYSSCREEN, PREGAMESCREEN, GAMESCREEN, PAUSESCREEN, WINSCREEN, LOSESCREEN, DYINGSTATE, WINNINGSTATE};
enum { PIKAFRONT, PIKABACK, PIKARIGHT, PIKALEFT, PIKAIDLE};

//Main Method
int main()
{
	//Mode 0
	//Backgrounds Enabled
	//Sprites Enabled
	//ScreenBlockBase and CharBlockBase
	//Background Sizes & Colors
	REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE;
	REG_BG0CNT = CBB(0) | SBB(28) | BG_SIZE0;
	REG_BG1CNT = CBB(1) | SBB(29) | BG_SIZE0;

	setupSounds();
	setupInterrupts();

	playSoundA(KneeDeep,KNEEDEEPLEN,KNEEDEEPFREQ);
	soundA.loops = 1;

	//Initialize background palette
	//DMA tiles and tile map
	//4bpp tiles with 'sbb' checked

	//Load Splash Palette
	loadPalette(splashPal);
	DMANow(3, splashTiles, &CHARBLOCKBASE[0], splashTilesLen/2);
	DMANow(3, splashMap, &SCREENBLOCKBASE[28], splashMapLen/2);
	
	//Sprites
	DMANow(3, pikachuTiles, &CHARBLOCKBASE[4], pikachuTilesLen/2);
	DMANow(3, pikachuPal, SPRITE_PALETTE, pikachuPalLen/2);

	// Handle Cornerface and Initialize Game
	hideSprites();
	
	//initialize();
	//initSplash();
	while(1)
	{
		oldButtons = buttons;
		buttons = BUTTONS;

		switch(state) {
			//Start
			case SPLASHSCREEN:
				splash();
			break;

			//Start
			case INSTRUCTIONSCREEN:
				instruction();
			break;

			//Storyline 1
			case STORY1SCREEN:
				story1();
			break;

			//Storyline 2
			case STORY2SCREEN:
				story2();
			break;

			//Storyline 3
			case STORY3SCREEN:
				story3();
			break;

			//Storyline 3
			case KEYSSCREEN:
				keys();
			break;

			//Pregame
			case PREGAMESCREEN:
				pregame();
			break;

			//Game
			case GAMESCREEN:
				game();
			break;

			//Pause
			case PAUSESCREEN:
				pause();
			break;

			//Win
			case WINSCREEN:
				win();
			break;

			//DYING
			case DYINGSTATE:
				dying();
			break;

			//DYING
			case WINNINGSTATE:
				winning();
			break;

			//Lose
			case LOSESCREEN:
				lose();
			break;

		}
		//flipPage();
		waitForVblank();
	}
}

/* SPLASHSCREEN */
void splash(){
	//On enter, switch to instruction screen
    if(BUTTON_PRESSED(BUTTON_START))
    {
    	//playSoundA(KneeDeep,KNEEDEEPLEN,KNEEDEEPFREQ);
    	loadPalette(story1Pal);
		DMANow(3, story1Tiles, &CHARBLOCKBASE[0], story1TilesLen/2);
		DMANow(3, story1Map, &SCREENBLOCKBASE[28], story1MapLen/2);
        state = STORY1SCREEN;
    }
}

/* STORY1SCREEN */
void story1() {
    if(BUTTON_PRESSED(BUTTON_START))
    {
    	loadPalette(story2Pal);
		DMANow(3, story2Tiles, &CHARBLOCKBASE[0], story2TilesLen/2);
		DMANow(3, story2Map, &SCREENBLOCKBASE[28], story2MapLen/2);
        state = STORY2SCREEN;
    }
}

/* STORY2SCREEN */
void story2() {
    if(BUTTON_PRESSED(BUTTON_START))
    {
    	loadPalette(story3Pal);
		DMANow(3, story3Tiles, &CHARBLOCKBASE[0], story3TilesLen/2);
		DMANow(3, story3Map, &SCREENBLOCKBASE[28], story3MapLen/2);
        state = STORY3SCREEN;
    }
}

/* STORY3SCREEN */
void story3() {
	if(BUTTON_PRESSED(BUTTON_START))
    {
    	loadPalette(keysPal);
		DMANow(3, keysTiles, &CHARBLOCKBASE[0], keysTilesLen/2);
		DMANow(3, keysMap, &SCREENBLOCKBASE[28], keysMapLen/2);
        state = KEYSSCREEN;
    }
}

/* STORY3SCREEN */
void keys() {
    if(BUTTON_PRESSED(BUTTON_START))
    {
    	loadPalette(instructionPal);
		DMANow(3, instructionTiles, &CHARBLOCKBASE[0], instructionTilesLen/2);
		DMANow(3, instructionMap, &SCREENBLOCKBASE[28], instructionMapLen/2);
        state = INSTRUCTIONSCREEN;
    }
}

/* INSTRUCTIONSCREEN */
void instruction(){
	//On enter, switch to game screen
    if(BUTTON_PRESSED(BUTTON_START))
    {
    	loadPalette(pregamePal);
		DMANow(3, pregameTiles, &CHARBLOCKBASE[0], pregameTilesLen/2);
		DMANow(3, pregameMap, &SCREENBLOCKBASE[28], pregameMapLen/2);
        state = PREGAMESCREEN;
    }
}

/* INSTRUCTIONSCREEN */
void pregame(){
	//On enter, switch to game screen
	if(BUTTON_PRESSED(BUTTON_START))
	{
		loadPalette(oceanPal);
		//loadPalette(oceanPal);
		//DMANow(3, gamebgTiles, &CHARBLOCKBASE[0], gamebgTilesLen/2);
		//DMANow(3, gamebgMap, &SCREENBLOCKBASE[28], gamebgMapLen/2);
		DMANow(3, oceanTiles, &CHARBLOCKBASE[0], oceanTilesLen/2);
		DMANow(3, oceanMap, &SCREENBLOCKBASE[28], oceanMapLen/2);
		DMANow(3, gamebgTiles, &CHARBLOCKBASE[1], gamebgTilesLen/2);
		DMANow(3, gamebgMap, &SCREENBLOCKBASE[29], gamebgMapLen/2);
		initializeGame();
        state = GAMESCREEN;
    }
}

/* GAMESCREEN */
void game() {
	
	REG_BG0HOFS = hOff*2;
	REG_BG1HOFS = h1Off/2;
	//REG_BG1HOFS = hOff/2;

	//Updates pikachu's world position based on its row/col movement
	pikachu.bigRow = pikachu.row + vOff;
	pikachu.bigCol = pikachu.col + hOff;

	ship.bigRow = ship.row + vOff;
	ship.bigCol = ship.col + hOff;

	animate();
	updateOAM();

	if (pikachu.col < 70) {
		hideSharks();
	} else {
		shark.isActive = 1;
		shark2.isActive = 1;
		shark3.isActive = 1;
	}

	// playSoundA(KneeDeep,KNEEDEEPLEN,KNEEDEEPFREQ);
	// soundA.loops = 1;

	// if (journeyCount == 50) {
	// 	initializePirate;
	// }

	if (pikachu.row < 80 && pirate.hide == 0) {
		playSoundB(water,WATERLEN,WATERFREQ);
		soundB.loops = 0;
		journeyCount = 0;
		hOff = 0;
		h1Off = 0;
		REG_BG0HOFS = hOff;
		pikachu.isActive = 0;
		hideSprites();
		loseValue = 1;
		state = DYINGSTATE;
		//dying();
		// loadPalette(lose1Pal);
		// DMANow(3, lose1Tiles, &CHARBLOCKBASE[0], lose1TilesLen/2);
		// DMANow(3, lose1Map, &SCREENBLOCKBASE[28], lose1MapLen/2);
		// state = LOSESCREEN;
		//goToLose1State();
	}
	if (pikachu.col >= ship.col && pikachu.row < 80 && ship.hide == 0) {
		playSoundB(water,WATERLEN,WATERFREQ);
		soundB.loops = 0;
		journeyCount = 0;
		//pikachu.col = 20;
		hOff = 0;
		h1Off = 0;
		REG_BG0HOFS = hOff;
		hideSprites();
		// loadPalette(winPal);
		// DMANow(3, winTiles, &CHARBLOCKBASE[0], winTilesLen/2);
		// DMANow(3, winMap, &SCREENBLOCKBASE[28], winMapLen/2);
		// state = WINSCREEN;
		state = WINNINGSTATE;
		//goToWinState();
	}

	//On enter, switch to pause screen
	if(BUTTON_PRESSED(BUTTON_START)) {
		// hideSprites();
		// DMANow(3, shadowOAM, OAM, 512);
		REG_BG0HOFS = 0;
		loadPalette(pausePal);
		DMANow(3, pauseTiles, &CHARBLOCKBASE[0], pauseTilesLen/2);
		DMANow(3, pauseMap, &SCREENBLOCKBASE[28], pauseMapLen/2);
		hideSprites();
        state = PAUSESCREEN;
    }
    if(BUTTON_HELD(BUTTON_A))
	{
		hideSharks();
	}

	hidePirate();
	hideShip();
	DMANow(3, shadowOAM, OAM, 512);
	waitForVblank();

	checkCollisions();

}

/* PAUSESCREEN */
void pause() {
	//On enter, switch to game screen
	if(BUTTON_PRESSED(BUTTON_START)) {
		loadPalette(oceanPal);
		DMANow(3, oceanTiles, &CHARBLOCKBASE[0], oceanTilesLen/2);
		DMANow(3, oceanMap, &SCREENBLOCKBASE[28], oceanMapLen/2);
		DMANow(3, gamebgTiles, &CHARBLOCKBASE[1], gamebgTilesLen/2);
		DMANow(3, gamebgMap, &SCREENBLOCKBASE[29], gamebgMapLen/2);
        state = GAMESCREEN;
    }

	//On delete, switch to splash screen
	if(BUTTON_PRESSED(BUTTON_SELECT)) {
        //initSplash();
        loadPalette(splashPal);
		DMANow(3, splashTiles, &CHARBLOCKBASE[0], splashTilesLen/2);
		DMANow(3, splashMap, &SCREENBLOCKBASE[28], splashMapLen/2);
        state = SPLASHSCREEN;
    }
}

/* WINSCREEN */
void win() {
	if(BUTTON_PRESSED(BUTTON_START)) {
		//initSplash();
		hideSprites();
		loadPalette(splashPal);
		DMANow(3, splashTiles, &CHARBLOCKBASE[0], splashTilesLen/2);
		DMANow(3, splashMap, &SCREENBLOCKBASE[28], splashMapLen/2);
		state = SPLASHSCREEN;
    }
}

void dying() {
	ship.hide = 1;
	while (loseTimer < 50) {
		num++;
		num %= 15;
		loseTimer++;
		updateOAM();
		DMANow(3, shadowOAM, OAM, 512);
		waitForVblank();
	} 
	loseTimer = 0;
	num = 0;
	hideSprites();
	if (loseValue == 1) {
		goToLose1State();
	} 
	if (loseValue == 2) {
		goToLose2State();
	}
}

void winning()
{
	int winCounter = 0;
	while (winCounter < 80) 
    {
		if (winCounter < 20)  {
			pikachu.hide = 1;
		}
		else if (winCounter > 20 && winCounter < 40) {
			pikachu.hide = 0;
		}
		else if (winCounter > 40 && winCounter < 60) {
			pikachu.hide = 1;
		}
		else {
			pikachu.hide = 0;
		}
		updateOAM();
		hidePikachu();
		//DMANow(3, shadowOAM, OAM, 512);
		waitForVblank();
		winCounter++;
	}
	hideSprites();
	goToWinState();
}

/* LOSESCREEN */
void lose() {
	if(BUTTON_PRESSED(BUTTON_START)) {
        //initSplash();
    	loadPalette(keysPal);
		DMANow(3, keysTiles, &CHARBLOCKBASE[0], keysTilesLen/2);
		DMANow(3, keysMap, &SCREENBLOCKBASE[28], keysMapLen/2);
        state = KEYSSCREEN;
    }
}

void initializeGame()
{
	initializeShip();
	initializePikachu();
	initializePirate();
	initializeShark();
	initializeShark2();
	initializeShark3();

	hOff = 0;
	h1Off = 0;
	vOff = 0;
	journeyCount = 0;
	loseTimer = 0;
}

/* INITIALIZE PIKACHU */
void initializePikachu()
{
	pikachu.width = 32;
	pikachu.height = 16;
	pikachu.rdel = 1;
	pikachu.cdel = 1;
	pikachu.row = 120;
	pikachu.col = 5;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	pikachu.aniCounter = 1;
	pikachu.currFrame = 0;
	pikachu.aniState = PIKABACK;
	pikachu.hide = 0;
	pikachu.isActive = 1;
}

/* INITIALIZE SHARK 1 */
void initializeShark()
{
	shark.width = 32;
	shark.height = 16;
	shark.rdel = 1;
	shark.cdel = 1;
	shark.row = 90;
	shark.col = 420;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	shark.aniCounter = 1;
	shark.currFrame = 0;
	shark.aniState = PIKABACK;
	shark.isActive = 1;
	shark.hide = 0;
}

/* INITIALIZE SHARK 2 */
void initializeShark2()
{
	shark2.width = 32;
	shark2.height = 16;
	shark2.rdel = 1;
	shark2.cdel = 1;
	shark2.row = 140;
	shark2.col = 350;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	shark2.aniCounter = 1;
	shark2.currFrame = 0;
	shark2.aniState = PIKABACK;
	shark2.isActive = 1;
	shark2.hide = 0;
}

/* INITIALIZE SHARK 3 */
void initializeShark3()
{
	shark3.width = 32;
	shark3.height = 16;
	shark3.rdel = 1;
	shark3.cdel = 1;
	shark3.row = 110;
	shark3.col = 240;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	shark3.aniCounter = 1;
	shark3.currFrame = 0;
	shark3.aniState = PIKABACK;
	shark3.isActive = 1;
	shark3.hide = 0;
}

/* INITIALIZE SHIP */
void initializeShip()
{
	ship.width = 32;
	ship.height = 16;
	ship.rdel = 1;
	ship.cdel = 1;
	ship.row = 45;
	ship.col = 170;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	ship.aniCounter = 0;
	ship.currFrame = 0;
	ship.aniState = PIKAFRONT;
	ship.hide = 1;
}

/* INITIALIZE SHIP */
void initializePirate()
{
	pirate.width = 16;
	pirate.height = 16;
	pirate.rdel = 1;
	pirate.cdel = 1;
	pirate.row = 55;
	pirate.col = 100;
	//pikachu.row = 180/2-pikachu.width/2;
	//pikachu.col = 240/2-pikachu.height/2;
	pirate.aniCounter = 0;
	pirate.currFrame = 0;
	pirate.aniState = PIKAFRONT;
	pirate.hide = 1;

}

void goToLose1State()
{
		loadPalette(lose1Pal);
		DMANow(3, lose1Tiles, &CHARBLOCKBASE[0], lose1TilesLen/2);
		DMANow(3, lose1Map, &SCREENBLOCKBASE[28], lose1MapLen/2);
		state = LOSESCREEN;
}

void goToLose2State()
{
	loadPalette(lose2Pal);
	DMANow(3, lose2Tiles, &CHARBLOCKBASE[0], lose2TilesLen/2);
	DMANow(3, lose2Map, &SCREENBLOCKBASE[28], lose2MapLen/2);
	state = LOSESCREEN;
}

void goToWinState()
{
	loadPalette(winPal);
	DMANow(3, winTiles, &CHARBLOCKBASE[0], winTilesLen/2);
	DMANow(3, winMap, &SCREENBLOCKBASE[28], winMapLen/2);
	state = WINSCREEN;
}

// /* HIDESPRITES */
void hideSprites()
{
    int i;for(i = 0; i < 128; i++)
    {
         shadowOAM[i].attr0 = ATTR0_HIDE;
    }
    DMANow(3, shadowOAM, OAM, 512);
}

// /* HIDESPRITES */
void hidePirate()
{
    if (pirate.hide == 1) {
    	shadowOAM[2].attr0 = ATTR0_HIDE;
    }
    DMANow(3, shadowOAM, OAM, 512);
}

// /* HIDESPRITES */
void hideShip()
{
    if (ship.hide == 1) {
    	shadowOAM[1].attr0 = ATTR0_HIDE;
    }
    DMANow(3, shadowOAM, OAM, 512);
}

// /* HIDESPRITES */
void hidePikachu()
{
    if (pikachu.hide == 1) {
    	shadowOAM[0].attr0 = ATTR0_HIDE;
    }
    DMANow(3, shadowOAM, OAM, 512);
}

// /* HIDESPRITES */
void hideSharks()
{
    int i;for(i = 3; i < 6; i++)
    {
         shadowOAM[i].attr0 = ATTR0_HIDE;
    }
    DMANow(3, shadowOAM, OAM, 512);
    shark.isActive = 0;
    shark2.isActive = 0;
    shark3.isActive = 0;
}

/* UPDATE OAM */
void updateOAM()
{
	//PIKACHU
	shadowOAM[0].attr0 = (ROWMASK & pikachu.row) | ATTR0_4BPP | ATTR0_WIDE | ATTR0_MOSAIC;
	shadowOAM[0].attr1 = (COLMASK & pikachu.col) | ATTR1_SIZE32;
	//shadowOAM[0].attr2 = SPRITEOFFSET16(0,0); //(2*pikachu.currFrame,2*pikachu.prevAniState);
	// num++;
	// num %= 15;
	REG_MOSAIC = OBJ_STRETCH_H(num) | OBJ_STRETCH_V(num);

	//CRUISE SHIP
	shadowOAM[1].attr0 = (ROWMASK & ship.row);// | ATTR0_4BPP | ATTR0_SQUARE;
	shadowOAM[1].attr1 = (COLMASK & ship.col) | ATTR1_SIZE64;
	shadowOAM[1].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(8*ship.currFrame,8))); //2*ship.prevAniState);

	//PIRATE SHIP
	shadowOAM[2].attr0 = (ROWMASK & pirate.row); // | ATTR0_4BPP | ATTR0_SQUARE
	shadowOAM[2].attr1 = (COLMASK & pirate.col) | ATTR1_SIZE32;
	shadowOAM[2].attr2 = (SPRITEOFFSET16(pirate.currFrame * 4, 4));	//pirate.currFrame * 4

	//SHARK 1
	shadowOAM[3].attr0 = (ROWMASK & shark.row) | ATTR0_4BPP | ATTR0_WIDE;
	shadowOAM[3].attr1 = (COLMASK & shark.col) | ATTR1_SIZE32;
	shadowOAM[3].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(0,16)));

	//SHARK 2
	shadowOAM[4].attr0 = (ROWMASK & shark2.row) | ATTR0_4BPP | ATTR0_WIDE;
	shadowOAM[4].attr1 = (COLMASK & shark2.col) | ATTR1_SIZE32;
	shadowOAM[4].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(0,16)));

	//SHARK 3
	shadowOAM[5].attr0 = (ROWMASK & shark3.row) | ATTR0_4BPP | ATTR0_WIDE;
	shadowOAM[5].attr1 = (COLMASK & shark3.col) | ATTR1_SIZE32;
	shadowOAM[5].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(0,16)));
	//shadowOAM[0].attr2 = SPRITEOFFSET16(0,0); //(2*pikachu.currFrame,2*pikachu.prevAniState);
	// shadowOAM[1].attr0 = (pokeball.row & ROWMASK);
 //    if(pokeball.hide) shadowOAM[1].attr0 |= ATTR0_HIDE;
 //    shadowOAM[1].attr1 = (pokeball.col & COLMASK) | ATTR1_SIZE32;
 //    shadowOAM[1].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(pokeball.currFrame * 4, pokeball.prevAniState * 4)));
}

void checkCollisions() {
	if (shark.isActive == 1) {

		if ((pikachu.row >= shark.row) && (pikachu.row <= (shark.row + 16)) && ((pikachu.col + 16) >= shark.col) && (pikachu.col <= (shark.col + shark.width))) {
		playSoundB(water,WATERLEN,WATERFREQ);
		soundB.loops = 0;
		journeyCount = 0;
		//pikachu.col = 20;
		hOff = 0;
		h1Off = 0;
		REG_BG0HOFS = hOff;
		hideSprites();
		// loadPalette(lose2Pal);
		// DMANow(3, lose2Tiles, &CHARBLOCKBASE[0], lose2TilesLen/2);
		// DMANow(3, lose2Map, &SCREENBLOCKBASE[28], lose2MapLen/2);
		loseValue = 2;
		state = DYINGSTATE;
		//goToLose2State();
		}
	}
	if (shark2.isActive == 1) {

		if ((pikachu.row >= shark2.row) && (pikachu.row <= (shark2.row + 16)) && ((pikachu.col + 16) >= shark2.col) && (pikachu.col <= (shark2.col + shark2.width))) {
			playSoundB(water,WATERLEN,WATERFREQ);
			soundB.loops = 0;
			journeyCount = 0;
			//pikachu.col = 20;
			hOff = 0;
			h1Off = 0;
			REG_BG0HOFS = hOff;
			hideSprites();
			// loadPalette(lose2Pal);
			// DMANow(3, lose2Tiles, &CHARBLOCKBASE[0], lose2TilesLen/2);
			// DMANow(3, lose2Map, &SCREENBLOCKBASE[28], lose2MapLen/2);
			// state = LOSESCREEN;
			//goToLose2State();
			loseValue = 2;
			state = DYINGSTATE;
		}
	}
	if (shark3.isActive == 1) {

		if ((pikachu.row >= shark3.row) && (pikachu.row <= (shark3.row + 16)) && ((pikachu.col + 16) >= shark3.col) && (pikachu.col <= (shark3.col + shark3.width))) {
			playSoundB(water,WATERLEN,WATERFREQ);
			soundB.loops = 0;
			journeyCount = 0;
			//pikachu.col = 20;
			hOff = 0;
			h1Off = 0;
			REG_BG0HOFS = hOff;
			hideSprites();
			// loadPalette(lose2Pal);
			// DMANow(3, lose2Tiles, &CHARBLOCKBASE[0], lose2TilesLen/2);
			// DMANow(3, lose2Map, &SCREENBLOCKBASE[28], lose2MapLen/2);
			// state = LOSESCREEN;
			loseValue = 2;
			state = DYINGSTATE;
			//goToLose2State();
		}
	}
}

/* ANIMATE */
void animate() {
// {
    if(pikachu.aniState != PIKAIDLE) pikachu.prevAniState = pikachu.aniState;
    pikachu.aniState = PIKAIDLE;
		
    if(pikachu.aniCounter % 20 == 0) 
    {
		ship.aniCounter = 0;
		if (ship.currFrame == 1) {
			ship.currFrame = 0;
		}
		else {
			ship.currFrame++;
		}
		pikachu.aniCounter = 0;
		if (pikachu.currFrame == 2) pikachu.currFrame = 0;
		else pikachu.currFrame++;
    }
    if ((journeyCount < 100 && journeyCount > 50) || ((journeyCount > 250) && (journeyCount < 350)) || ((journeyCount > 450) && (journeyCount < 550)) || ((journeyCount > 600) && (journeyCount < 700))) {
    	pirate.hide = 0;
    	if (pirate.currFrame == 0) {
    		pirate.col++;
    	} else {
    		pirate.col--;
    	}
    } else {
    	pirate.hide = 1;
    }

    if (pikachu.col < 200 && journeyCount > 1000) {
    	ship.hide = 0;
    } else {
    	ship.hide = 1;
    }
    if (ship.hide == 0 && (ship.col > 169)) {
    	ship.col--;
    }

    if (hOff > 40) {
    	if(pikachu.aniCounter % 20 == 0) 
    	{
	    	pirate.aniCounter = 0;
			if (pirate.currFrame == 1) pirate.currFrame = 0;
			else pirate.currFrame++;
		}
    }


    if(BUTTON_HELD(BUTTON_DOWN))
	{
		//pikachu.aniState = PIKABACK;
		if (pikachu.row > 76 && pikachu.row < 144) {
			pikachu.row++;
		}
	}
   	// if(BUTTON_HELD(BUTTON_DOWN))
   	else if((pikachu.row > (77)) && (journeyCount > 0))
   		{
   			pikachu.row--;
   		} 
 //    if(BUTTON_HELD(BUTTON_DOWN))
	// {
	// 	//pikachu.aniState = PIKAFRONT;
	// 	if ((pikachu.row + pikachu.height) <= 160) {
	// 		pikachu.row++;
	// 	}
		
	// }

    if(BUTTON_HELD(BUTTON_RIGHT))
	{
		ship.aniState = PIKARIGHT;
		pikachu.aniCounter++;
		if ((pikachu.col) <= ship.col) {
			pikachu.col++;
		}
		journeyCount++;
		hOff++;
		h1Off++;
	}
	if ((pikachu.col) >= pirate.col) {
		shark.col--;
		shark2.col--;
		shark3.col--;
	}
	if (shark.col > 150 && shark.col < 160 && shark.row > 80) {
		shark.row--;
	}
	if (shark2.col > 170 && shark2.col < 180 && shark2.row > 80) {
		shark2.row--;
	}
	if (shark2.col > 190 && shark2.col < 200 && shark3.row > 80) {
		shark3.row--;
	}
	if (shark.col > 30 && shark.col < 40 && shark.row < 150) {
		shark.row++;
	}
	if (shark2.col > 60 && shark2.col < 70 && shark2.row < 150) {
		shark2.row++;
	}
	if (shark2.col > 100 && shark2.col < 110 && shark3.row < 150) {
		shark3.row++;
	}
	if (shark.col + shark.width == 1) {
		shark.col = 250;
		if (shark.row > 90) {
			shark.row = shark.row - 5;
		} else {
			shark.row = 145;
		}
	}
	if (shark2.col + shark2.width == 1) {
		shark2.col = 250;
		if (shark2.row > 90) {
			shark2.row = shark2.row - 10;
		} else {
			shark2.row = 145;
		}
	}
	if (shark3.col + shark3.width == 1) {
		shark3.col = 250;
		if (shark3.row > 90) {
			shark3.row = shark3.row - 5;
		} else {
			shark3.row = 145;
		}
	}
    if(pikachu.aniState == PIKAIDLE)
	{
        pikachu.currFrame = 0;
	}
    else
	{
        pikachu.aniCounter++;
	}
	if (journeyCount < 5) {
		ship.hide = 0;
	} 
}











