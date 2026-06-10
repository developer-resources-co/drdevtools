
#ifndef DRMON_ZARDOZ_HPP
#define DRMON_ZARDOZ_HPP


typedef struct lineRec {
	long addr;
	short linNum;
	short nBytes;
	char filNum;
	char ps;
	} LinRec;


inline long
get_long( FILE* fp )
	{
	long v;

	v = (unsigned long)getc( fp );
	v |= ((unsigned long)getc( fp ) ) << 8;
	v |= ((unsigned long)getc( fp ) ) << 16;
	v |= ((unsigned long)getc( fp ) ) << 24;

	return v;
	}


inline long
get_long( istream& input )
	{
	unsigned char c;
	long v;

	input.get( (char&)c );
	v = (unsigned long)c;
	input.get( (char&)c );
	v |= ((unsigned long)c ) << 8;
	input.get( (char&)c );
	v |= ((unsigned long)c ) << 16;
	input.get( (char&)c );
	v |= ((unsigned long)c ) << 24;

	return v;
	}

inline short
get_word( FILE* fp )
	{
	short v;

	v = (unsigned long)getc( fp );
	v |= ((unsigned long)getc( fp ) ) << 8;

	return v;
	}

inline short
get_word( istream& input )
	{
	unsigned char c;
	short v;

	input.get( (char&)c );
	v = (unsigned long)c;
	input.get( (char&)c );
	v |= ((unsigned long)c ) << 8;

	return v;
	}

inline char
get_byte( istream& input )
	{
	unsigned char c;

	input.get( (char&)c );

	return c;
	}


class zardoz
	{
public:
	zardoz( FILE* fd );
	~zardoz();

private:
	zardoz() {}							// cannot construct without data
	FILE* zardoz_fd;

//	void read_headers();
//	void read_sections();
//	void read_strings();
	void read_symbols();

//	void LoadSymbols(void);
	};

extern zardoz* zardozBase;

#endif

