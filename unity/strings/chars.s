;
; Copyright (c) 2018 Anthony Beaucamp.
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

	.export _charBlank
	.export _charDigit
	.export _charLetter
	.export _charBracket
	.export _charColon
	.export _charComma
	.export _charDot
	.export _charDollar
	.export _charExclaim
	.export _charHyphen
	.export _charPlus
	.export _charQuestion
	.export _charQuote
	.export _charBwSlash
	.export _charFwSlash
	.export _charStar
	.export _charUnderbar
	
	.export _charArrowDown
	.export _charArrowLeft
	.export _charArrowRight
	.export _charArrowUp
	.export _charLineHorz
	.export _charLineVert
	.export _charSliderHorz
	.export _charSliderVert
	
	.export _charAmmo
	.export _charBlock
	.export _charDeath
	.export _charHeart
	.export _charPotion
	.export _charShield	

.segment "DATA"

; Icon Data
_charHeart:		
	.byte %00001010, %11101110, %11100100	; ASCII 1
_charPotion:
	.byte %00000100, %01001110, %11101110	; ASCII 2
_charShield:
	.byte %00001110, %11101110, %11100100	; ASCII 3
_charDeath:
	.byte %00000100, %11101110, %01000100	; ASCII 4
_charAmmo:
	.byte %10100000, %10101010, %10101010	; ASCII 5
_charBlock:
	.byte %00001110, %11101110, %11100000	; ASCII 6

; Text Data
_charBlank:
	.byte %00000000, %00000000, %00000000	; ASCII 32
_charExclaim:	
	.byte %01000100, %01000100, %00000100	; ASCII 33
_charDollar:	
	.byte %01001110, %11000110, %11100100	; ASCII 36
_charQuote:	
	.byte %01000100, %01000000, %00000000	; ASCII 39
_charBracket:	
	.byte %00100100, %10001000, %01000010	; ASCII 40	
	.byte %10000100, %00100010, %01001000	; ASCII 41
_charStar:
	.byte %00000100, %11101110, %01000000	; ASCII 42
_charPlus:
	.byte %00000100, %11101110, %01000000	; ASCII 43
_charComma:	
	.byte %00000000, %00000000, %01001000	; ASCII 44
_charHyphen:
	.byte %00000000, %00001110, %00000000	; ASCII 45
_charDot:	
	.byte %00000000, %00000000, %00000100	; ASCII 46
_charFwSlash:
	.byte %00100010, %01000100, %10001000	; ASCII 47
_charDigit: 
	.byte %01001010, %10101010, %10100100	; ASCII 48 (0)
	.byte %01101010, %00100010, %00100010	; ASCII 49 (1)
	.byte %01001010, %00100100, %10001110	; ASCII 50 (2)
	.byte %11000010, %01000100, %00101100	; ASCII 51 (3)
	.byte %10101010, %10101110, %00100010	; ASCII 52 (4)
	.byte %11101000, %10001110, %00101100	; ASCII 53 (5)
	.byte %01101000, %10001100, %10100100	; ASCII 54 (6)
	.byte %11100010, %01000100, %10001000	; ASCII 55 (7)
	.byte %01001010, %10100100, %10100100	; ASCII 56 (8)
	.byte %01001010, %10100110, %00101100	; ASCII 57 (9)
_charColon:	
	.byte %00000100, %00000000, %01000000	; ASCII 58
_charQuestion:
	.byte %11000010, %00100100, %00000100   ; ASCII 63
_charLetter: 
	.byte %01001010, %10101110, %10101010	; ASCII 65 (A)
	.byte %11001010, %10101100, %10101100	; B
	.byte %01101000, %10001000, %10000110	; C
	.byte %11001010, %10101010, %10101100	; D
	.byte %01101000, %11001100, %10000110	; E
	.byte %11101000, %10001100, %10001000	; F
	.byte %01101000, %10101010, %10100110	; G
	.byte %10101010, %11101110, %10101010	; H
	.byte %11100100, %01000100, %01001110	; I
	.byte %11100100, %01000100, %01001100	; J
	.byte %10101010, %11001100, %10101010	; K
	.byte %10001000, %10001000, %10001110	; L
	.byte %10101110, %11101010, %10101010	; M
	.byte %10101110, %11101110, %11101010	; N
	.byte %01101010, %10101010, %10101100	; O
	.byte %11001010, %10101110, %10001000	; P
	.byte %01001010, %10101010, %11000110	; Q
	.byte %11001010, %10101110, %11001010	; R
	.byte %01101000, %10000100, %00101100	; S
	.byte %11100100, %01000100, %01000100	; T
	.byte %10101010, %10101010, %10101110	; U
	.byte %10101010, %10101010, %10100100	; V
	.byte %10101010, %10101110, %11101010	; W
	.byte %10101010, %01000100, %10101010	; X
	.byte %10101010, %11100100, %01000100	; Y
	.byte %11100010, %01000100, %10001110	; Z
_charBwSlash:
	.byte %10001000, %01000100, %00100010   ; ASCII 92
_charUnderbar:
	.byte %00000000, %00000000, %00001110   ; ASCII 95
	
; Widget Data
_charArrowDown:
	.byte %01000100, %01000100, %11101110
_charArrowLeft:
	.byte %00001100, %11101110, %11000000	
_charArrowRight:
	.byte %00000110, %11101110, %01100000
_charArrowUp:
	.byte %11101110, %11100100, %01000100
_charLineHorz:
	.byte %00000000, %11101110, %00000000
_charLineVert:
	.byte %01000100, %01000100, %01000100
_charSliderHorz:
	.byte %00000111, %11101110, %11100000
_charSliderVert:
	.byte %01001110, %11101110, %11100100
