echo off

setlocal enableextensions enabledelayedexpansion

mkdir lynx
cd lynx
del *.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

copy ..\..\utils\scripts\lynx\cursor.png cursor.png
..\..\utils\scripts\png2bmp cursor.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u cursor.bmp

copy ..\..\utils\scripts\lynx\keyboard.png keyboard.png
..\..\utils\scripts\png2bmp keyboard.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u keyboard.bmp

copy ..\..\projects\slicks\bitmaps\menu-lynx.png menu.png
..\..\utils\scripts\png2bmp menu.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u menu.bmp
copy ..\..\projects\slicks\bitmaps\arizona-lynx.png arizona.png
..\..\utils\scripts\png2bmp arizona.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u arizona.bmp
copy ..\..\projects\slicks\bitmaps\arto-lynx.png arto.png
..\..\utils\scripts\png2bmp arto.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u arto.bmp
copy ..\..\projects\slicks\bitmaps\cramp-lynx.png cramp.png
..\..\utils\scripts\png2bmp cramp.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u cramp.bmp
copy ..\..\projects\slicks\bitmaps\freeway-lynx.png freeway.png
..\..\utils\scripts\png2bmp freeway.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u freeway.bmp
copy ..\..\projects\slicks\bitmaps\gta-lynx.png gta.png
..\..\utils\scripts\png2bmp gta.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u gta.bmp
copy ..\..\projects\slicks\bitmaps\island-lynx.png island.png
..\..\utils\scripts\png2bmp island.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u island.bmp
copy ..\..\projects\slicks\bitmaps\mtcarlo-lynx.png mtcarlo.png
..\..\utils\scripts\png2bmp mtcarlo.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u mtcarlo.bmp
copy ..\..\projects\slicks\bitmaps\rally-lynx.png rally.png
..\..\utils\scripts\png2bmp rally.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u rally.bmp
copy ..\..\projects\slicks\bitmaps\river-lynx.png river.png
..\..\utils\scripts\png2bmp river.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u river.bmp
copy ..\..\projects\slicks\bitmaps\stadium-lynx.png stadium.png
..\..\utils\scripts\png2bmp stadium.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u stadium.bmp

copy ..\..\projects\slicks\sprites\sprites-lynx.png sprites.png
..\..\utils\scripts\png2bmp sprites.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u -r001018 -S007009 -a003004 sprites.bmp

set /a CHUNKNUM=0
set /a FILENUM=!CHUNKNUM!+26
copy ..\..\unity\Lynx\chipper.s soundbs.mac
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/slicks/music/speednik-lynx.asm music00.asm _musData00 MUS0DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/slicks/music/chase-lynx.asm music01.asm _musData01 MUS1DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/slicks/music/driven-lynx.asm music02.asm _musData02 MUS2DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/slicks/music/stroll-lynx.asm music03.asm _musData03 MUS3DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/slicks/music/whirlwnd-lynx.asm music04.asm _musData04 MUS4DATA"
del *.png /F /Q
del *.bmp /F /Q
del *.pal /F /Q

copy ..\..\projects\slicks\navigation\arizona.nav arizona.nav
copy ..\..\projects\slicks\navigation\arto.nav arto.nav
copy ..\..\projects\slicks\navigation\cramp.nav cramp.nav
copy ..\..\projects\slicks\navigation\freeway.nav freeway.nav
copy ..\..\projects\slicks\navigation\gta.nav gta.nav
copy ..\..\projects\slicks\navigation\island.nav island.nav
copy ..\..\projects\slicks\navigation\mtcarlo.nav mtcarlo.nav
copy ..\..\projects\slicks\navigation\rally.nav rally.nav
copy ..\..\projects\slicks\navigation\river.nav river.nav
copy ..\..\projects\slicks\navigation\stadium.nav stadium.nav

set FILESIZES=
for %%I in (menu.spr,arizona.spr,arto.spr,cramp.spr,freeway.spr,gta.spr,island.spr,mtcarlo.spr,rally.spr,river.spr,stadium.spr,music00.asm,music01.asm,music02.asm,music03.asm,music04.asm,arizona.nav,arto.nav,cramp.nav,freeway.nav,gta.nav,island.nav,mtcarlo.nav,rally.nav,river.nav,stadium.nav) do set FILESIZES=!FILESIZES!%%~zI,

