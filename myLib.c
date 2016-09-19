#include "myLib.h"

unsigned short *videoBuffer = (u16 *)0x6000000;

unsigned short *frontBuffer = (u16 *)0x6000000;
unsigned short *backBuffer =  (u16 *)0x600A000;

DMA *dma = (DMA *)0x40000B0;

void setPixel3(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row, col, SCREENWIDTH)] = color;
}

void drawRect3(int row, int col, int height, int width, unsigned short color)
{
	volatile unsigned short c = color;
	
	int i;
	for(i = 0; i < height; i++)
	{
		DMANow(3, &c, &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)], (width) | DMA_SOURCE_FIXED);
	}
}

void drawImage3(const unsigned short* image, int row, int col, int height, int width)
{
	int i;
	for(i = 0; i < height; i++)
	{
		DMANow(3, &image[OFFSET(i,0, width)], &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)], (width));
	}
}

void fillScreen3(unsigned short color)
{
	volatile unsigned short c = color;

	DMANow(3, &c, videoBuffer, (240*160) | DMA_SOURCE_FIXED);
	
}

void setPixel4(int row, int col, unsigned char colorIndex)
{
	volatile unsigned short pixels = videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)];

	if(col & 1) // odd
	{
		pixels &= 0xFF;
		videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex << 8;
	}
	else // even
	{
		pixels &= 0xFF << 8;
		videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex;
	}
}

void drawRect4(int row, int col, int height, int width, unsigned char colorIndex)
{
	volatile unsigned short pixels = colorIndex << 8 | colorIndex;

	int r;
	for(r = 0; r < height; r++)
	{
		if(col & 1) // odd starting col
		{
			setPixel4(row+r, col, colorIndex);

			if(width & 1) // if width is odd
			{
				DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_FIXED);
			}
			else  // width is even
			{
				DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)], ((width/2)-1) | DMA_SOURCE_FIXED);
				setPixel4(row+r, col+width - 1, colorIndex);
			}
		}
		else // even starting col
		{
			DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_FIXED);	
			if(width & 1) // if width is odd
			{
				setPixel4(row+r, col+width - 1, colorIndex);
			}
		}
	}	
}

void fillScreen4(unsigned char colorIndex)
{
	volatile unsigned short pixels = colorIndex << 8 | colorIndex;
	DMANow(3, &pixels, videoBuffer, ((240 * 160)/2) | DMA_SOURCE_FIXED);
}

void drawBackgroundImage4(const unsigned short* image)
{
	DMANow(3, image, videoBuffer, ((240 * 160)/2));
}

void drawImage4(const unsigned short* image, int row, int col, int height, int width)
{
	if(col & 1)
	{
		col++;
	}

	int r;
	for(r = 0; r < height; r++)
	{
		DMANow(3, &image[OFFSET(r,0,width/2)], &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], width/2);
	}
}

void drawSubImage4(const unsigned short* sourceImage, int sourceRow, int sourceCol, 
				   int row, int col, int height, int width)
{
	if(sourceCol & 1)
	{
		sourceCol++;
	}

	if(col & 1)
	{
		col++;
	}

	int r;
	for(r = 0; r < height; r++)
	{
		DMANow(3, &sourceImage[OFFSET(sourceRow + r, sourceCol/2,width/2)], &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], width/2);
	}
}

void loadPalette(const unsigned short* palette)
{
	DMANow(3, palette, PALETTE, 256);
}


void DMANow(int channel, volatile const void* source, volatile const void* destination, unsigned int control)
{
	dma[channel].src = source;
	dma[channel].dst = destination;
	dma[channel].cnt = DMA_ON | control;
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}


void flipPage()
{
    if(REG_DISPCTL & BACKBUFFER)
    {
        REG_DISPCTL &= ~BACKBUFFER;
        videoBuffer = backBuffer;
    }
    else
    {
        REG_DISPCTL |= BACKBUFFER;
        videoBuffer = frontBuffer;
    }
}

