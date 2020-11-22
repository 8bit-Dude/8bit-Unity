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

copy ..\..\projects\os\bitmaps\hokusai-lynx.png hokusai.png
..\..\utils\scripts\png2bmp hokusai.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u hokusai.bmp
copy ..\..\projects\os\bitmaps\munch-lynx.png munch.png
..\..\utils\scripts\png2bmp munch.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u munch.bmp
copy ..\..\projects\os\bitmaps\picasso-lynx.png picasso.png
..\..\utils\scripts\png2bmp picasso.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u picasso.bmp
copy ..\..\projects\os\bitmaps\vangogh-lynx.png vangogh.png
..\..\utils\scripts\png2bmp vangogh.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u vangogh.bmp

copy ..\..\projects\os\sprites\sprites-lynx.png sprites.png
..\..\utils\scripts\png2bmp sprites.png
..\..\utils\scripts\lynx\sprpck -t6 -p2 -u -r001002 -S009013 -a004006 sprites.bmp

set /a CHUNKNUM=0
..\..\utils\py27\python ..\..\utils\scripts\ProcessChunks.py lynx ../../projects/os/chunks/chunks-lynx.txt ../../build/lynx/
for /f "tokens=*" %%A in (chunks.lst) do set CHUNKNAMES=!CHUNKNAMES!_shkName!CHUNKNUM!,&&set /a CHUNKNUM+=1
set /a FILENUM=!CHUNKNUM!+10
copy ..\..\unity\Lynx\chipper.s soundbs.mac
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/os/music/chase-lynx.asm music00.asm _musData00 MUS0DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/os/music/driven-lynx.asm music01.asm _musData01 MUS1DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/os/music/speednik-lynx.asm music02.asm _musData02 MUS2DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/os/music/stroll-lynx.asm music03.asm _musData03 MUS3DATA"
..\..\utils\py27\python ../../utils/scripts/lynx/LynxChipper.py ../../projects/os/music/whirlwnd-lynx.asm music04.asm _musData04 MUS4DATA"
del *.png /F /Q
del *.bmp /F /Q
del *.pal /F /Q

copy ..\..\projects\os\shared\readme.txt readme.txt

set FILESIZES=
for %%I in (hokusai.spr,munch.spr,picasso.spr,vangogh.spr,music00.asm,music01.asm,music02.asm,music03.asm,music04.asm,readme.txt) do set FILESIZES=!FILESIZES!%%~zI,

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
@echo _fileNames: .addr _bmpName00,_bmpName01,_bmpName02,_bmpName03,_musName00,_musName01,_musName02,_musName03,_musName04,_shrName00,%CHUNKNAMES:~0,-1% >> data.asm
@echo _bmpName00: .byte "hokusai.img",0 >> data.asm
@echo _bmpName01: .byte "munch.img",0 >> data.asm
@echo _bmpName02: .byte "picasso.img",0 >> data.asm
@echo _bmpName03: .byte "vangogh.img",0 >> data.asm
@echo _musName00: .byte "chase.mus",0 >> data.asm
@echo _musName01: .byte "driven.mus",0 >> data.asm
@echo _musName02: .byte "speednik.mus",0 >> data.asm
@echo _musName03: .byte "stroll.mus",0 >> data.asm
@echo _musName04: .byte "whirlwnd.mus",0 >> data.asm
@echo _shrName00: .byte "readme.txt",0 >> data.asm
set /a IND=0
for /f "tokens=*" %%A in (chunks.lst) do @echo _shkName!IND!: .byte "%%~nxA",0 >> data.asm && set /a IND+=1
@echo ; >> data.asm
@echo .segment "BMP0DATA" >> data.asm
@echo _bmpData00: .incbin "hokusai.spr" >> data.asm
@echo .segment "BMP1DATA" >> data.asm
@echo _bmpData01: .incbin "munch.spr" >> data.asm
@echo .segment "BMP2DATA" >> data.asm
@echo _bmpData02: .incbin "picasso.spr" >> data.asm
@echo .segment "BMP3DATA" >> data.asm
@echo _bmpData03: .incbin "vangogh.spr" >> data.asm
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
@echo _shrData00: .incbin "readme.txt" >> data.asm
set /a IND=0
for /f "tokens=*" %%A in (chunks.lst) do @echo .segment "SHK!IND!DATA" >> data.asm && @echo _shkData!IND!: .incbin "%%~nxA" >> data.asm && set /a IND+=1
@echo ; >> data.asm
@echo .segment "RODATA" >> data.asm
@echo _spriteNum: .byte 2 >> data.asm
@echo _spriteData: .addr _spr000, _spr001 >> data.asm
@echo _spr000: .incbin "sprites000000.spr" >> data.asm
@echo _spr001: .incbin "sprites001000.spr" >> data.asm
@echo ; >> data.asm
@echo _cursorData: .incbin "cursor.spr" >> data.asm
@echo _keybrdData: .incbin "keyboard.spr" >> data.asm

cd ..
cd ..

utils\py27\python utils/scripts/lynx/LynxConfig.py unity/Lynx/lynx.cfg build/lynx/lynx.cfg 4 5 1 %CHUNKNUM%
utils\py27\python utils/scripts/lynx/LynxDirectory.py unity/Lynx/directory.s build/lynx/directory.asm 4 5 1 %CHUNKNUM%

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
utils\cc65\bin\cl65 -o build/os-lynx.lnx -m build/os-lynx.map -Cl -O -t lynx -C build/lynx/lynx.cfg -I unity projects/os/src/os.c projects/os/src/chat.c projects/os/src/files.c projects/os/src/home.c projects/os/src/image.c projects/os/src/mouse.c projects/os/src/music.c build/lynx/music00.asm build/lynx/music01.asm build/lynx/music02.asm build/lynx/music03.asm build/lynx/music04.asm unity/Lynx/sfx.s build/lynx/directory.asm build/lynx/data.asm build/lynx/unity.lib

echo DONE!

pause

cd "utils\emulators\Handy-0.98-Hub"
handy.exe "..\..\..\build\os-lynx.lnx"