for /f "tokens=*" %%A in (chunks.lst) do set FILESIZES=!FILESIZES!%%~zA,
@echo .global _fileNum  >> data.asm
@echo .global _fileSizes >> data.asm
@echo .global _fileNames >> data.asm
@echo .global _spriteNum  >> data.asm
@echo .global _spriteData >> data.asm
@echo .global _cursorData >> data.asm
@echo .global _keybrdData >> data.asm
@echo ; >> data.asm
@echo .segment "RODATA" >> data.asm
@echo _fileNum: .byte %FILENUM% >> data.asm
@echo _fileSizes: .word %FILESIZES:~0,-1% >> data.asm
@echo _fileNames: .addr _bmpName00,_bmpName01,_bmpName02,_bmpName03,_bmpName04,_bmpName05,_bmpName06,_bmpName07,_bmpName08,_bmpName09,_bmpName10,_musName00,_musName01,_musName02,_musName03,_musName04,_shrName00,_shrName01,_shrName02,_shrName03,_shrName04,_shrName05,_shrName06,_shrName07,_shrName08,_shrName09 >> data.asm
@echo _bmpName00: .byte "menu.img",0 >> data.asm
@echo _bmpName01: .byte "arizona.img",0 >> data.asm
@echo _bmpName02: .byte "arto.img",0 >> data.asm
@echo _bmpName03: .byte "cramp.img",0 >> data.asm
@echo _bmpName04: .byte "freeway.img",0 >> data.asm
@echo _bmpName05: .byte "gta.img",0 >> data.asm
@echo _bmpName06: .byte "island.img",0 >> data.asm
@echo _bmpName07: .byte "mtcarlo.img",0 >> data.asm
@echo _bmpName08: .byte "rally.img",0 >> data.asm
@echo _bmpName09: .byte "river.img",0 >> data.asm
@echo _bmpName10: .byte "stadium.img",0 >> data.asm
@echo _musName00: .byte "speednik.mus",0 >> data.asm
@echo _musName01: .byte "chase.mus",0 >> data.asm
@echo _musName02: .byte "driven.mus",0 >> data.asm
@echo _musName03: .byte "stroll.mus",0 >> data.asm
@echo _musName04: .byte "whirlwnd.mus",0 >> data.asm
@echo _shrName00: .byte "arizona.nav",0 >> data.asm
@echo _shrName01: .byte "arto.nav",0 >> data.asm
@echo _shrName02: .byte "cramp.nav",0 >> data.asm
@echo _shrName03: .byte "freeway.nav",0 >> data.asm
@echo _shrName04: .byte "gta.nav",0 >> data.asm
@echo _shrName05: .byte "island.nav",0 >> data.asm
@echo _shrName06: .byte "mtcarlo.nav",0 >> data.asm
@echo _shrName07: .byte "rally.nav",0 >> data.asm
@echo _shrName08: .byte "river.nav",0 >> data.asm
@echo _shrName09: .byte "stadium.nav",0 >> data.asm
@echo ; >> data.asm
@echo .segment "BMP0DATA" >> data.asm
@echo _bmpData00: .incbin "menu.spr" >> data.asm
@echo .segment "BMP1DATA" >> data.asm
@echo _bmpData01: .incbin "arizona.spr" >> data.asm
@echo .segment "BMP2DATA" >> data.asm
@echo _bmpData02: .incbin "arto.spr" >> data.asm
@echo .segment "BMP3DATA" >> data.asm
@echo _bmpData03: .incbin "cramp.spr" >> data.asm
@echo .segment "BMP4DATA" >> data.asm
@echo _bmpData04: .incbin "freeway.spr" >> data.asm
@echo .segment "BMP5DATA" >> data.asm
@echo _bmpData05: .incbin "gta.spr" >> data.asm
@echo .segment "BMP6DATA" >> data.asm
@echo _bmpData06: .incbin "island.spr" >> data.asm
@echo .segment "BMP7DATA" >> data.asm
@echo _bmpData07: .incbin "mtcarlo.spr" >> data.asm
@echo .segment "BMP8DATA" >> data.asm
@echo _bmpData08: .incbin "rally.spr" >> data.asm
@echo .segment "BMP9DATA" >> data.asm
@echo _bmpData09: .incbin "river.spr" >> data.asm
@echo .segment "BMP10DATA" >> data.asm
@echo _bmpData10: .incbin "stadium.spr" >> data.asm
@echo ; >> data.asm
@echo ; >> data.asm
@echo .segment "MUS0DATA" >> data.asm
@echo .import _musData00 >> data.asm
@echo .segment "MUS1DATA" >> data.asm
@echo .import _musData01 >> data.asm
@echo .segment "MUS2DATA" >> data.asm
@echo .import _musData02 >> data.asm
@echo .segment "MUS3DATA" >> data.asm
@echo .import _musData03 >> data.asm
@echo .segment "MUS4DATA" >> data.asm
@echo .import _musData04 >> data.asm
@echo .segment "SHR0DATA" >> data.asm
@echo _shrData00: .incbin "arizona.nav" >> data.asm
@echo .segment "SHR1DATA" >> data.asm
@echo _shrData01: .incbin "arto.nav" >> data.asm
@echo .segment "SHR2DATA" >> data.asm
@echo _shrData02: .incbin "cramp.nav" >> data.asm
@echo .segment "SHR3DATA" >> data.asm
@echo _shrData03: .incbin "freeway.nav" >> data.asm
@echo .segment "SHR4DATA" >> data.asm
@echo _shrData04: .incbin "gta.nav" >> data.asm
@echo .segment "SHR5DATA" >> data.asm
@echo _shrData05: .incbin "island.nav" >> data.asm
@echo .segment "SHR6DATA" >> data.asm
@echo _shrData06: .incbin "mtcarlo.nav" >> data.asm
@echo .segment "SHR7DATA" >> data.asm
@echo _shrData07: .incbin "rally.nav" >> data.asm
@echo .segment "SHR8DATA" >> data.asm
@echo _shrData08: .incbin "river.nav" >> data.asm
@echo .segment "SHR9DATA" >> data.asm
@echo _shrData09: .incbin "stadium.nav" >> data.asm
@echo ; >> data.asm
@echo .segment "RODATA" >> data.asm
@echo _spriteNum: .byte 18 >> data.asm
@echo _spriteData: .addr _spr000, _spr001, _spr002, _spr003, _spr004, _spr005, _spr006, _spr007, _spr008, _spr009, _spr010, _spr011, _spr012, _spr013, _spr014, _spr015, _spr016, _spr017 >> data.asm
@echo _spr000: .incbin "sprites000000.spr" >> data.asm
@echo _spr001: .incbin "sprites001000.spr" >> data.asm
@echo _spr002: .incbin "sprites002000.spr" >> data.asm
@echo _spr003: .incbin "sprites003000.spr" >> data.asm
@echo _spr004: .incbin "sprites004000.spr" >> data.asm
@echo _spr005: .incbin "sprites005000.spr" >> data.asm
@echo _spr006: .incbin "sprites006000.spr" >> data.asm
@echo _spr007: .incbin "sprites007000.spr" >> data.asm
@echo _spr008: .incbin "sprites008000.spr" >> data.asm
@echo _spr009: .incbin "sprites009000.spr" >> data.asm
@echo _spr010: .incbin "sprites010000.spr" >> data.asm
@echo _spr011: .incbin "sprites011000.spr" >> data.asm
@echo _spr012: .incbin "sprites012000.spr" >> data.asm
@echo _spr013: .incbin "sprites013000.spr" >> data.asm
@echo _spr014: .incbin "sprites014000.spr" >> data.asm
@echo _spr015: .incbin "sprites015000.spr" >> data.asm
@echo _spr016: .incbin "sprites016000.spr" >> data.asm
@echo _spr017: .incbin "sprites017000.spr" >> data.asm
@echo ; >> data.asm
@echo _cursorData: .incbin "cursor.spr" >> data.asm
@echo _keybrdData: .incbin "keyboard.spr" >> data.asm

