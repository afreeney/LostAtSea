#include "myLib.h"

#include "pikachu.h"
#include "house.h"

// TODO Include collisionmap.h

unsigned int buttons;
unsigned int oldButtons;

int hOff=0;
int vOff=0;

OBJ_ATTR shadowOAM[128];

int collMapSize = 256;

#define ROWMASK 0xFF
#define COLMASK 0x1FF

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
} SPRITE;

SPRITE pikachu;
SPRITE pokeball;

void hideSprites();
void animate();
void updateOAM();

enum { PIKAFRONT, PIKABACK, PIKARIGHT, PIKALEFT, PIKAIDLE};
enum { BALLIDLE = 2};

int main()
{
	REG_DISPCTL = MODE0 | BG0_ENABLE | SPRITE_ENABLE;
	REG_BG0CNT = CBB(0) | SBB(28) | BG_SIZE0 | COLOR256;
	
	loadPalette(housePal);
	DMANow(3, houseTiles, &CHARBLOCKBASE[0], houseTilesLen/2);
	DMANow(3, houseMap, &SCREENBLOCKBASE[28], houseMapLen/2);
	
	DMANow(3, pikachuTiles, &CHARBLOCKBASE[4], pikachuTilesLen/2);
    DMANow(3, pikachuPal, SPRITE_PALETTE, pikachuPalLen/2);
	
	hideSprites();
	initialize();
	
	hOff = 9;
	vOff = 96;

	while(1)
	{
		oldButtons = buttons;
		buttons = BUTTONS;
		
		//Handle movement and collision logic using pikachu.bigRow and pikachu.bigCol
		if(BUTTON_HELD(BUTTON_UP))
		{
			//Move and check for collision
		}
		if(BUTTON_HELD(BUTTON_DOWN))
		{
			//Move and check for collision
		}
		if(BUTTON_HELD(BUTTON_LEFT))
		{
			//Move and check for collision
		}
		if(BUTTON_HELD(BUTTON_RIGHT))
		{
			//Move and check for collision
		}
		
		REG_BG0HOFS = hOff;
		REG_BG0VOFS = vOff;

		//Updates pikachu's world position based on its row/col movement
		pikachu.bigRow = pikachu.row + vOff;
		pikachu.bigCol = pikachu.col + hOff;
		
		//Updates the stationary pikachu's row/col screen position based on its world position
		pokeball.row = pokeball.bigRow - vOff;
		pokeball.col = pokeball.bigCol - hOff;

		pokeball.hide = (pokeball.row < -pokeball.height || pokeball.row > 160) ? 1 : 0;
		
		animate();
		updateOAM();
		
		DMANow(3, shadowOAM, OAM, 512);
		waitForVblank();
	}
	return 0;
}

void hideSprites()
{
    int i;for(i = 0; i < 128; i++)
    {
         shadowOAM[i].attr0 = ATTR0_HIDE;
    }
}

void initialize()
{
	pikachu.width = 16;
	pikachu.height = 16;
	pikachu.rdel = 1;
	pikachu.cdel = 1;
	pikachu.row = 160/2-pikachu.width/2;
	pikachu.col = 240/2-pikachu.height/2;
	pikachu.aniCounter = 0;
	pikachu.currFrame = 0;
	pikachu.aniState = PIKAFRONT;
	
	pokeball.width = 32;
    pokeball.height = 32;
    pokeball.bigRow = 50;
    pokeball.bigCol = 50;

    pokeball.aniCounter = 0;
    pokeball.currFrame = 0;

    pokeball.aniState = BALLIDLE;
    pokeball.prevAniState = BALLIDLE;

    pokeball.frameDirection = 1;
}

void animate()
{
    if (pikachu.aniState != PIKAIDLE) pikachu.prevAniState = pikachu.aniState;
		
    pikachu.aniState = PIKAIDLE;
		
    if(pikachu.aniCounter % 20 == 0) 
    {
		pikachu.aniCounter = 0;
		if (pikachu.currFrame == 2) pikachu.currFrame = 0;
		else pikachu.currFrame++;
    }

    if(pokeball.aniCounter % 20 == 0)
    {
        pokeball.currFrame += pokeball.frameDirection;
        if(pokeball.currFrame == 2 || pokeball.currFrame == 0)
            pokeball.frameDirection *= -1;
    }

    if(BUTTON_HELD(BUTTON_UP))
	{
		pikachu.aniState = PIKABACK;
	}
    if(BUTTON_HELD(BUTTON_DOWN))
	{
		pikachu.aniState = PIKAFRONT;
	}
    if(BUTTON_HELD(BUTTON_LEFT))
	{
		pikachu.aniState = PIKALEFT;
	}
    if(BUTTON_HELD(BUTTON_RIGHT))
	{
		pikachu.aniState = PIKARIGHT;
	}
		
    if(pikachu.aniState == PIKAIDLE)
	{
        pikachu.currFrame = 0;
	}
    else
	{
        pikachu.aniCounter++;
	}
	pokeball.aniCounter++;
}

void updateOAM()
{
	shadowOAM[0].attr0 = (ROWMASK & pikachu.row) | ATTR0_4BPP | ATTR0_SQUARE;
	shadowOAM[0].attr1 = (COLMASK & pikachu.col) | ATTR1_SIZE16;
	shadowOAM[0].attr2 = SPRITEOFFSET16(2*pikachu.currFrame,2*pikachu.prevAniState);
	
	shadowOAM[1].attr0 = (pokeball.row & ROWMASK);
    if(pokeball.hide) shadowOAM[1].attr0 |= ATTR0_HIDE;
    shadowOAM[1].attr1 = (pokeball.col & COLMASK) | ATTR1_SIZE32;
    shadowOAM[1].attr2 = ((ATTR2_PALBANK(1)) | (SPRITEOFFSET16(pokeball.currFrame * 4, pokeball.prevAniState * 4)));
}