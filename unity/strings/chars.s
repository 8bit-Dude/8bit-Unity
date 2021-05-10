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

	.export _charData

.segment "DATA"

_charData:

; Widget Data
_charArrowDown:
	.byte %01000100, %01000100, %11101110	; CUSTOM 18
_charArrowLeft:
	.byte %00001100, %11101110, %11000000	; CUSTOM 19
_charArrowRight:
	.byte %00000110, %11101110, %01100000	; CUSTOM 20
_charArrowUp:
	.byte %11101110, %11100100, %01000100	; CUSTOM 21
_charLineHorz:
	.byte %00000000, %11101110, %00000000	; CUSTOM 22
_charLineVert:
	.byte %01000100, %01000100, %01000100	; CUSTOM 23
_charSliderHorz:
	.byte %00000111, %11101110, %11100000	; CUSTOM 24
_charSliderVert:
	.byte %01001110, %11101110, %11100100	; CUSTOM 25
	
; Icon Data
_charHeart:		
	.byte %00001010, %11101110, %11100100	; CUSTOM 26
_charPotion:
	.byte %00000100, %01001110, %11101110	; CUSTOM 27
_charShield:
	.byte %00001110, %11101110, %11100100	; CUSTOM 28
_charDeath:
	.byte %00000100, %11101110, %01000100	; CUSTOM 29 
_charAmmo:
	.byte %10100000, %10101010, %10101010	; CUSTOM 30
_charBlock:
	.byte %00001110, %11101110, %11100000	; CUSTOM 31
	
; ASCII Data
_charBlank:
	.byte %00000000, %00000000, %00000000	; ASCII 32
_charExclaim:	
	.byte %01000100, %01000100, %00000100	; ASCII 33
_charQuotes:	
	.byte %10101010, %10100000, %00000000	; ASCII 34
_charHash:	
	.byte %10101110, %10101010, %11101010	; ASCII 35
_charDollar:	
	.byte %01001110, %11000110, %11100100	; ASCII 36
_charPercent:	
	.byte %01001110, %11000110, %11100100	; ASCII 37
_charAmpersand:	
	.byte %01001010, %10100101, %10100101	; ASCII 38
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
_charSemiColon:	
	.byte %00000100, %00000000, %01001000	; ASCII 59
_charLessThan:	
	.byte %00000010, %01001000, %01000010	; ASCII 60
_charEqual:	
	.byte %00001110, %00000000, %11100000	; ASCII 61
_charMoreThan:	
	.byte %00001000, %01000010, %01001000	; ASCII 62
_charQuestion:
	.byte %11000010, %00100100, %00000100   ; ASCII 63
_charAt:
	.byte %01001010, %10101010, %10100100   ; ASCII 64
_charLetter: 
	.byte %01001010, %10101110, %10101010	; A (ASCII 65)
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
	.byte %11100010, %01000100, %10001110	; Z (ASCII 90)
_charSqrBracket1:
	.byte %01100100, %01000100, %01000110   ; ASCII 91
_charBwSlash:
	.byte %10001000, %01000100, %00100010   ; ASCII 92
_charSqrBracket2:
	.byte %11000100, %01000100, %01001100   ; ASCII 93
_charHat:
	.byte %01001010, %00000000, %00000000   ; ASCII 94
_charUnderbar:
	.byte %00000000, %00000000, %00001110   ; ASCII 95
