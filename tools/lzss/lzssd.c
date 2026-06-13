
#define N         4096         /* size of ring buffer */
#define F           18         /* upper limit for match_length */
#define THRESHOLD    2         /* encode string into position and length
                                  if match_length is greater than this */
#define NIL          N         /* index for root of binary search trees */


void Decode(void)       /* Just the reverse of Encode(). */
	{
	unsigned int i, j;
	unsigned int c;
	int k, r;
	unsigned int  flags;
	unsigned char* p;
	unsigned int len;
	unsigned char* buffer;

	getc( infile );						// Read C
	getc( infile );						// Read L
	getc( infile );						// Read 2
	getc( infile );						// Read 0

	len = getc( infile );
	len |= getc( infile ) << 8;		// Read size
	printf( "inlen: %d\n", len );
	buffer = (char*)malloc( len );
	assert( buffer );
	fread( buffer, len, 1, infile );

	//for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	p = text_buf;
	for ( i=0; i<N-F; ++i )
		*p++ = ' ';

	r = N - F;
	flags = 0;

	for ( p=buffer; len; )
		{
		if (((flags >>= 1) & 256) == 0)
			{
			c = *p++;
			flags = c | 0xff00;		/* uses higher byte cleverly */
			}                               /* to count eight */
		if (flags & 1)
			{
			c = *p++;
			--len;
			putc(c, outfile);
			//			text_buf[r++] = c;  r &= (N - 1);
			text_buf[r] = c;  ++r;  r &= (N - 1);
			}
		else
			{
			i = *p++;
			j = *p++;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++)
				{
				c = text_buf[(i + k) & (N - 1)];
				putc(c, outfile);
				//text_buf[r++] = c;  r &= (N - 1);
				text_buf[r] = c;  ++r;  r &= (N - 1);
				--len;
				}
			}
		}
	free( buffer );
	}
