; Regression repro for WLA-DX issue #619 -- ROM usage report over 100%.
;
; A full-ROM .BACKGROUND marks every byte of the usage map as used, so the
; verbose ROM-usage summary already reads 100% before sections are counted.
; An OVERWRITE section on top of it was then double-counted and pushed the
; reported figure past 100% (the original report showed 100.61%).
;
; This mirrors the issue's SMS/Z80 setup but with a small 2-bank ROM so the
; generated .BACKGROUND file stays tiny (32 KB). The bug is in shared
; phase_4.c, so the CPU target is irrelevant.
;
; Pass condition: assembling with -v must not print any ROM bank or ROM
; total line above 100.00% used. See verify.sh.

.memorymap
slotsize $4000
slot 0 $0000
slot 1 $4000
defaultslot 1
.endme

.rombankmap
bankstotal 2
banksize $4000
banks 2
.endro

.background "background.bin"

.org $1000
.section "Overwrite section" overwrite
.dsb 100 0
.ends
