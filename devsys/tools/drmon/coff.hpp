//==============================================================================
// coff.hpp:
//==============================================================================

#ifndef DRMON_COFF_HPP
#define DRMON_COFF_HPP

//==============================================================================

#include <stdio.h>
#include <pclib/list.hpp>
#include <time.h>

//==============================================================================

typedef long fileOffset;

unsigned short swaphilo( unsigned short i );
long swaphilo( long l );

//==============================================================================

inline ulong ReadLong(istream &input)
{
	ulong data;
	ubyte temp;
	input.get((char&)temp);   // istream::get only binds char&; temp stays unsigned
	data = ((ulong)temp) << 24;
	input.get((char&)temp);
	data |= ((ulong)temp) << 16;
	input.get((char&)temp);
	data |= ((ulong)temp) << 8;
	input.get((char&)temp);
	data |= ((ulong)temp);
	return(data);
}

//==============================================================================

inline uword ReadWord(istream &input)
{
	uword data;
	ubyte temp;
	input.get((char&)temp);   // istream::get only binds char&; temp stays unsigned
	data = ((ulong)temp) << 8;
	input.get((char&)temp);
	data |= ((ulong)temp);
	return(data);
}

//==============================================================================

#if defined( DEBUGCOFF)
//#include <aouthdr.h>
#define COFF_SIERRA_MAGIC_HEADER	0x150
typedef struct aouthdr
	{
	short magic;					// magic number
	short vstamp;					// version stamp
	long tsize;						// text size in bytes
	long dsize;						// initialized data size
	long bsize;						// uninitialized data size
	long entry;						// entry point
	long text_start;				// base of text used for this file
	long data_start;				// base of data used for this file
	} AOUTHDR;


//#include <filehdr.h>
typedef struct filehdr
	{
	unsigned short f_magic;		// magic number
	unsigned short f_nscns;		// number of sections
	long           f_timdat;	// time & date stamp
	long           f_symptr;	// file pointer to symtab
	long           f_nsyms;		// number of symtab entries
	unsigned short f_opthdr;	// sizeof( optional hdr )
	unsigned short f_flags;		// flags;
	} FILHDR;
#define FILHSZ	sizeof( FILHDR )

#define F_RELFLG	0x0001		// does not contain relocation information
#define F_EXEC		0x0002		// executable
#define F_LNNO		0x0004		// line numbers stripped
#define L_SYMS		0x0010		// local symbols stripped


//#include <scnhdr.h>
typedef struct scnhdr
	{
	char           s_name[8];	// section name
	long           s_paddr;		// physical address
	long           s_vaddr;		// virtual address
	long           s_size;		// section size
	long           s_scnptr;	// file ptr to raw data for section
	long           s_relptr;	// file ptr to relocation
	long           s_lnnoptr;	// file ptr to line numbers
	unsigned short s_nreloc;	// number of relocation entries
	unsigned short s_nlnno;		// number of line number entries
	long           s_flags;		// type and content flags
	} SCNHDR;
#define SCNHSZ	sizeof( SCNHDR )

// Section header flags
#define SH_REG	    0x0	    /* regular section: ALLOC'D, RELOC'D, LOAD'D    */
#define SH_DSECT    0x1	    /* dummy section: \alloc'd, RELOC'D, \load'd    */
#define SH_NOLOAD   0x2	    /* noload section: ALLOC'D, RELOC'D, \load'd    */
#define SH_RESIDENT 0x2	    /* noload section: ALLOC'D, RELOC'D, \load'd    */
#define SH_GROUP    0x4	    /* grouped section: formed from input sections  */
#define SH_PAD	    0x8	    /* padding section: \alloc'd, \reloc'd, LOAD'D  */
#define SH_FILLONLY 0x8	    /* fill only section: filled at run-time	    */
#define SH_COPY	    0x10    /* copy section: copied at run-time from	    */
			    /* desitination address to virtual address	    */
#define SH_TEXT	    0x20    /* section contains executable text		    */
#define SH_DATA	    0x40    /* section contains initialized data	    */
#define SH_BSS	    0x80    /* section contains only uninitialized data	    */
#define SH_ORG	    0x100   /* section contains ORG'd (absolute) data	    */
#define SH_INFO	    0x200   /* comment section: \alloc'd, \reloc'd, \load'd */
#define SH_OVERT    0x400   /* overlay section: RELOC'D, \alloc'd, \load'd  */
#define SH_LIB	    0x800   /* for .lib section: treated like STYP_INFO	    */

