;
; Copyright (c) 2020 Anthony Beaucamp.
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
;	Original implementation by Christophe Meneboeuf
;

	.import     __dos_type
	.import     popa

	.export _mli

.code

_mli:
        ; Store parameters
        STA     Mli_Param
        STX     Mli_Param+1
        ; Store call number
        JSR     popa
        STA     Mli_Call

        ; Call MLI and return
        JSR     $BF00           ; MLI call entry point
		
Mli_Call:
        .byte   $00
Mli_Param:
        .addr   $00
        rts
		
		