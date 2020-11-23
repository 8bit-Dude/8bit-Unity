echo off

setlocal enableextensions enabledelayedexpansion

mkdir oric
cd ..

del build\oric\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

cd utils\scripts\oric
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/menu-oric.png ../../../build/oric/menu.dat
header -a0 ../../../build/oric/menu.dat ../../../build/oric/menu.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/arizona-oric.png ../../../build/oric/arizona.dat
header -a0 ../../../build/oric/arizona.dat ../../../build/oric/arizona.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/arto-oric.png ../../../build/oric/arto.dat
header -a0 ../../../build/oric/arto.dat ../../../build/oric/arto.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/cramp-oric.png ../../../build/oric/cramp.dat
header -a0 ../../../build/oric/cramp.dat ../../../build/oric/cramp.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/freeway-oric.png ../../../build/oric/freeway.dat
header -a0 ../../../build/oric/freeway.dat ../../../build/oric/freeway.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/gta-oric.png ../../../build/oric/gta.dat
header -a0 ../../../build/oric/gta.dat ../../../build/oric/gta.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/island-oric.png ../../../build/oric/island.dat
header -a0 ../../../build/oric/island.dat ../../../build/oric/island.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/mtcarlo-oric.png ../../../build/oric/mtcarlo.dat
header -a0 ../../../build/oric/mtcarlo.dat ../../../build/oric/mtcarlo.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/rally-oric.png ../../../build/oric/rally.dat
header -a0 ../../../build/oric/rally.dat ../../../build/oric/rally.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/river-oric.png ../../../build/oric/river.dat
header -a0 ../../../build/oric/river.dat ../../../build/oric/river.img $A000
..\..\py27\python OricBitmap.py ../../../projects/slicks/bitmaps/stadium-oric.png ../../../build/oric/stadium.dat
header -a0 ../../../build/oric/stadium.dat ../../../build/oric/stadium.img $A000
cd ..\..\..
utils\py27\python utils\scripts\oric\OricSprites.py projects/slicks/sprites/sprites-oric.png build/oric/sprites.dat 6
utils\scripts\oric\header -a0 build/oric/sprites.dat build/oric/sprites.dat $7800
utils\scripts\oric\ym2mym projects/slicks/music/speednik-oric.ym build/oric/speednik.mus
utils\scripts\oric\header -h1 -a0 build/oric/speednik.mus build/oric/speednik.mus $8000
utils\scripts\oric\header -a0 projects/slicks/navigation/arizona.nav build/oric/arizona.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/arto.nav build/oric/arto.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/cramp.nav build/oric/cramp.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/freeway.nav build/oric/freeway.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/gta.nav build/oric/gta.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/island.nav build/oric/island.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/mtcarlo.nav build/oric/mtcarlo.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/rally.nav build/oric/rally.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/river.nav build/oric/river.nav $A000
utils\scripts\oric\header -a0 projects/slicks/navigation/stadium.nav build/oric/stadium.nav $A000

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\bitmap.c
utils\cc65\bin\ca65 -t atmos unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\charmap.c
utils\cc65\bin\ca65 -t atmos unity\charmap.s
del unity\charmap.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\chunks.c
utils\cc65\bin\ca65 -t atmos unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\geom2d.c
utils\cc65\bin\ca65 -t atmos unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\hub.c
utils\cc65\bin\ca65 -t atmos unity\hub.s
del unity\hub.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\joystick.c
utils\cc65\bin\ca65 -t atmos unity\joystick.s
del unity\joystick.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\mouse.c
utils\cc65\bin\ca65 -t atmos unity\mouse.s
del unity\mouse.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\music.c
utils\cc65\bin\ca65 -t atmos unity\music.s
del unity\music.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\net-base.c
utils\cc65\bin\ca65 -t atmos unity\net-base.s
del unity\net-base.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\net-url.c
utils\cc65\bin\ca65 -t atmos unity\net-url.s
del unity\net-url.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\net-tcp.c
utils\cc65\bin\ca65 -t atmos unity\net-tcp.s
del unity\net-tcp.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\net-udp.c
utils\cc65\bin\ca65 -t atmos unity\net-udp.s
del unity\net-udp.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\net-web.c
utils\cc65\bin\ca65 -t atmos unity\net-web.s
del unity\net-web.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\pixel.c
utils\cc65\bin\ca65 -t atmos unity\pixel.s
del unity\pixel.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\print.c
utils\cc65\bin\ca65 -t atmos unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\scaling.c
utils\cc65\bin\ca65 -t atmos unity\scaling.s
del unity\scaling.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\sfx.c
utils\cc65\bin\ca65 -t atmos unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\sprites.c
utils\cc65\bin\ca65 -t atmos unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\widgets.c
utils\cc65\bin\ca65 -t atmos unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\Oric\directory.c
utils\cc65\bin\ca65 -t atmos unity\Oric\directory.s
del unity\Oric\directory.s
utils\cc65\bin\cc65 -Cl -O -t atmos -I unity unity\Oric\files.c
utils\cc65\bin\ca65 -t atmos unity\Oric\files.s
del unity\Oric\files.s
utils\cc65\bin\ca65 -t atmos unity\atan2.s
utils\cc65\bin\ca65 -t atmos unity\chars.s
utils\cc65\bin\ca65 -t atmos unity\tiles.s
utils\cc65\bin\ca65 -t atmos unity\Oric\blit.s
utils\cc65\bin\ca65 -t atmos unity\Oric\paseIJK.s
utils\cc65\bin\ca65 -t atmos unity\Oric\keyboard.s
utils\cc65\bin\ca65 -t atmos unity\Oric\scroll.s
utils\cc65\bin\ca65 -t atmos unity\Oric\sedoric.s
utils\cc65\bin\ca65 -t atmos unity\Oric\MYM.s
utils\cc65\bin\ar65 r build/oric/unity.lib unity\bitmap.o unity\charmap.o unity\chunks.o unity\geom2d.o unity\hub.o unity\joystick.o unity\mouse.o unity\music.o unity\net-base.o unity\net-url.o unity\net-tcp.o unity\net-udp.o unity\net-web.o unity\pixel.o unity\print.o unity\scaling.o unity\sfx.o unity\sprites.o unity\widgets.o unity\Oric\directory.o unity\Oric\files.o unity\atan2.o unity\chars.o unity\tiles.o unity\Oric\blit.o unity\Oric\paseIJK.o unity\Oric\keyboard.o unity\Oric\scroll.o unity\Oric\sedoric.o unity\Oric\MYM.o 
utils\cc65\bin\cl65 -o build/oric/slicks.bin -m build/slicks-oric48k.map -Cl -O -t atmos -C unity/Oric/oric.cfg -I unity projects/slicks/src/slicks.c projects/slicks/src/game.c projects/slicks/src/interface.c projects/slicks/src/navigation.c projects/slicks/src/network.c build/oric/unity.lib

utils\scripts\oric\header.exe build/oric/slicks.bin build/oric/slicks.com $0501

utils\scripts\exomizer-3.0.2.exe sfx bin build/oric/slicks.com -o build/oric/launch.com


echo DONE!

echo --------------- ORIC DISK BUILDER --------------- 

set TAP2DSK=utils\scripts\oric\tap2dsk.exe -iLAUNCH.COM build/oric/launch.com build/oric/sprites.dat build/oric/menu.img build/oric/arizona.img build/oric/arto.img build/oric/cramp.img build/oric/freeway.img build/oric/gta.img build/oric/island.img build/oric/mtcarlo.img build/oric/rally.img build/oric/river.img build/oric/stadium.img build/oric/speednik.mus build/oric/arizona.nav build/oric/arto.nav build/oric/cramp.nav build/oric/freeway.nav build/oric/gta.nav build/oric/island.nav build/oric/mtcarlo.nav build/oric/rally.nav build/oric/river.nav build/oric/stadium.nav
set TAP2DSK=%TAP2DSK% build/slicks-oric48k.dsk
%TAP2DSK%
utils\scripts\oric\old2mfm.exe build/slicks-oric48k.dsk

echo DONE