#define SH_TEXT_NAME	    ".text"
#define SH_DATA_NAME	    ".data"
#define SH_BSS_NAME	    ".bss"
#define SH_LOAD_TBL_NAME    ".ld_tbl"


//#include <linenum.h>
typedef struct lineno
	{
	union
		{
		long l_symndx;				// if l_lnno == 0
										//  then l_symndx
		long l_paddr;				//  else l_paddr
		} l_addr;
	unsigned short l_lnno;		// line number
	} LINENO;

#define LINESZ	sizeof( LINENO )


//#include <reloc.h>
typedef struct reloc
	{
	long           r_vaddr;		// address of reference
	long           r_symndx;	// index into symbol table
	unsigned short r_type;		// relocation type
	} RELOC;
#define RELSZ sizeof( RELOC )

/* WARNING: do not use relocation type flags that use bits in the high order */
/* byte of ->r_type_info.  A Sierra Systems extension to COFF depends on     */
/* ->r_type_info[0] and r_sym_index[0] being avaialable for internal use in  */
/* the assembler and linker.						     */

#define RL_FIXED    0x0
#define RL_DIR_BYTE 0xf
#define RL_DIR_WORD 0x10
#define RL_DIR_LONG 0x11
#define RL_PC_BYTE  0x12
#define RL_PC_WORD  0x13
#define RL_PC_LONG  0x14


//#include <syms.h>
#define SYMNMLEN 8
typedef struct syment
	{
	union
		{
		char _n_name[SYMNMLEN];	// symbol name
		struct						// if _n_name[0--3] == 0
			{
			long _n_zeroes;		// then _n_name[4--7] is an
			long _n_offset;		// offset into string table
			} _n_n;
		char* _n_nptr[2];			// allows for overlaying
		} _n;
	long           n_value;		// value of symbol
	short          n_scnum;		// section number
	unsigned short n_type;		// type and derived type
	char           n_sclass;	// storage class
	char           n_numaux;	// number of aux. entries
	} SYMENT;
#define n_name		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

/* Storage Class */

#define C_EFCN	    -1	    /* physical end of function		 */
#define C_NULL	    0
#define C_AUTO	    1	    /* automatic variable		 */
#define C_EXT	    2	    /* external symbol			 */
#define C_STAT	    3	    /* static				 */
#define C_REG	    4	    /* register variable		 */
#define C_EXTDEF    5	    /* external definition		 */
#define C_LABEL	    6	    /* label				 */
#define C_ULABEL    7	    /* undefined label			 */
#define C_MOS	    8	    /* member of structure		 */
#define C_ARG	    9	    /* function argument		 */
#define C_STRTAG    10	    /* structure tag			 */
#define C_MOU	    11	    /* member of union			 */
#define C_UNTAG	    12	    /* union tag			 */
#define C_TPDEF	    13	    /* type definition			 */
#define C_USTATIC   14	    /* uninitialized static		 */
#define C_ENTAG	    15	    /* enumeration tag			 */
#define C_MOE	    16	    /* member of enumeration		 */
#define C_REGPARM   17	    /* register parameter		 */
#define C_FIELD	    18	    /* bit field			 */
#define C_ARRAY	    19	    /* array dimension information	 */
#define C_SUE	    20	    /* struct, union or enum		 */
#define C_SKIP	    21	    /* symbol that should not be output	 */
#define C_BLOCK	    100	    /* beginning and end of block	 */
#define C_FCN	    101	    /* beginning and end of function	 */
#define C_EOS	    102	    /* end of structure			 */
#define C_FILE	    103	    /* file name			 */
#define C_ALIAS	    105	    /* duplicate tag			 */
#define C_HIDDEN    106	    /* like static, avoids name conflict */

/* Section Number Field */

#define N_DEBUG	    -2	    /* special symbolic debugging symbol */
#define N_ABS	    -1	    /* absolute symbol			 */
#define N_UNDEF	    0	    /* undefined external symbol	 */
//#define N_SCNUM	    1 - 127 /* section nbr where sym is defined	 */

/* fundemental types */

