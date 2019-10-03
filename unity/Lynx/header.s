;
; Copyright (c) 2019 Anthony Beaucamp.
;
; This software is provided 'as-is', without any express or implied warranty.
; In no event will the authors be held liable for any damages arising from
; the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
;   1. The origin of this software must not be misrepresented; you must not
;   claim that you wrote the original software. If you use this software in a
;   product, an acknowledgment in the product documentation would be
;   appreciated but is not required.
;
;   2. Altered source versions must be plainly marked as such, and must not
;   be misrepresented as being the original software.
;
;   3. This notice may not be removed or altered from any distribution.
;
;   4. The names of this software and/or it's copyright holders may not be
;   used to endorse or promote products derived from this software without
;   specific prior written permission.
;
; This header contains data for emulators like Handy and Mednafen
;

	.import		__BLOCKSIZE__
	.export		__EXEHDR__: absolute = 1

; ------------------------------------------------------------------------
; EXE header
	.segment "EXEHDR"
	.byte	'L','Y','N','X'				; magic
	.word	__BLOCKSIZE__				; bank 0 page size
	.word	0					; bank 1 page size
	.word	1					; version number
	.asciiz	"8BIT UNITY                     "	; 32 bytes cart name
	.asciiz	"8BIT UNITY     "			; 16 bytes manufacturer 
	.byte	0					; rotation 1=left
							; rotation 2=right
        .byte   0                                       ; aud bits 1=in use for addressing
        .byte   0                                       ; eeprom -> use 1 for the most common eeprom, 64 to use SD Cart saves, 65 for both
                                                        ; eeprom [2:0] -
                                                        ; 0 - no eeprom
                                                        ; 1 - 93c46 16 bit mode (used in Ttris, SIMIS, Alpine Games, ..., MegaPak I at least)
                                                        ; 2        56
                                                        ; 3 - 93c66 16 bit mode
                                                        ; 4        76
                                                        ; 5 - 93c86 16 bit mode
                                                        ; (remark: size in bits is 2^(value+9) -- (please recheck!)
                                                        ; eeprom [3-5] - reserved - keep it to 0 for further usage
                                                        ; eeprom [6] - 0 - real eeprom, 1 - eeprom as a file in /saves/flappy.hi on SD cart
                                                        ; eeprom [7] - 0 - 16 bit mode, 1 - 8 bit mode
        .byte   0,0,0                                   ; spare
