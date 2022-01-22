         Handy V0.99a - Atari Lynx Emulator for Windows
         ---------------------------------------------
	 Modified by A.Beaucamp (8bit-dude [at] github.com)	 
	 Original by K.Wilkins (dysfunction [at] users.sourceforge.net)
	 
	 https://github.com/8bit-Dude/handy-fork/tree/master/handy-win32src-0.95-patched


       <-------------------HANDY HOMEPAGE-------------------->
                       http://handy.sourceforge.net
       <----------------------------------------------------->

*********************************************************************
             DO NOT ASK ME FOR LYNX CARTRIDGE IMAGES.
         (I WILL NOT REPLY TO ANY EMAIL ON THIS SUBJECT)
*********************************************************************


Index
-----

1.0      Installation
  1.1      Game Images
  1.2      System Requirements
2.0      Using Handy
3.0      Emulator Development status
4.0      Converting cartridges to LNX format with MAKE_LNX.EXE
5.0      Patching files with ROTATE.EXE
6.0      With thanks to


1.0 Installation
----------------

Unpack all of the files into the directory of your choice. e.g

md c:\emu\handy
cd c:\emu\handy
pkunzip handyXXX.zip
del handyXXX.zip

If you have lynxboot.img then just unpack it into the same directory. If
you dont have a real lynxboot.img you will not be able to run commercial
cartridges, the one in the zip archive is a fake file and Handy will warn
you of this. Lynxboot.img is copyright material and I do not have the
rights to distribute this with the emulator.

Then just run Handy.exe.

If Handy cant find the boot ROM it will put up a dialog to ask you to locate
it. REPEAT - The real lynxboot image is not provided as part of the Handy
distribution as its copyrighted material.

Try a web search for Lynxboot and see what turns up...

You wouldnt believe the number of emails I get on this.


   1.1 Game images
   ---------------

DO NOT ASK ME FOR LYNX CARTRIDGE IMAGES, I WILL NOT REPLY TO ANY EMAIL
ON THIS SUBJECT.

NOTE: Handy will NOT run .LYX images, you need to convert to .LNX format
using the supplied 'make_lnx' utility, see section 4.0.

You will need a proper lynxboot.img to play carts, this has to be extracted
from the lynx itself to run cartridges, Handy will warn you if you do not have
a correct boot image. I'm sure some unscrupulous person will publish it
somewhere along with game images, just don't ask me for them. The one included
in this release is a fake and is non functional. i.e You cant play cartridges
with it.

There are a numer of "homebrew" images floating around in homebrew.zip
these are all copyright Bastian Schick and are from his homepage. He
deserves some real high praise for the work he has done.
<---------------------------------------------------------->
   http://www.geocities.com/SiliconValley/Byte/4242/lynx/
<---------------------------------------------------------->


   1.2 System Requirements
   -----------------------

* PC running Windows 95/98/98SE/NT/2000
* 16MB or more system memory.
* Pentium class processor or better (Recommended P133 or higher).
* Minimum 1MB Hard Disk space + whatever images you require/own.

I would not recommend enabling sound if you have anything less than a P133
as its only just acceptable on that class of machine. 

This program performs best under 65536(16bit) colour mode, but also works in
256 colour mode. It will NOT work in 16 colour mode or less.



2.0 Using Handy
---------------

Just click on the Handy icon and away you go. Select your file from the
file browser. You can also start via the command line with:

handy <filename>

When starting from the command line the ESC key will exit the emulator.

By default the keys are mapped to:

Option 1     : 1
Option 2     : 2

Pause        : Q

Inside Fire  : Z
Outside Fire : X

Joypad       : Arrow keys

The Lynx buttons can be remapped to your own choice of keys via the options
menu, see the "Define Keys" option.

Reset Lynx   : F3
Run/Pause    : F5
Sound on/off : Ctrl-S
Load cart    : Ctrl-0
Network Mode : Ctrl-N
BMP Snapshot : Ctrl-Alt-B
RAW Snapshot : Ctrl-Alt-R
Exit         : Alt-F4 (or ESC in command line mode)

Joysticks are supported by selecting the Joystick under the options menu. 

Pad - Direction Keys
Button 1 - Lynx Button B (Gravis Gamepad - Red Button)
Button 2 - Lynx Button A (Gravis Gamepad - Blue Button)
Button 3 - Lynx Option 1 (Gravis Gamepad - Yellow Button) 
Button 4 - Lynx Option 2 (Gravis Gamepad - Green Button)