#define T_NULL	    0	    /* type not assigned */
#define T_LDOUBLE   1	    /* long double	 */
#define T_CHAR	    2	    /* character	 */
#define T_SHORT	    3	    /* short		 */
#define T_INT	    4	    /* integer		 */
#define T_LONG	    5	    /* long integer	 */
#define T_FLOAT	    6	    /* float		 */
#define T_DOUBLE    7	    /* double		 */
#define T_STRUCT    8	    /* structure	 */
#define T_UNION	    9	    /* union		 */
#define T_ENUM	    10	    /* enumeration	 */
#define T_MOE	    11	    /* member of enum	 */
#define T_UCHAR	    12	    /* unsigned char	 */
#define T_USHORT    13	    /* unsigned short	 */
#define T_UINT	    14	    /* unsigned integer	 */
#define T_ULONG	    15	    /* unsigned long	 */

/* defined types */

#define DT_NON	    0	    /* no derived type	 */
#define DT_PTR	    1	    /* pointer		 */
#define DT_FCN	    2	    /* function		 */
#define DT_ARY	    3	    /* array		 */

#define ISFCN( n_type ) ( n_type & DT_FCN )
#define ISPTR( n_type )	( n_type & DT_PTR )
#define ISARY( n_type ) ( n_type & DT_ARY )

#define COFF_FILE_NAME_LEN  14
#define NBR_OF_DIMS	    4

typedef union auxent {
    char aux_file_name[COFF_FILE_NAME_LEN];
    struct {
	unsigned char tag_index[4];
	union {
	    struct {
		unsigned char c_line_nbr[2];
		unsigned char size[2];
	    } s;
	    unsigned char func_size[4];
	} u1;
	union {
	    struct {
		unsigned char line_ptr[4];
		unsigned char end_index[4];
	    } s;
	    unsigned char array_dim[NBR_OF_DIMS][2];
	} u2;
	unsigned char high_size[2];
    } symbol;
    struct {
	unsigned char len[4];
	unsigned char nbr_rel_ents[2];
	unsigned char nbr_line_nbrs[2];
    } sec;
} AUXENT;
#define AUXESZ sizeof( AUXENT )

//==============================================================================

class coffSymbol
{
public:
	coffSymbol(istream &initStream,char *stringTabPtr)
	 {
		for(int i = 0; i < 8; i++)
			initStream.get(name[i]);
		name[8] = 0;

		if(name[0] == 0)
		 {
			long offset;

			offset = *(long *)&name[4];
			offset = swaphilo(offset);
			namePtr = stringTabPtr+offset;
		 }
		else
			namePtr = &name[0];

		n_value = ReadLong(initStream);
		n_scnum = ReadWord(initStream);
		n_type =  ReadWord(initStream);
		initStream.get(n_sclass);
		initStream.get(n_numaux);
	 }

	const char *Name(void) { return(namePtr); }

	long Addr(void) { return(n_value); }
	unsigned short Type(void) { return(n_type); }
	unsigned short SClass(void) { return(n_sclass); }
	UBYTE NumAuxEntries(void) { return(n_numaux); }

private:
	char name[9];
	char *namePtr;
	long           n_value;		// value of symbol
	short          n_scnum;		// section number
	unsigned short n_type;		// type and derived type
	char           n_sclass;	// storage class
	char           n_numaux;	// number of aux. entries
};

//==============================================================================

class coffAuxSymbol
{
public:
	coffAuxSymbol(istream &initStream,char *stringTabPtr)
	 {
		for(int i = 0; i < AUXESZ; i++)
			initStream.get(data[i]);
	 }

	coffAuxSymbol(coffAuxSymbol &copy)
	 {
		for(int i = 0; i < AUXESZ; i++)
			data[i] = copy.data[i];
	 }

	 /*
	operator=(coffAuxSymbol &copy)
	 {
		for(int i = 0; i < AUXESZ; i++)
			data[i] = copy.data[i];
	 }
	  */

	coffAuxSymbol()
	 {
		data[0] = 0;
	 }

	char *Data(void) { return &data[0]; }
private:
	char data[AUXESZ];
};

//==============================================================================

class coffLineNo
{
public:
	coffLineNo(fileOffset newData, unsigned short newEntries, istream *newCoffFile)
	 {
	 	dataOffset = newData;
		entries = newEntries;
		functionSourceLine = 0;
		currentSourceLine = 0;
		fileName[0] = 0;
		fileName[AUXESZ] = 0;
		coffFile = newCoffFile;
		funcSymNdx = 0;
		exactMatch = boolean::FALSE;
		currentPC = OUT_OF_BAND;
	 }

