/*==========================================================================*/
/*==    Genesis.h: C version of equates for genesis                       ==*/
/*==    (c) 1991 Developer Resources                                      ==*/
/*==========================================================================*/

/*==========================================================================*/
/* joystick equates							    */
/*--------------------------------------------------------------------------*/
/* Bit 0 = up / Bit 1 = Down / Bit 2 = Left / Bit 3 = Right                 */
/* Bit 4 = B  / Bit 5 = C    / Bit 6 = A    / Bit 7 = START		    */

#define	JOYB_UP              0
#define	JOYB_DOWN            1
#define	JOYB_LEFT            2
#define	JOYB_RIGHT           3
#define	JOYB_B               4
#define	JOYB_C               5
#define	JOYB_A               6
#define	JOYB_START           7

#define	JOYF_UP              1
#define	JOYF_DOWN            2
#define	JOYF_LEFT            4
#define	JOYF_RIGHT           8
#define	JOYF_B               0x10
#define	JOYF_C               0x20
#define	JOYF_A               0x40
#define	JOYF_START           0x80

/*==========================================================================*/
/* vdp equates								    */

enum
{
	CHARSIZE	=	32,
	CHAR_XSIZE	=	8,
	CHAR_YSIZE	=	8,
	VDP_MEM_SIZEOF	=	0x10000
};

#define	CHARNUMCOLS		16	 /* number of colors per character */
#define	VDPSIZE			0x10000


#define	VDP_REG0		0x8000
#define	VDP_REG1		0x8100
#define	VDP_REG2		0x8200
#define	VDP_REG3		0x8300
#define	VDP_REG4		0x8400
#define	VDP_REG5		0x8500
#define	VDP_REG6		0x8600
#define	VDP_REG7		0x8700
#define	VDP_REG8		0x8800
#define	VDP_REG9		0x8900
#define	VDP_REG10		0x8A00
#define	VDP_REG11		0x8B00
#define	VDP_REG12		0x8C00
#define	VDP_REG13		0x8D00
#define	VDP_REG14		0x8E00
#define	VDP_REG15		0x8F00
#define	VDP_REG16		0x9000
#define	VDP_REG17		0x9100
#define	VDP_REG18		0x9200
#define	VDP_REG19		0x9300
#define	VDP_REG20		0x9400
#define	VDP_REG21		0x9500
#define	VDP_REG22		0x9600
#define	VDP_REG23		0x9700

/*--------------------------------------------------------------------------*/

#define	VDP_REG_MODESET1			0x8000
#define	VDP_REG_MODESET2       		0x8100
#define	VDP_REG_FIELDABASE     		0x8200
#define	VDP_REG_WINDOWBASE     		0x8300
#define	VDP_REG_FIELDBBASE     		0x8400
#define	VDP_REG_SPRITEBASE     		0x8500
#define	VDP_REG_BACKCOLOR      		0x8700
#define	VDP_REG_HINTERUPT      		0x8A00
#define	VDP_REG_MODESET3       		0x8B00
#define	VDP_REG_MODESET4       		0x8C00
#define	VDP_REG_HSCROLLBASE    		0x8D00
#define	VDP_REG_AUTOINCSIZE    		0x8F00
#define	VDP_REG_FIELDSIZE     		0x9000
#define	VDP_REG_WINDOWHPOS     		0x9100
#define	VDP_REG_WINDOWVPOS     		0x9200
#define	VDP_REG_DMALENLOW      		0x9300
#define	VDP_REG_DMALENHIGH     		0x9400
#define	VDP_REG_DMASOURCELOW   		0x9500
#define	VDP_REG_DMASOURCEMID  		0x9600
#define	VDP_REG_DMASOURCEHIGH  		0x9700

/*--------------------------------------------------------------------------*/

#define	VDP_DMA_START		0x00000080

#define	VDP_MODE_VRW         0x40000000
#define	VDP_MODE_CRW         0xC0000000
#define	VDP_MODE_VSW         0x40000010
#define	VDP_MODE_VRR         0x00000000
#define	VDP_MODE_CRR         0x00000020
#define	VDP_MODE_VSR         0x00000010

#define	VDP_STATB_PAL        0x0
#define	VDP_STATB_DMA        0x1
#define	VDP_STATB_HB         0x2
#define	VDP_STATB_VB         0x3
#define	VDP_STATB_ODD        0x4
#define	VDP_STATB_C          0x5
#define	VDP_STATB_SOVR       0x6
#define	VDP_STATB_F          0x7
#define	VDP_STATB_FULL       0x8
#define	VDP_STATB_EMPT       0x9

#define	VDP_STATF_PAL        0x1
#define	VDP_STATF_DMA        0x2
#define	VDP_STATF_HB         0x4
#define	VDP_STATF_VB         0x8
#define	VDP_STATF_ODD        0x10
#define	VDP_STATF_C          0x20
#define	VDP_STATF_SOVR       0x40
#define	VDP_STATF_F          0x80
#define	VDP_STATF_FULL       0x100
#define	VDP_STATF_EMPT       0x200