Note: Enabling the joystick will slow the emulator slightly, hence the joystick
      defaults to off.

You can enter full screen mode at any point by doing Alt-Enter as with a DOS
box, either Esc or Alt-Enter will return you to windowed mode. Any form of
error will cause Handy to revert to windowed mode and then display the error.

The type of full screen mode you get will depend on the windowed settings:

Windowed        Full Screen
---------------------------
Normal X1   ==  Normal X1 image in a 320x240 screen
Normal X2   ==  Normal X2 image in a 320x240 screen
Normal X2   ==  Normal X2 image in a 320x240 screen

Rotated X1  ==  Rotated X1 images in a 512x384 screen
Rotated X2  ==  Rotated X1 images in a 512x384 screen
Rotated X3  ==  Rotated X1 images in a 512x384 screen



3.0 Status
----------

Handy is writtin using MSVC 6.0 in C++ using MFC in the windows areas. The
underlying lynx emulation is written entirely in C++ and should be reasonably
easy to port to any other architecture. The emulation is 100% complete but
I'm sure there are still one or two bugs lurking under the hood.

The documents are not the most fun things to work with, the abiguities within
them mean that often my interpretation of them is incorrect and hence the
large amount of bug fixing to be done, especially in Suzy's sprite engine as
the sprite docs are vague in the extreme in places.

There are minor graphics issues on some cartridges. If you have a cartridge
image that hangs at the blank screen (colour is random) what you'll find is
that you have a corrupted cartridge image. I know for a fact that there are
a whole batch of corrupted images floating around on the net.



4.0 Converting cartridges to LNX format with MAKE_LNX.EXE
---------------------------------------------------------

Handy only reads .LNX formats files, any will not read raw (.LYX) cartridge
images. To convert from raw (.LYX/.040) to LNX format use the make_lnx
utility included in with the release.

make_lnx <infile> [options]

Optional arguments
------------------
   -o Output filename (Default=<infile>.lnx)
   -m Manufacturer    (Default=Atari)
   -g Game name       (Default=<infile>.lnx)
   -r Left/Right      (Default=no screen image rotation)
   -b0 Bank0 size     (Default=Automatic, options 0K,64K,128K,256K,512K)
   -b1 Bank1 size     (Default=0K, options 0K,64K,128K,256K,512K)

The default action (no optional params) is to convert the input filename
from raw format to LNX format with the default options given above. Options
can be mixed and placed in any order on the command line.

Example:
make_lnx cgames.lyx                  (Converts cgames.lyx to cgames.lnx)
make_lnx cgames.lyx -o calgames.lnx  (Converts cgames.lyx to calgames.lnx)

Part of the LNX header contains game and manufacturer information that may
be used in later versions of Handy, you can add this into the header using
the -g and -m options.

Example:

make_lnx cgames.lyx -g "California Games" -m Atari

(Converts cgames.lyx to cgames.lnx and sets the internal game/manufacturer
 fields within the LNX header. The quotes are important only if your game
 name has spaces within it.)



5.0 Patching files with ROTATE.EXE
----------------------------------

Handy now supports automatic rotation of images on startup via a flag in the
LNX header of the cartridge. This new addition is completely compatible with
any existing files. One of the spare bytes in the header is now used to
indicate whether which way the screen image should be rotated. A utility has
been provided (ROTATE.EXE) to allow you to patch any existing files you may
have. This facility has also bee added to the MAKE_LNX.EXE utility.

Example:

   ROTATE LEFT GAUNTLET.LNX

This will cause the gaunlet cart header to be modified so that it will be
displayed correctly when starting handy.

You can undo the change with

  ROTATE DEFAULT GAUNTLET.LNX

Gauntlet will then revert back to the landscape style display.



6.0 With Thanks
---------------

Lee Witek         - Best friend and windows guru.

Bastian Schick    - Your HTML lynx docs make my paper copy redundant, thanks.
                    Also a big thanks for all of your lynx development work.

Harry Dodgson     - Insights into timer related pernicousness...
                    Help with test code for sprite engine bugs.

Lucien            - Sprite engine debugging code.
                  
RTOONist          - Beta testing & suggestions.

Riddler           - Beta testing.

Jonas Gustavsson  - DEC Alpha Port

Ruchard Bannister - Apple MAC Port

Steven Fuller     - Linux Port

Gareth Stewart    - New background images

Thanks to all those who've emailed their support.


--- END OF FILE ---