cd ..
cd ..

utils\py27\python utils/scripts/lynx/LynxConfig.py unity/Lynx/lynx.cfg build/lynx/lynx.cfg 11 5 10 %CHUNKNUM%
utils\py27\python utils/scripts/lynx/LynxDirectory.py unity/Lynx/directory.s build/lynx/directory.asm 11 5 10 %CHUNKNUM%

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\bitmap.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\charmap.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\charmap.s
del unity\charmap.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\chunks.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\geom2d.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\hub.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\hub.s
del unity\hub.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\joystick.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\joystick.s
del unity\joystick.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\mouse.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\mouse.s
del unity\mouse.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\music.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\music.s
del unity\music.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\net-base.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\net-base.s
del unity\net-base.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\net-url.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\net-url.s
del unity\net-url.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\net-tcp.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\net-tcp.s
del unity\net-tcp.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\net-udp.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\net-udp.s
del unity\net-udp.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\net-web.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\net-web.s
del unity\net-web.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\pixel.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\pixel.s
del unity\pixel.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\print.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\scaling.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\scaling.s
del unity\scaling.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\sfx.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\sprites.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\widgets.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\Lynx\display.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\display.s
del unity\Lynx\display.s
utils\cc65\bin\cc65 -Cl -O -t lynx -I unity unity\Lynx\files.c
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\files.s
del unity\Lynx\files.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\atan2.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\chars.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\tiles.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\header.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\scroll.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\serial.s
utils\cc65\bin\ca65 -t lynx --cpu 65SC02 unity\Lynx\suzy.s
utils\cc65\bin\ar65 r build/lynx/unity.lib unity\bitmap.o unity\charmap.o unity\chunks.o unity\geom2d.o unity\hub.o unity\joystick.o unity\mouse.o unity\music.o unity\net-base.o unity\net-url.o unity\net-tcp.o unity\net-udp.o unity\net-web.o unity\pixel.o unity\print.o unity\scaling.o unity\sfx.o unity\sprites.o unity\widgets.o unity\Lynx\display.o unity\Lynx\files.o unity\atan2.o unity\chars.o unity\tiles.o unity\Lynx\header.o unity\Lynx\scroll.o unity\Lynx\serial.o unity\Lynx\suzy.o 
utils\cc65\bin\cl65 -o build/slicks-lynx.lnx -m build/slicks-lynx.map -Cl -O -t lynx -C build/lynx/lynx.cfg -I unity projects/slicks/src/slicks.c projects/slicks/src/game.c projects/slicks/src/interface.c projects/slicks/src/navigation.c projects/slicks/src/network.c build/lynx/music00.asm build/lynx/music01.asm build/lynx/music02.asm build/lynx/music03.asm build/lynx/music04.asm unity/Lynx/sfx.s build/lynx/directory.asm build/lynx/data.asm build/lynx/unity.lib

echo DONE!

