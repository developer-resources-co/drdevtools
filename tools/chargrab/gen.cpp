//==============================================================================
// genesis.cpp:
//==============================================================================

#define CHAR_FLIPX 0x800
#define CHAR_FLIPY 0x1000

//==============================================================================

void
	CreateYFlip( void )
	{
	int x,y;
	for(y=0;y<8;y++)
		for(x=0;x<4;x++)
		 {
			tempcharY[x+((7-y)*4)]  = tempchar[x+(y*4)];
		 }
	}

void
	CreateXFlip( void )
	{
	int x,y,temp,temp2;
	for(y=0;y<8;y++)
		for(x=0;x<4;x++)
		 {
			temp = tempchar[x+(y*4)];
			temp2 = temp >> 4;
			temp2 |= (temp & 0xf) << 4;
			tempcharX[(3-x)+(y*4)] = temp2;
		 }
	}

void
	CreateXYFlip()
	{
	int x,y,temp,temp2;
	for(y=0;y<8;y++)
		for(x=0;x<4;x++)
		 {
			temp = tempcharY[x+(y*4)];
			temp2 = temp >> 4;
			temp2 |= (temp & 0xf) << 4;
			tempcharXY[(3-x)+(y*4)] = temp2;
		 }
	}

//==============================================================================

boolean
	MakeChar(int x, int y)
	{
	int c,d,pal;
	boolean good = boolean::TRUE;
	unsigned char planes[4],temp,t2;

	pal = 0xffff;
	for (c=0;c<CHAR_Y;c++)
	 {
    	for (d=0;d<CHAR_X;d++)
		 {
			temp = GetPixel (b2+d,a2+c);
			if(pal == 0xffff)
			 {
				if(temp != 0)
					pal = temp >> 4;
			 }
			else
			 {
				if(temp & 0xf)					 		// check for color zero
				 {
			 		if (pal != temp >> 4)
						good = boolean::FALSE;
				 }
				else
					if(temp > 0xf)		// if color zero of some other pallete, incorrect
						good = boolean::FALSE;
			 }

			if(d & 1)
			 {
    			tempchar[c*(CHAR_X>>1)+(d>>1)] = (temp & 0xf) | t2<<4;
				temp = 0;
			 }
			t2 = temp & 0xf;
		 }
	 }
	if(pal == 0xffff)
		palette = 0;
	else
		palette = pal << 13;

	CreateXFlip();
	CreateYFlip();
	CreateXYFlip();
	return(good);
	}

//==============================================================================

