echo off

mkdir atari
cd ..

del build\atari\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/menu-atari.png build/atari/menu.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/arizona-atari.png build/atari/arizona.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/arto-atari.png build/atari/arto.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/cramp-atari.png build/atari/cramp.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/freeway-atari.png build/atari/freeway.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/gta-atari.png build/atari/gta.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/island-atari.png build/atari/island.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/mtcarlo-atari.png build/atari/mtcarlo.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/rally-atari.png build/atari/rally.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/river-atari.png build/atari/river.img
utils\py27\python utils\scripts\atari\AtariBitmap.py projects/slicks/bitmaps/stadium-atari.png build/atari/stadium.img
utils\py27\python utils\scripts\atari\AtariSprites.py projects/slicks/sprites/sprites-atari.png build/atari/sprites.dat 10
copy projects\slicks\navigation\arizona.nav build\atari
copy projects\slicks\navigation\arto.nav build\atari
copy projects\slicks\navigation\cramp.nav build\atari
copy projects\slicks\navigation\freeway.nav build\atari
copy projects\slicks\navigation\gta.nav build\atari
copy projects\slicks\navigation\island.nav build\atari
copy projects\slicks\navigation\mtcarlo.nav build\atari
copy projects\slicks\navigation\rally.nav build\atari
copy projects\slicks\navigation\river.nav build\atari
copy projects\slicks\navigation\stadium.nav build\atari
copy projects\slicks\music\speednik-atari.rmt build\atari\speednik.mus

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
utils\cc65\bin\cl65 -o build/atari/slicks.bin -m build/slicks-atari.map -Cl -O -t atarixl -Wl -D,__STACKSIZE__=$0400,-D,__CHARGENSIZE__=$0000 -C atarixl-largehimem.cfg -I unity projects/slicks/src/slicks.c projects/slicks/src/game.c projects/slicks/src/interface.c projects/slicks/src/navigation.c projects/slicks/src/network.c unity/Atari/POKEY.s build/atari/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_atarixl.lib
utils\cc65\bin\cl65 -t atarixl -C atari-asm.cfg -o build/atari/basicoff.bin unity/Atari/BASICOFF.s
utils\scripts\atari\mads.exe -o:build/atari/rmt.bin unity/Atari/RMT.a65

utils\py27\python utils\scripts\atari\AtariMerge.py build/atari/xautorun build/atari/basicoff.bin build/atari/slicks.bin build/atari/rmt.bin

echo DONE!

echo --------------- ATARI DISK BUILDER --------------- 

del build\atari\*.bin
del build\atari\*.lib
del build\atari\*.lst
copy utils\scripts\atari\xbios.com build\atari\autorun
copy utils\scripts\atari\xbios.cfg build\atari\xbios.cfg
utils\scripts\atari\dir2atr.exe -d -B utils/scripts/atari/xboot.obx 1440 build/slicks-atari.atr build\atari

echo DONE
