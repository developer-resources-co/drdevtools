//=============================================================================
// coff2bin.hpp:
//=============================================================================

#include <stdio.h>
#include <time.h>

//#include <aouthdr.h>
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
#define N_SCNUM	    1 - 127 /* section nbr where sym is defined	 */

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

//=============================================================================