void setupSounds()
{
    REG_SOUNDCNT_X = SND_ENABLED;

	REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 | 
                     DSA_OUTPUT_RATIO_100 | 
                     DSA_OUTPUT_TO_BOTH | 
                     DSA_TIMER0 | 
                     DSA_FIFO_RESET |
                     DSB_OUTPUT_RATIO_100 | 
                     DSB_OUTPUT_TO_BOTH | 
                     DSB_TIMER1 | 
                     DSB_FIFO_RESET;

	REG_SOUNDCNT_L = 0;
}

void playSoundA( const unsigned char* sound, int length, int frequency) {
        dma[1].cnt = 0;
	
        int ticks = PROCESSOR_CYCLES_PER_SECOND/frequency;
	
        DMANow(1, sound, REG_FIFO_A, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);
	
        REG_TM0CNT = 0;
	
        REG_TM0D = -ticks;
        REG_TM0CNT = TIMER_ON;
	
        //DONE: FINISH THIS FUNCTION
        // Assign all the appropriate struct values
        soundA.data = sound;
        soundA.length = length;
        soundA.frequency = frequency;
        soundA.duration = ((VBLANK_FREQ*soundA.length) / soundA.frequency);
        soundA.isPlaying = 1;
        soundA.vbCount = 0;
}

void playSoundB( const unsigned char* sound, int length, int frequency) {

        dma[2].cnt = 0;

        int ticks = PROCESSOR_CYCLES_PER_SECOND/frequency;

        DMANow(2, sound, REG_FIFO_B, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);

        REG_TM1CNT = 0;
	
        REG_TM1D = -ticks;
        REG_TM1CNT = TIMER_ON;
	
	    soundB.data = sound;
        soundB.length = length;
        soundB.frequency = frequency;
        soundB.duration = ((VBLANK_FREQ*soundB.length) / soundB.frequency);
        soundB.isPlaying = 1;
        soundB.vbCount = 0;
        // DONE: FINISH THIS FUNCTION
        // Assign all the appropriate struct values

}

void pauseSound()
{
	// DONE: WRITE THIS FUNCTION
	REG_TM0CNT = 0;
	REG_TM1CNT = 0;
	soundA.isPlaying = 0;
	soundB.isPlaying = 0;
}

void unpauseSound()
{
	// DONE: WRITE THIS FUNCTION
	REG_TM0CNT = TIMER_ON;
	REG_TM1CNT = TIMER_ON;
	soundA.isPlaying = 1;
	soundB.isPlaying = 1;
}

void stopSound()
{
    // DONE: WRITE THIS FUNCTION
    dma[1].cnt = 0;
	REG_TM0CNT = 0;
	soundA.isPlaying = 0;

	dma[2].cnt = 0;
	REG_TM1CNT = 0;
	soundB.isPlaying = 0;
}

void setupInterrupts()
{
	REG_IME = 0;
	// DONE: SET UP THE INTERRUPT HANDLER HERE
	// HINT: THERE IS A REGISTER THAT NEEDS TO POINT TO A INTERRUPT FUNCTION
	REG_INTERRUPT = (unsigned int) interruptHandler;
	REG_IE |= INT_VBLANK;
	REG_DISPSTAT |= INT_VBLANK_ENABLE;
	REG_IME = 1;
}

void interruptHandler()
{
	REG_IME = 0;
	if(REG_IF & INT_VBLANK)
	{
		if (soundA.isPlaying)
		{
			if (soundA.vbCount >= soundA.duration)
			{
				if (soundA.loops)
				{
					playSoundA(soundA.data, soundA.length, soundA.frequency);
				} else {
					dma[1].cnt = 0;
					REG_TM0CNT = 0;
					soundA.isPlaying = 0;
				}
			}
		soundA.vbCount++;
		REG_IF = INT_VBLANK;
		}
		if (soundB.isPlaying)
		{
			if (soundB.vbCount >= soundB.duration)
			{
				if (soundB.loops)
				{
					playSoundB(soundB.data, soundB.length, soundB.frequency);
				} else {
					dma[2].cnt = 0;
					REG_TM1CNT = 0;
					soundB.isPlaying = 0;
				}
			}
		soundB.vbCount++;
		REG_IF = INT_VBLANK; 
		}
		//DONE: FINISH THIS FUNCTION
		//This should be where you determine if a sound if finished or not
	}

	REG_IME = 1;
}











