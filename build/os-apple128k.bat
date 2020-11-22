echo off

mkdir apple
cd ..

del build\apple\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

utils\py27\python utils\scripts\apple\AppleBitmap.py double projects/os/bitmaps/hokusai-apple.png build/apple/hokusai.img
utils\py27\python utils\scripts\apple\AppleBitmap.py double projects/os/bitmaps/munch-apple.png build/apple/munch.img
utils\py27\python utils\scripts\apple\AppleBitmap.py double projects/os/bitmaps/picasso-apple.png build/apple/picasso.img
utils\py27\python utils\scripts\apple\AppleBitmap.py double projects/os/bitmaps/vangogh-apple.png build/apple/vangogh.img
utils\py27\python utils\scripts\apple\AppleSprites.py double projects/os/sprites/sprites-apple.png build/apple/sprites.dat 7
utils\py27\python utils\scripts\ProcessChunks.py apple-double projects/os/chunks/chunks-apple.txt build/apple/

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\bitmap.c
utils\cc65\bin\ca65 -t apple2 unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\charmap.c
utils\cc65\bin\ca65 -t apple2 unity\charmap.s
del unity\charmap.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\chunks.c
utils\cc65\bin\ca65 -t apple2 unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\geom2d.c
utils\cc65\bin\ca65 -t apple2 unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\mouse.c
utils\cc65\bin\ca65 -t apple2 unity\mouse.s
del unity\mouse.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\music.c
utils\cc65\bin\ca65 -t apple2 unity\music.s
del unity\music.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\net-base.c
utils\cc65\bin\ca65 -t apple2 unity\net-base.s
del unity\net-base.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\net-url.c
utils\cc65\bin\ca65 -t apple2 unity\net-url.s
del unity\net-url.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\net-tcp.c
utils\cc65\bin\ca65 -t apple2 unity\net-tcp.s
del unity\net-tcp.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\net-udp.c
utils\cc65\bin\ca65 -t apple2 unity\net-udp.s
del unity\net-udp.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\net-web.c
utils\cc65\bin\ca65 -t apple2 unity\net-web.s
del unity\net-web.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\pixel.c
utils\cc65\bin\ca65 -t apple2 unity\pixel.s
del unity\pixel.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\print.c
utils\cc65\bin\ca65 -t apple2 unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\scaling.c
utils\cc65\bin\ca65 -t apple2 unity\scaling.s
del unity\scaling.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\sfx.c
utils\cc65\bin\ca65 -t apple2 unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\sprites.c
utils\cc65\bin\ca65 -t apple2 unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\widgets.c
utils\cc65\bin\ca65 -t apple2 unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\CLOCK.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\CLOCK.s
del unity\Apple\CLOCK.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\directory.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\directory.s
del unity\Apple\directory.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\files.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\files.s
del unity\Apple\files.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\hires.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\hires.s
del unity\Apple\hires.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\pixelDHR.c
utils\cc65\bin\ca65 -t apple2 unity\Apple\pixelDHR.s
del unity\Apple\pixelDHR.s
utils\cc65\bin\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\Apple\pixelSHR.c
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
utils\cc65\bin\cl65 -o build/apple/os.bin -m build/os-apple128k.map -Cl -O -t apple2 -D __DHR__ -Wl -D,__STACKSIZE__=$0400,-D,__HIMEM__=$BC00,-D,__LCADDR__=$D000,-D,__LCSIZE__=$1000 -C apple2-hgr.cfg -I unity projects/os/src/os.c projects/os/src/chat.c projects/os/src/files.c projects/os/src/home.c projects/os/src/image.c projects/os/src/mouse.c projects/os/src/music.c build/apple/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_apple2.lib

utils\scripts\exomizer-3.0.2.exe sfx bin build/apple/os.bin -o build/apple/loader

echo DONE!

echo --------------- APPLE DISK BUILDER --------------- 

copy utils\scripts\apple\ProDOS190.dsk build\os-apple128k.do
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -as build/os-apple128k.do LOADER bin 0x0803 < build/apple/loader
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do SPRITES.DAT bin < build/apple/sprites.dat
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do HOKUSAI.IMG bin < build/apple/hokusai.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do MUNCH.IMG bin < build/apple/munch.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do PICASSO.IMG bin < build/apple/picasso.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do VANGOGH.IMG bin < build/apple/vangogh.img
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do DUNGEON.MUS bin < projects/os/music/dungeon-apple.m
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do GOBLIN.MUS bin < projects/os/music/goblin-apple.m
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do SPEEDNIK.MUS bin < projects/os/music/speednik-apple.m
utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do README.TXT bin < projects/os/shared/readme.txt
for /f "tokens=*" %%A in (build\apple\chunks.lst) do utils\java\bin\java -jar utils\scripts\apple\AppleCommander-1.6.0.jar -p build/os-apple128k.do %%~nxA bin < %%A 

echo DONE
pause

cd "utils\emulators\AppleWin-1.26.3.1"
Applewin.exe -d1 "..\..\..\build\os-apple128k.do"
