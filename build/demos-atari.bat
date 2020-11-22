echo off

mkdir atari
cd ..

del build\atari\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

utils\py27\python utils\scripts\atari\AtariBitmap.py projects/demos/bitmaps/banner-atari.png build/atari/banner.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/demos/bitmaps/stadium-atari.png build/atari/stadium.img
utils\py27\python utils\scripts\atari\AtariSprites.py projects/demos/sprites/sprites-atari.png build/atari/sprites.dat 13
copy projects\demos\music\demo-atari.rmt build\atari\demo.mus

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\bitmap.c
utils\cc65\bin\ca65 -t atarixl unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\charmap.c
utils\cc65\bin\ca65 -t atarixl unity\charmap.s
del unity\charmap.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\chunks.c
utils\cc65\bin\ca65 -t atarixl unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\geom2d.c
utils\cc65\bin\ca65 -t atarixl unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\joystick.c
utils\cc65\bin\ca65 -t atarixl unity\joystick.s
del unity\joystick.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\mouse.c
utils\cc65\bin\ca65 -t atarixl unity\mouse.s
del unity\mouse.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\music.c
utils\cc65\bin\ca65 -t atarixl unity\music.s
del unity\music.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\net-base.c
utils\cc65\bin\ca65 -t atarixl unity\net-base.s
del unity\net-base.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\net-url.c
utils\cc65\bin\ca65 -t atarixl unity\net-url.s
del unity\net-url.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\net-tcp.c
utils\cc65\bin\ca65 -t atarixl unity\net-tcp.s
del unity\net-tcp.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\net-udp.c
utils\cc65\bin\ca65 -t atarixl unity\net-udp.s
del unity\net-udp.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\net-web.c
utils\cc65\bin\ca65 -t atarixl unity\net-web.s
del unity\net-web.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\pixel.c
utils\cc65\bin\ca65 -t atarixl unity\pixel.s
del unity\pixel.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\print.c
utils\cc65\bin\ca65 -t atarixl unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\scaling.c
utils\cc65\bin\ca65 -t atarixl unity\scaling.s
del unity\scaling.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\sfx.c
utils\cc65\bin\ca65 -t atarixl unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\sprites.c
utils\cc65\bin\ca65 -t atarixl unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\widgets.c
utils\cc65\bin\ca65 -t atarixl unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\Atari\directory.c
utils\cc65\bin\ca65 -t atarixl unity\Atari\directory.s
del unity\Atari\directory.s
utils\cc65\bin\cc65 -Cl -O -t atarixl -I unity unity\Atari\files.c
utils\cc65\bin\ca65 -t atarixl unity\Atari\files.s
del unity\Atari\files.s
utils\cc65\bin\ca65 -t atarixl unity\atan2.s
utils\cc65\bin\ca65 -t atarixl unity\chars.s
utils\cc65\bin\ca65 -t atarixl unity\tiles.s
utils\cc65\bin\ca65 -t atarixl unity\Atari\DLI.s
utils\cc65\bin\ca65 -t atarixl unity\Atari\ROM.s
utils\cc65\bin\ca65 -t atarixl unity\Atari\scroll.s
utils\cc65\bin\ca65 -t atarixl unity\Atari\xbios.s
utils\cc65\bin\ar65 r build/atari/unity.lib unity\bitmap.o unity\charmap.o unity\chunks.o unity\geom2d.o unity\joystick.o unity\mouse.o unity\music.o unity\net-base.o unity\net-url.o unity\net-tcp.o unity\net-udp.o unity\net-web.o unity\pixel.o unity\print.o unity\scaling.o unity\sfx.o unity\sprites.o unity\widgets.o unity\Atari\directory.o unity\Atari\files.o unity\atan2.o unity\chars.o unity\tiles.o unity\Atari\DLI.o unity\Atari\ROM.o unity\Atari\scroll.o unity\Atari\xbios.o 
utils\cc65\bin\cl65 -o build/atari/demos.bin -m build/demos-atari.map -Cl -O -t atarixl -Wl -D,__STACKSIZE__=$0400 -C atarixl-largehimem.cfg -I unity projects/demos/src/demo.c projects/demos/src/demo-bitmap.c projects/demos/src/demo-controls.c projects/demos/src/demo-gfx.c projects/demos/src/demo-hello.c projects/demos/src/demo-network.c projects/demos/src/demo-sprites.c unity/Atari/POKEY.s build/atari/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_atarixl.lib
utils\cc65\bin\cl65 -t atarixl -C atari-asm.cfg -o build/atari/basicoff.bin unity/Atari/BASICOFF.s
utils\scripts\atari\mads.exe -o:build/atari/rmt.bin unity/Atari/RMT.a65

utils\py27\python utils\scripts\atari\AtariMerge.py build/atari/xautorun build/atari/basicoff.bin build/atari/demos.bin build/atari/rmt.bin

echo DONE!

echo --------------- ATARI DISK BUILDER --------------- 

del build\atari\*.bin
del build\atari\*.lib
del build\atari\*.lst
copy utils\scripts\atari\xbios.com build\atari\autorun
copy utils\scripts\atari\xbios.cfg build\atari\xbios.cfg
utils\scripts\atari\dir2atr.exe -d -B utils/scripts/atari/xboot.obx 720 build/demos-atari.atr build\atari

echo DONE
pause

cd "utils\emulators\Altirra-3.20"
Altirra.exe "..\..\..\build\demos-atari.atr"
