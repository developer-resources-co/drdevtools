// used by brr.c and unbrr.c

//==============================================================================
// given a brr record, decompress into 16-bit binary samples
// note: the filters use globals to remember the previous 2 samples


// these are named differently from the compressor so they will
// work and play well together
int	prev,prev2;

void
UnDoBrr(char iPtr[],int oPtr[],int brrByte)
{
	int i;					// interation counter through the block
	int db;					// shift value for the block
	char compType;				// which compression type is used in this block
	char temp;				// used for sign-extension of 4-bit deltas
	int t2;					// temporary sample holder
	int current;				// current output sample(cpoied to prev)
	long a,b;				// filter values
	long ax,bx; 				// filters after applying previous samples

	db = brrByte>>4;
	compType = (brrByte >> 2) & 3;

	switch(compType)
	 {
		case 0:
			a = 0;
			b = 0;
			break;
		case 1:
			a = 61440;				// .9375 * 65536
			b = 0;
			break;
		case 2:
			a = 124928;				// 1.90625 * 65536
			b = -61440;				// -.9375 * 65536
			break;
		case 3:
			a = 117760;				// 1.796875 * 65536
			b = -53248;				// -0.8125 * 65536
			break;
	 }

	// convert the data
	for(i=0;i<16;i++)
	 {
		temp = iPtr[i]<<4;
		t2 = (int)temp;				// sign extend
		t2  >>=4;
		t2 <<= db;
			// now apply filter
		ax = a*(prev<<16);				// .9375 * 65536
		bx = b*(prev2<<16);				// .9375 * 65536
		current = t2+(ax>>16)+(bx>>16);
		oPtr[i] = current;
		prev2 = prev;
		prev = current;
	 }
}

//==============================================================================
