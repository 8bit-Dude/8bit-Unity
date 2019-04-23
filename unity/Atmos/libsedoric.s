;
; Generated with ap65 v 1.00
;
.setcpu     "65816"
.smart      on
.autoimport on
.case       on
.importzp   sp, sreg, regsave, regbank
.importzp   tmp1, tmp2, tmp3, tmp4
.importzp   ptr1, ptr2, ptr3, ptr4
;
_exvec =  $02f5
_dosrom =  $04f2
_dosflag =  $04fc
_doserr =  $04fd
.code
_sed_fname: .byte 0,0
_sed_begin: .byte 0,0
_sed_end: .byte 0,0
_sed_size: .byte 0,0
_sed_err: .byte 0,0
savebuf_zp: .res 256
_sed_savefile:

tya
pha
jsr sed_savezp
lda _sed_fname
sta $e9
lda _sed_fname+1
sta $ea
jsr _dosrom
lda #1
sta $0b
lda #$00
sta $c018
clc
lda #$00
jsr $d454
lda _sed_begin
sta $c052
lda _sed_begin+1
sta $c053
lda _sed_end
sta $c054
lda _sed_end+1
sta $c055
lda _sed_begin
sta $c056
lda _sed_begin+1
sta $c057
lda #$00
sta $c04d
lda #$40
sta $c04e
lda #$40
sta $c051
jsr $de0b
jsr _dosrom
jsr sed_restorezp
lda _doserr
sta _sed_err
lda _doserr+1
sta _sed_err+1
pla
tay
rts

_sed_loadfile:

tya
pha
jsr sed_savezp
lda _sed_fname
sta $e9
lda _sed_fname+1
sta $ea
jsr _dosrom
lda #1
sta $0b
lda #$00
sta $c018
clc
lda #$00
jsr $d454
lda _sed_begin
sta $c052
lda _sed_begin+1
sta $c053
lda #$00
sta $c04d
lda #$40
sta $c04e
jsr $e0e5
lda $c052
sta _sed_begin
lda $c053
sta _sed_begin+1
clc
lda $c04f
sta _sed_size
adc _sed_begin
sta _sed_end
lda $c050
sta _sed_size+1
adc _sed_begin+1
sta _sed_end+1
jsr _dosrom
jsr sed_restorezp
lda _doserr
sta _sed_err
lda _doserr+1
sta _sed_err+1
pla
tay

sed_savezp:

ldx #00
L_139_loop:
lda $00,x
sta savebuf_zp,x
dex
bne L_139_loop
rts

sed_restorezp:

ldx #00
L_150_loop:
lda savebuf_zp,x
sta $00,x
dex
bne L_150_loop
rts

_cls:
jmp $ccce
.export _exvec
.export _dosrom
.export _dosflag
.export _doserr
.export _sed_fname
.export _sed_begin
.export _sed_end
.export _sed_size
.export _sed_err
.export _sed_savefile
.export _sed_loadfile
.export _cls