	~coffLineNo() {}

	unsigned short Entries(void) { return(entries); }

	UWORD CurrentSourceLine() { return(currentSourceLine); }
	const char *FileName() { return(&fileName[0]); }
	UWORD CurrentPC() { return(currentPC); }

	boolean FindSource(ULONG addr);
	boolean FindAddress(char *fileName, UWORD line);
	boolean ExactMatch(void) { return(exactMatch); }

private:
	coffLineNo() {}						// cannot construct without data

	fileOffset dataOffset;
	unsigned short entries;

	boolean exactMatch;								// did we actually find a source line?

	ULONG funcSymNdx;
	UWORD functionSourceLine;						// line # of function pc is in(used for relative source line offsets)
	UWORD currentSourceLine;	   					// current absosulte source line pc is on
	char fileName[AUXESZ+1];				// current file pc is in
	ULONG currentPC;
	istream *coffFile;
};

//==============================================================================

class coffSection : public nodeInterface<coffSection>
{
public:
	coffSection(SCNHDR *s, istream *coffStream) : nodeInterface<coffSection>(), sectionLineNo(s->s_lnnoptr,s->s_nlnno,coffStream)
	 {
		int len;
		name = new char[9];				// kts for now
		strncpy(name,s->s_name,8);
		name[8] = 0;

		pAddr = s->s_paddr;
		vAddr = s->s_vaddr;
		size = s->s_size;
		flags = s->s_flags;
		rawData = s->s_scnptr;
	 }

	~coffSection()
	 {
		delete name;
	 }

	const char *Name() { return(name); }
	long PAddr(void) { return(pAddr); }
	long VAddr(void) { return(vAddr); }
	long Size(void) { return(size); }

	fileOffset DataOffset(void) { return(rawData); }

	coffLineNo &GetLineNo(void) { return(sectionLineNo); }
	long Flags(void) { return(flags); }
private:
	coffSection();						// cannot construct without data
	char *name;
	long pAddr;
	long vAddr;
	long size;
	fileOffset rawData;
	coffLineNo sectionLineNo;
	long flags;
};

//==============================================================================

#include "reg.hpp"

class Scope
	{
public:
	Scope() { nCount = 0; inScope = 0; outScope = ~0; }
	Scope( ULONG start ) { nCount = 0; inScope = start; outScope = ~0; }
	~Scope() { }

	ULONG Start()	{ return( inScope ); }
	ULONG Start( ULONG newStart )	{ return( inScope = newStart ); }

	ULONG End()		{ return( outScope ); }
	ULONG End( ULONG newEnd )	{ return( outScope = newEnd ); }

	boolean InScope( ULONG addr )	{ return( inScope<=addr && addr<=outScope ); }
	boolean InScope()	{ return( InScope( GetReg(REG_PC) ) ); }

private:
	int nCount;
	ULONG inScope, outScope;
	};

//==============================================================================

class coff
	{
public:
	coff( FILE* fd , istream *newCoffStream);
	~coff( void );

	coffSection *GetSectionPtr(void) { return(sectionList.GetNext()); }

	char *GetStringTable(void) { return(str_tab); }
	long GetStringBase(void) { return(strptr); }
	long GetSymbolBase(void) { return(symptr); }
	FILE *GetFileHandle(void) { return(coff_fd); }
	istream *GetStream(void) { return(coffStream); }

	ulong BinarySize(void) { return(cbBinary); }

private:
	coff() {} 							// cannot construct without data
	FILE* coff_fd;

	FILHDR file_header;
	AOUTHDR optional_header;

	long nSections;
	long section_seek;

	long symptr;
	long num_symbols;

	long strptr;

	char huge *str_tab;
	long str_length;

	void symbols_stack( long* i, Scope* scope );
	void read_headers( void );
	void read_sections( void );
	void read_strings( void );
	void read_symbols( void );
	void read_binary( void );

	void LoadSymbols(void);

	list<coffSection> sectionList;
 	unsigned long cbBinary;

	istream *coffStream;

	};
extern coff* coffBase;

#else

#define C_LABEL	    6	    /* label				 */

#endif

//=============================================================================

#endif

//=============================================================================
