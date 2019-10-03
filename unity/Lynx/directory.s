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

	.import __BLOCKSIZE__
	.import __STARTOFDIRECTORY__

	.export _MAIN_FILENR : absolute
	.import __STARTUP_LOAD__
	.import __STARTUP_SIZE__
	.import __LOWCODE_SIZE__
	.import __ONCE_SIZE__
	.import __CODE_SIZE__
	.import __DATA_SIZE__
	.import __RODATA_SIZE__
	
	.segment "DIRECTORY"

__DIRECTORY_START__:

.macro entry old_off, old_len, new_off, new_block, new_len, new_size, new_addr
new_off=old_off+old_len
new_block=new_off/__BLOCKSIZE__
new_len=new_size
	.byte	<new_block
	.word	(new_off & (__BLOCKSIZE__ - 1))
	.byte	$88
	.word	new_addr
	.word	new_len
.endmacro

; Entry 0 - first executable
_MAIN_FILENR=0

entry __STARTOFDIRECTORY__+(__DIRECTORY_END__-__DIRECTORY_START__), 0, mainoff, mainblock, mainlen, __STARTUP_SIZE__+__LOWCODE_SIZE__+__ONCE_SIZE__+__CODE_SIZE__+__RODATA_SIZE__+__DATA_SIZE__, __STARTUP_LOAD__

__DIRECTORY_END__:
