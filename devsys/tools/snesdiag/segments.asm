;==============================================================================
;	SEGMENTS.ASM --- Definition for all of the segments.
;==============================================================================




gfx1	defs	rom

gfx1	segment
	size	$8000
	org	$18000
	porg	$8000

gfx2	defs	rom

gfx2	segment
	size	$8000
	org	$28000
	porg	$10000


gfx3	defs	rom
gfx3	segment
	size	$8000
	org	$38000
	porg	$18000

gfx4	defs	rom
gfx4	segment
	size	$8000
	org	$48000
	porg	$20000

gfx5	defs	rom
gfx5	segment
	size	$8000
	org	$58000
	porg	$28000

gfx6	defs	rom
gfx6 	segment
	size	$8000
	org	$68000
	porg	$30000

gfx7	defs	rom
gfx7	segment
	size	$8000
	org	$78000
	porg	$38000

gfx8	defs	rom
gfx8	segment
	size	$8000
	org	$88000
	porg	$40000

gfx9	defs	rom
gfx9	segment
	size	$8000
	org	$98000
	porg	$48000

gfx10	defs	rom
gfx10	segment
	size	$8000
	org	$a8000
	porg	$50000
;	org	$108000
;	porg	$80000

gfx11	defs	rom
gfx11	segment
	size	$8000
	org	$b8000
	porg	$58000

gfx12	defs	rom
gfx12	segment
	size	$8000
	org	$c8000
	porg	$60000

gfx13	defs	rom
gfx13	segment
	size	$8000
	org	$d8000
	porg	$68000

gfx14	defs	rom
gfx14	segment
	size	$8000
	org	$e8000
	porg	$70000

gfx15	defs	rom
gfx15	segment
	size	$8000
	org	$F8000
	PORG	$78000

SRAM	DEFS	RAM
SRAM	SEGMENT
	SIZE	$2000
	ORG	$0

CODE	DEFS	ROM

CODE	SEGMENT
	size	$7600
	org	$8a00
	porg	$a00

;=============================================================================


ZPAGE	DEFS	RAM

ZPAGE	SEGMENT
	SIZE	$FF
	org	0

;=============================================================================



	end
