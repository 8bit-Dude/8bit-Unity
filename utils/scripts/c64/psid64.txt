                                 PSID64 README
                                 =============

INTRODUCTION
------------

PSID64 is a program that automatically generates a C64 self extracting
executable from a PSID file. The executable contains the PSID data, a
pre-relocated player and may also - if there is enough space available in the
C64 environment - contain a demonstration program with information about the
PSID file.

PSID files contain music code and data originally generated on the Commodore 64
(C64). If you're new to PSID files please refer to the introduction chapter of
the High Voltage SID Collection documentation for a brief introduction into SID
music.

Before PSID64, playing PSID files on a C64 or a C64 emulator has been a rather
frustrating experience. The fundamental problem of where to locate the PSID
driver has simply not been appropriately addressed. Actually, having a PSID
file play at all has been pure luck, since there has been made no attempt to
find a memory range that is not written to by the music code. Not surprisingly,
a large number of PSID files could not be played at all using existing C64
computers and C64 emulator SID players.

The document "Proposal for C64 compatible PSID files", written by Dag Lem and
Simon White extends the PSID format with relocation information. PSID64 is the
first implementation of a PSID player intended for playback on a real C64 or
C64 environment that uses this relocation information stored in the PSID
header.


USAGE
-----

PSID64 is invoked from the command line as follows:

    psid64 [OPTION]... PSID_FILE...

where PSID_FILE is the names of one or more the PSID files. The resulting
C64 executable(s) are written to the current directory. Options available:

  -b, --blank-screen     use a minimal driver that blanks the screen
  -c, --compress         compress output file with Exomizer
  -g, --global-comment   include the global comment STIL text
  -i, --initial-song=NUM override the initial song to play
  -n, --no-driver        convert SID to C64 program file without driver code
  -o, --output=FILE      specify output file
  -p, --player-id=FILE   specify SID ID config file for player identification
  -r, --root             specify HVSC root directory
  -s, --songlengths=FILE specify HVSC song length database
  -t, --theme=THEME      specify a visual theme for the driver
                         use `help' to show the list of available themes
  -v, --verbose          explain what is being done
  -h, --help             display this help and exit
  -V, --version          output version information and exit


ENVIRONMENT VARIABLES
---------------------

HVSC_BASE                Default HVSC root directory
HVSC_SONGLENGTHS         Default HVSC song length database
SIDIDCFG                 Default SID ID configuration file


KEYS PLAYER INSTRUCTIONS
------------------------

1-0, A-Z		Select song 1-36
+			Select next song (**)
-			Select previous song (**)
INST/DEL		Toggle screen blanking on/off (**)
RUN/STOP		Stop playback
LEFT ARROW		Fast forward (*)
SHIFT LEFT / LOCK	Show rastertime used by player (*)
CONTROL + CBM + DEL	Reset computer (**)

* not available for custom players (i.e. play address is $0000).
** not available in minimal driver (i.e. the driver that blanks the screen)


JOYSTICK CONTROL
----------------

Except when using the minimal driver, a joystick connected to port two can be
used to control some functions of the player.

Up			Select next song
Down			Select previous song
Left			Stop playback
Right			Restart current song
Fire button		Fast forward (*)

* not available for custom players (i.e. play address is $0000).


CREDITS
-------

PSID64 contains the following contributed or derived work. In the order they
first supplied contributions or code was derived from their work:

    Dag Lem           - PSID driver reference implementation
    Simon White       - SidUtils library
    Michael Schwendt  - SidTune library
    LaLa              - STILView library
    Magnus Lind       - Exomizer compressor

Credit where credit is due, so if I missed anyone please let me know.


KNOWN PROBLEMS AND LIMITATIONS
------------------------------

At least two free 256 byte pages are required for the minimal driver and at
least nine 256 byte pages are required for the extended driver.

The accuracy of the clock cannot be guaranteed while fast forwarding a song.
This is due to the fact that the driver does not know how often the play
function is called. PSID64 assumes that the play function called 50 times per
second on a PAL machine and 60 times per second on an NTSC machine.

The scroller might show some artifacts when interrupts occur just before or just
after the scroller area. To guarantee maximal compatibility with SID tunes it's
neither feasible nor desired to program a scroller based on code that generates
interrupts. Any ideas on how to improve the scroller code are welcome.


OTHER TOOLS
-----------

Andre Fachat's relocating 6502 cross assembler XA is used to create the
relocatable driver code. XA can be found at:

    http://www.floodgap.com/retrotech/xa/

The Linux to DJGPP cross-compiler djcross-gcc-4.8.2 has been used to build the
MS-DOS executable. It is maintained by Andris Pavenis and available at:

    http://ap1.pp.fi/djgpp/gcc/4.8.2/

UPX is a free, portable, extendable, high-performance executable packer for
several different executable formats. It has been used to create the compressed
executable of the MS-DOS release.

    http://upx.sourceforge.net/

The Fedora MinGW cross-compiler environment has been used to build the Windows
32-bit and 64-bit executables.

    http://fedoraproject.org/wiki/MinGW
    http://www.mingw.org/
