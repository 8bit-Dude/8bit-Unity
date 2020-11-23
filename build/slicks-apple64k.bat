echo off

mkdir apple
cd ..

del build\apple\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/menu-apple.png build/apple/menu.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/arizona-apple.png build/apple/arizona.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/arto-apple.png build/apple/arto.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/cramp-apple.png build/apple/cramp.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/freeway-apple.png build/apple/freeway.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/gta-apple.png build/apple/gta.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/island-apple.png build/apple/island.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/mtcarlo-apple.png build/apple/mtcarlo.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/rally-apple.png build/apple/rally.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/river-apple.png build/apple/river.img
utils\py27\python utils\scripts\apple\AppleBitmap.py single projects/slicks/bitmaps/stadium-apple.png build/apple/stadium.img
utils\py27\python utils\scripts\apple\AppleSprites.py single projects/slicks/sprites/sprites-apple.png build/apple/sprites.dat 5

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\bitmap.c
utils\cc65\bin\ca65 -t apple2 unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\charmap.c
utils\cc65\bin\ca65 -t apple2 unity\charmap.s
del unity\charmap.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\chunks.c
utils\cc65\bin\ca65 -t apple2 unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\geom2d.c
utils\cc65\bin\ca65 -t apple2 unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\mouse.c
utils\cc65\bin\ca65 -t apple2 unity\mouse.s
del unity\mouse.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\music.c
utils\cc65\bin\ca65 -t apple2 unity\music.s
del unity\music.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\net-base.c
utils\cc65\bin\ca65 -t apple2 unity\net-base.s
del unity\net-base.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\net-url.c
utils\cc65\bin\ca65 -t apple2 unity\net-url.s
del unity\net-url.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\net-tcp.c
utils\cc65\bin\ca65 -t apple2 unity\net-tcp.s
del unity\net-tcp.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\net-udp.c
utils\cc65\bin\ca65 -t apple2 unity\net-udp.s
del unity\net-udp.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\net-web.c
utils\cc65\bin\ca65 -t apple2 unity\net-web.s
del unity\net-web.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\pixel.c
utils\cc65\bin\ca65 -t apple2 unity\pixel.s
del unity\pixel.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\print.c
utils\cc65\bin\ca65 -t apple2 unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\scaling.c
utils\cc65\bin\ca65 -t apple2 unity\scaling.s
del unity\scaling.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\sfx.c
utils\cc65\bin\ca65 -t apple2 unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\sprites.c
utils\cc65\bin\ca65 -t apple2 unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\widgets.c
utils\cc65\bin\ca65 -t apple2 unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\CLOCK.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\CLOCK.s
del unity\Apple\CLOCK.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\directory.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\directory.s
del unity\Apple\directory.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\files.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\files.s
del unity\Apple\files.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\hires.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\hires.s
del unity\Apple\hires.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\pixelDHR.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\pixelDHR.s
del unity\Apple\pixelDHR.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -I unity unity\Apple\pixelSHR.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\pixelSHR.s
del unity\Apple\pixelSHR.s
utils\cc65\bin\ca65 -t apple2 unity\atan2.s
utils\cc65\bin\ca65 -t apple2 unity\chars.s
utils\cc65\bin\ca65 -t apple2 unity\tiles.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\blitDHR.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\blitSHR.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\DUET.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\hiresLines.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\joystick.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\MOCKING.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\PADDLE.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\prodos.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\scrollDHR.s
utils\cc65\bin\ca65 -t apple2 unity\Apple\scrollSHR.s
utils\cc65\bin\ar65 r build/apple/unity.lib unity\bitmap.o unity\charmap.o unity\chunks.o unity\geom2d.o unity\mouse.o unity\music.o unity\net-base.o unity\net-url.o unity\net-tcp.o unity\net-udp.o unity\net-web.o unity\pixel.o unity\print.o unity\scaling.o unity\sfx.o unity\sprites.o unity\widgets.o unity\Apple\CLOCK.o unity\Apple\directory.o unity\Apple\files.o unity\Apple\hires.o unity\Apple\pixelDHR.o unity\Apple\pixelSHR.o unity\atan2.o unity\chars.o unity\tiles.o unity\Apple\blitDHR.o unity\Apple\blitSHR.o unity\Apple\DUET.o unity\Apple\hiresLines.o unity\Apple\joystick.o unity\Apple\MOCKING.o unity\Apple\PADDLE.o unity\Apple\prodos.o unity\Apple\scrollDHR.o unity\Apple\scrollSHR.o 
utils\cc65\bin\cl65 -o build/apple/slicks.bin -m build/slicks-apple64k.map -Cl -O -t apple2 -Wl -D,__STACKSIZE__=$0400,-D,__HIMEM__=$BC00,-D,__LCADDR__=$D000,-D,__LCSIZE__=$1000 -C apple2-hgr.cfg -I unity projects/slicks/src/slicks.c projects/slicks/src/game.c projects/slicks/src/interface.c projects/slicks/src/navigation.c projects/slicks/src/network.c build/apple/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_apple2.lib

utils\scripts\exomizer-3.0.2.exe sfx bin build/apple/slicks.bin -o build/apple/loader

echo DONE!

echo --------------- APPLE DISK BUILDER --------------- 

copy utils\scripts\apple\ProDOS190.dsk build\slicks-apple64k.do
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -as build/slicks-apple64k.do LOADER bin 0x0803 < build/apple/loader
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do SPRITES.DAT bin < build/apple/sprites.dat
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do MENU.IMG bin < build/apple/menu.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ARIZONA.IMG bin < build/apple/arizona.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ARTO.IMG bin < build/apple/arto.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do CRAMP.IMG bin < build/apple/cramp.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do FREEWAY.IMG bin < build/apple/freeway.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do GTA.IMG bin < build/apple/gta.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ISLAND.IMG bin < build/apple/island.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do MTCARLO.IMG bin < build/apple/mtcarlo.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do RALLY.IMG bin < build/apple/rally.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do RIVER.IMG bin < build/apple/river.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do STADIUM.IMG bin < build/apple/stadium.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do SPEEDNIK.MUS bin < projects/slicks/music/speednik-apple.m
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ARIZONA.NAV bin < projects/slicks/navigation/arizona.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ARTO.NAV bin < projects/slicks/navigation/arto.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do CRAMP.NAV bin < projects/slicks/navigation/cramp.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do FREEWAY.NAV bin < projects/slicks/navigation/freeway.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do GTA.NAV bin < projects/slicks/navigation/gta.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do ISLAND.NAV bin < projects/slicks/navigation/island.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do MTCARLO.NAV bin < projects/slicks/navigation/mtcarlo.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do RALLY.NAV bin < projects/slicks/navigation/rally.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do RIVER.NAV bin < projects/slicks/navigation/river.nav
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/slicks-apple64k.do STADIUM.NAV bin < projects/slicks/navigation/stadium.nav

echo DONE