#define	VDP_R0F_REQ          0x4              /* required on bits */
#define	VDP_R0F_HINT         0x10
#define	VDP_R0F_HVCOUNT      0x2

#define	VDP_R0B_HBLANK       0x4
#define	VDP_R0B_HVCOUNT      0x1

#define	VDP_R1F_REQ          0x4              /* required on bits */
#define	VDP_R1F_DISP         0x40
#define	VDP_R1F_VINT         0x20
#define	VDP_R1F_DMAEN        0x10
#define	VDP_R1F_30CELL       0x80

#define	VDP_R1B_DISP         0x7
#define	VDP_R1B_VINT         0x6
#define	VDP_R1B_DMAEN        0x5
#define	VDP_R1B_30CELL       0x4

#define	VDP_R11F_EXINT       0x8
#define	VDP_R11F_VSCR        0x4
#define	VDP_R11F_HFULL       0x0
#define	VDP_R11F_H1CELL      0x2
#define	VDP_R11F_H1LINE      0x3

#define	VDP_R11B_IE2         0x3
#define	VDP_R11B_VSCR        0x2

#define	VDP_R12F_STE         0x4
#define	VDP_R12F_LACE        0x2
#define	VDP_R12F_DLACE       0x6
#define	VDP_R12F_H32         0x0
#define	VDP_R12F_H40         0x81
#define	VDP_R12B_STE         0x3

#define	VDP_R16F_SCRV32      0x0
#define	VDP_R16F_SCRV64      0x10
#define	VDP_R16F_SCRV128     0x30
#define	VDP_R16F_SCRH32      0x0
#define	VDP_R16F_SCRH64      0x1
#define	VDP_R16F_SCRH128     0x3

#define	VDP_R16F_SCRY32      0x0
#define	VDP_R16F_SCRY64      0x10
#define	VDP_R16F_SCRY128     0x30
#define	VDP_R16F_SCRX32      0x0
#define	VDP_R16F_SCRX64      0x1
#define	VDP_R16F_SCRX128     0x3

#define	VDP_R17F_WINRIGT     0x80
#define	VDP_R17B_WINRIGT     0x7

#define	VDP_R18F_WINDOWN     0x80
#define	VDP_R18B_WINDOWN     0x7

#define	VDP_R23F_DMACOPY      0x0
#define	VDP_R23F_DMAFILL     0x80
#define	VDP_R23F_DMAVDPVDP      0xC0

/*--------------------------------------------------------------------------*/

#define	VDP_DATA        	0xC00000
#define	VDP_CMD         	0xC00004
#define	VDP_HVCOUNT     	0xC00008
#define	VDP_RASTER      	VDP_HVCOUNT

/*VDP_HSCROLL_SIZE		2*2*224

/*--------------------------------------------------------------------------*/
/* character equates							    */

#define	CHARF_XFLIP		0x0800
#define	CHARB_XFLIP		0xb
#define	CHARF_HFLIP		0x0800
#define	CHARB_HFLIP		0xb

#define	CHARF_YFLIP		0x1000
#define	CHARB_YFLIP		0xc
#define	CHARF_VFLIP		0x1000
#define	CHARB_VFLIP		0xc

#define	CHARF_PAL0		0x0000
#define	CHARF_PAL1		0x2000
#define	CHARF_PAL2		0x4000
#define	CHARF_PAL3		0x6000

#define	CHARF_PRI		0x8000
#define	CHARB_PRI		0xf

/*--------------------------------------------------------------------------*/
/* sprite equates							    */

#define	SPRITEF_XFLIP		0x0800
#define	SPRITEB_XFLIP		0xb
#define	SPRITEF_HFLIP		0x0800
#define	SPRITEB_HFLIP		0xb

#define	SPRITEF_YFLIP		0x1000
#define	SPRITEB_YFLIP		0xc
#define	SPRITEF_VFLIP		0x1000
#define	SPRITEB_VFLIP		0xc

#define	SPRITEF_PAL0		0x0000
#define	SPRITEF_PAL1		0x2000
#define	SPRITEF_PAL2		0x4000
#define	SPRITEF_PAL3		0x6000

#define	SPRITEF_PRI		0x8000
#define	SPRITEB_PRI		0xf

#define	SPRITEATTR_SIZE

/*============================================================================*/
/* sound equates							      */

#define	PSG			0xc00011

#define	PSGB_FEEDBACK		0x2
#define	PSGF_FEEDBACK		0x4

#define	PSG_NOISELOW		0x0
#define	PSG_NOISEMIDDLE		0x1
#define	PSG_NOISEHIGH		0x2
#define	PSG_NOISETRACK3		0x3

/*==========================================================================*/
/* Z80 equates								    */

#define	Z80_CONTR		0xa11100
#define	Z80_RESET		0xa11200

#define	Z80B_BUSREQ		8
#define	Z80F_BUSREQ		1<<Z80B_BUSREQ

#define	Z80B_NORESET		8
#define	Z80F_NORESET		1<<Z80B_NORESET

#define	Z80_BASE		0xa00000

/*==========================================================================*/

enum
{
	RAMBASE = 0xFF0000,
	RAMSIZE = 0x10000
};

/*==========================================================================*/


