"""
 * Copyright (c) 2018 Anthony Beaucamp.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
"""

from Tkinter import *
import tkMessageBox as messagebox
import DragDropListbox as DragDropListbox
from tkFileDialog import askopenfilename, asksaveasfilename
from PIL import Image, ImageTk
import os, pickle, pygubu, sys, collections, json, codecs

# Script extension    
if "nt" == os.name:
    addr = ""
    sext = ".bat"
    cl15 = "utils\\scripts\\c64\\c1541.exe"
    sidr = "utils\\scripts\\c64\\sidreloc.exe"
    datr = "utils\\scripts\\atari\\dir2atr.exe"
    ex30 = "utils\\scripts\\exomizer-3.0.2.exe"
    ex31 = "utils\\scripts\\exomizer-3.1.0.exe"
    mads = "utils\\scripts\\atari\\mads.exe"
    famt = "utils\\scripts\\nes\\text2data.exe"
    orih = "utils\\scripts\\oric\\header.exe"
    orim = "utils\\scripts\\oric\\ym2mym.exe"
    orit = "utils\\scripts\\oric\\tap2dsk.exe"
    orio = "utils\\scripts\\oric\\old2mfm.exe"
    ar65 = "utils\\cc65\\bin\\ar65"
    ca65 = "utils\\cc65\\bin\\ca65"
    cc65 = "utils\\cc65\\bin\\cc65"
    cl65 = "utils\\cc65\\bin\\cl65"
    java = "utils\\java\\bin\\java"
    py27 = "utils\\py27\\python"
    icon = "builder.ico"
else:
    addr = "\\"
    sext = ".sh"
    cl15 = "wine utils/scripts/c64/c1541.exe"
    sidr = "wine utils/scripts/c64/sidreloc.exe"
    datr = "wine utils/scripts/atari/dir2atr.exe"
    ex30 = "wine utils/scripts/exomizer-3.0.2.exe"
    ex31 = "wine utils/scripts/exomizer-3.1.0.exe"
    mads = "wine utils/scripts/atari/mads.exe"
    famt = "wine utils/scripts/nes/text2data.exe"
    orih = "wine utils/scripts/oric/header.exe"
    orim = "wine utils/scripts/oric/ym2mym.exe"
    orit = "wine utils/scripts/oric/tap2dsk.exe"
    orio = "wine utils/scripts/oric/old2mfm.exe"
    ar65 = "ar65"
    ca65 = "ca65"
    cc65 = "cc65"
    cl65 = "cl65"
    java = "java"
    py27 = "python2"
    icon = "@builder.xbm"
            
cCore = [ 'adaptors/joystick.c', 'adaptors/mouse.c', 'geom/geom2d.c', 'math/dot.c', 'sound/music.c', 'sound/sfx.c',
          'bitmap/InitBitmap.c', 'bitmap/ShowBitmap.c', 'bitmap/HideBitmap.c', 'bitmap/ClearBitmap.c',  'bitmap/LoadBitmap.c',
          'charmap/InitCharmap.c','charmap/ShowCharmap.c','charmap/HideCharmap.c', 'charmap/ClearCharmap.c', 'charmap/LoadCharmap.c', 'charmap/LoadCharset.c', 'charmap/LoadTileset.c', 
          'charmap/PrintCharmap.c', 'charmap/DrawCharmap.c', 'charmap/ScrollCharmap.c', 'charmap/DecodeTiles.c', 'charmap/GetFlag.c', 'charmap/GetTile.c', 'charmap/SetTile.c', 'charmap/FreeCharmap.c',
          'chunks/LoadChunks.c', 'chunks/GetChunk.c', 'chunks/SetChunk.c', 'chunks/FreeChunk.c', 'files/FileExists.c',
          'graphics/logos.c', 'graphics/menu.c', 'graphics/parallax.c', 'graphics/scaling.c', 
          'network/net-base.c', 'network/net-easy.c', 'network/net-ip.c', 'network/net-tcp.c', 'network/net-udp.c', 'network/net-url.c', 'network/net-web.c', 
          'sprites/LoadSprites.c', 'sprites/EnableSprite.c', 'sprites/DisableSprite.c', 'sprites/LocateSprite.c','sprites/SetSprite.c', 'sprites/CropSprite.c', 'sprites/RecolorSprite.c', 'sprites/ScaleSprite.c', 
          'strings/blanks.c', 'strings/copy.c', 'strings/cursor.c', 'strings/guru.c', 'strings/input.c', 'strings/number.c',  'strings/print.c',
          'widgets/Button.c', 'widgets/CheckCallbacks.c', 'widgets/ClearCallbacks.c', 'widgets/Icon.c', 'widgets/Input.c', 'widgets/ListBox.c', 'widgets/Panel.c', 'widgets/PopCallback.c', 'widgets/ProcessInput.c', 'widgets/PushCallback.c', 'widgets/ScrollBar.c', 'widgets/SliderPos.c' ]

sCore = [ 'math/atan2.s', 'charmap/DecodeTiles2x2.s' ]

# Useful functions
def CD(path):
    if "nt" == os.name:
        return 'cd "' + path.replace('/','\\') + '"\n'
    else:
        return 'cd "' + path + '"\n'   

def Copy(f1, f2):
    if "nt" == os.name:
        return 'copy ' + f1.replace('/','\\') + ' ' + f2.replace('/','\\') + '\n'
    else:
        return 'cp ' + f1 + ' ' + f2 + '\n'
    
def Remove(filename):
    if "nt" == os.name:
        return 'del ' + filename.replace('/','\\') + ' /F /Q\n'
    else:
        return 'rm ' + filename + '\n'
        
def Emulate(path, executable, disk):
    disk = '..\\..\\..\\' + buildFolder + '\\' + disk
    cmd = 'echo Ready to start Emulation...\n\n'
    if "nt" == os.name:
        cmd += 'pause\n\n'
        cmd += CD(path)
        cmd += 'if exist "' + disk + '" (\n'
        cmd += '  ' + executable + ' "' + disk + '"\n'
        cmd += ') else (\n'
        cmd += '  ' + executable + ' "' + disk[0:-4] + '-d1' + disk[-4:len(disk)] + '"\n'
        cmd += ')\n'
    else:
        cmd += 'read -p "Press ENTER to continue..."\n\n'
        cmd += CD(path)
        cmd += 'if [ ! -a "' + disk + '" ]; then \n'
        cmd += '  wine ' + executable + ' "' + disk + '"\n'
        cmd += 'else\n'                    
        cmd += '  wine ' + executable + ' "' + disk[0:-4] + '-d1' + disk[-4:len(disk)] + '"\n'
        cmd += 'fi\n'                    
    return cmd
    
def Str2Bool(v):
    return v.lower() in ("yes", "true", "1")
    
def FileBase(filepath, suffix):
    # Return asset file base
    name = os.path.basename(filepath).lower().replace(suffix, '')
    name = name.split("-")
    return name[0]
    
    
# Unity Library Builder    
def BuildUnityLibrary(self, fp, target, symbols, cList, sList, filepath):
    # Add shared symbols
    if self.combobox_TileSize.get() == 'None':
        symbols += ' -D __TILE_NONE__'                        
    elif self.combobox_TileSize.get() == '2x2':
        symbols += ' -D __TILE_2X2__'                        
    elif self.combobox_TileSize.get() == '3x3':
        symbols += ' -D __TILE_3X3__'                        
    elif self.combobox_TileSize.get() == '4x4':
        symbols += ' -D __TILE_4X4__'      

    # Compile .c files
    if "nt" == os.name:
        fp.write('set SYMBOLS=-Cl -O ' + target + ' ' + symbols + ' -I unity\n')
        for file in cList:
            fp.write(cc65 + ' %SYMBOLS% unity/' + file + '\n')
    else:
        fp.write('SYMBOLS="-Cl -O ' + target + ' ' + symbols + ' -I unity"\n')
        for file in cList:
            fp.write(cc65 + ' $SYMBOLS unity/' + file + '\n')
    fp.write('\n')
    
    # Compile .s files
    if "nt" == os.name:
        fp.write('set SYMBOLS=' + target + ' ' + symbols + '\n')
        for file in cList:
            fp.write(ca65 + ' %SYMBOLS% unity/' + file[0:-2] + '.s\n')
        for file in sList:            
            fp.write(ca65 + ' %SYMBOLS% unity/' + file + '\n')
    else:
        fp.write('SYMBOLS="' + target + ' ' + symbols + '"\n')
        for file in cList:
            fp.write(ca65 + ' $SYMBOLS unity/' + file[0:-2] + '.s\n')
        for file in sList:            
            fp.write(ca65 + ' $SYMBOLS unity/' + file + '\n')
    fp.write('\n')
    
    # Package into .lib
    fp.write(ar65 + ' r ' + filepath)
    for file in cList:
        fp.write(' unity/' + file[0:-2] + '.o')
    for file in sList:            
        fp.write(' unity/' + file[0:-2] + '.o')
    fp.write('\n\n')
    
    # Clean-up
    if "nt" == os.name:
        fp.write('del')
        for file in cList:
            fp.write(' unity\\' + file[0:-2].replace('/','\\') + '.s')
            fp.write(' unity\\' + file[0:-2].replace('/','\\') + '.o')
        for file in sList:            
            fp.write(' unity\\' + file[0:-2].replace('/','\\') + '.o')
    else:
        fp.write('rm')
        for file in cList:
            fp.write(' unity/' + file[0:-2] + '.s')
            fp.write(' unity/' + file[0:-2] + '.o')
        for file in sList:            
            fp.write(' unity/' + file[0:-2] + '.o')    
    fp.write('\n\n')    

# Constants
PROJECT_FILE_FORMATS = (("Project files","*.builder"), )

# Defaults options
useGUI = True
callEmu = True
projectFile = ""
buildFolder = "build"

# Parse parameters
i = 1
while i<len(sys.argv):
    if sys.argv[i] == '-projectFile':
        projectFile = sys.argv[i+1]
    if sys.argv[i] == '-buildFolder':
        buildFolder = sys.argv[i+1]
    if sys.argv[i] == '-callEmu':
        callEmu = Str2Bool(sys.argv[i+1])
    if sys.argv[i] == '-useGUI':
        useGUI = Str2Bool(sys.argv[i+1])
    i += 1

print "projectFile: ", projectFile, " buildFolder: ", buildFolder, " callEmu: ", callEmu, " useGUI: ", useGUI

class Application:

    version = 2
    
    listbox_Code = None
    listbox_Charmap = None
    listbox_Shared = None
    
    listbox_AppleBitmapDHR = None
    listbox_AppleBitmapSHR = None
    listbox_AppleCharsetDHR = None
    listbox_AppleCharsetSHR = None
    listbox_AppleSpritesDHR = None
    listbox_AppleSpritesSHR = None
    listbox_AppleChunks = None
    listbox_AppleMusic = None
    
    listbox_AtariBitmap = None
    listbox_AtariCharset = None
    listbox_AtariSprites = None
    listbox_AtariChunks = None
    listbox_AtariMusic = None
    
    listbox_C64Bitmap = None
    listbox_C64Charset = None
    listbox_C64Sprites = None
    listbox_C64Chunks = None
    listbox_C64Music = None
    
    listbox_LynxBitmap = None
    listbox_LynxCharset = None
    listbox_LynxSprites = None
    listbox_LynxChunks = None
    listbox_LynxMusic = None    

    listbox_NESBitmap = None
    listbox_NESCharset = None
    listbox_NESSprites = None
    listbox_NESChunks = None
    listbox_NESMusic = None
    
    listbox_OricBitmap = None
    listbox_OricCharset = None
    listbox_OricSprites = None
    listbox_OricChunks = None
    listbox_OricMusic = None
    
    lists = None
    cwd = None
        
    def __init__(self, master):

        # Save working directory
        self.cwd = os.getcwd()
        self.cwd, dump = os.path.split(self.cwd)
        self.cwd, dump = os.path.split(self.cwd)
        self.cwd = self.cwd.replace(os.sep, "/")
        self.cwd += "/"

        # Create a builder
        self.builder = pygubu.Builder()
        self.builder.add_resource_path(os.getcwd()+'/res/')

        # Load an ui file
        self.builder.add_from_file('builder.ui')

        # Show tite
        master.title("8bit-Unity Disk Builder") 
        
        # Add file menu
        # menubar = Menu(master)
        # fileMenu = Menu(menubar)
        # fileMenu.add_command(label="New", command=self.FileNew)
        # fileMenu.add_command(label="Load", command=self.FileLoad)
        # fileMenu.add_command(label="Save", command=self.FileSave)
        # fileMenu.add_separator()
        # fileMenu.add_command(label="Exit", command=self.FileExit)        
        # menubar.add_cascade(label="File", menu=fileMenu)
        # master.config(menu=menubar)
        
        # Create widgets using a master as parent
        self.mainwindow = self.builder.get_object('mainwindow', master)        
                
        # Connect callbacks
        self.builder.connect_callbacks(self)
        
        # Get list boxes and fields
        self.listbox_Code = self.builder.get_object('Listbox_Code')
        self.listbox_Charmap = self.builder.get_object('Listbox_Charmap')
        self.combobox_TileSize = self.builder.get_object('Combobox_TileSize')
        self.listbox_Shared = self.builder.get_object('Listbox_Shared')
        self.entry_Disk = self.builder.get_object('Entry_Disk')
        
        self.listbox_AppleBitmapDHR = self.builder.get_object('Listbox_AppleBitmapDHR')        
        self.listbox_AppleBitmapSHR = self.builder.get_object('Listbox_AppleBitmapSHR')        
        self.listbox_AppleCharsetDHR = self.builder.get_object('Listbox_AppleCharsetDHR')        
        self.listbox_AppleCharsetSHR = self.builder.get_object('Listbox_AppleCharsetSHR')        
        self.listbox_AppleSpritesDHR = self.builder.get_object('Listbox_AppleSpritesDHR')        
        self.listbox_AppleSpritesSHR = self.builder.get_object('Listbox_AppleSpritesSHR')        
        self.listbox_AppleChunks = self.builder.get_object('Listbox_AppleChunks')        
        self.listbox_AppleMusic = self.builder.get_object('Listbox_AppleMusic')        
        self.entry_AppleSpriteFrames = self.builder.get_object('Entry_AppleSpriteFrames')
        self.entry_AppleSpriteWidth = self.builder.get_object('Entry_AppleSpriteWidth')
        self.entry_AppleSpriteHeight = self.builder.get_object('Entry_AppleSpriteHeight')
        self.combobox_AppleDiskSize = self.builder.get_object('Combobox_AppleDiskSize');
        self.combobox_AppleCrunchAssets = self.builder.get_object('Combobox_AppleCrunchAssets');
        self.entry_AppleMusicMemory = self.builder.get_object('Entry_AppleMusicMemory')
        self.entry_AppleChunkMemory = self.builder.get_object('Entry_AppleChunkMemory')
        self.checkbutton_AppleNetwork8bitHub = self.builder.get_variable('AppleNetwork8bitHub');
        self.checkbutton_AppleNetworkUthernet = self.builder.get_variable('AppleNetworkUthernet');
        self.combobox_AppleNetworkProtocols = self.builder.get_object('Combobox_AppleNetworkProtocols');
        
        self.listbox_AtariBitmap = self.builder.get_object('Listbox_AtariBitmap')        
        self.listbox_AtariCharset = self.builder.get_object('Listbox_AtariCharset')        
        self.listbox_AtariSprites = self.builder.get_object('Listbox_AtariSprites')        
        self.listbox_AtariChunks = self.builder.get_object('Listbox_AtariChunks')        
        self.listbox_AtariMusic = self.builder.get_object('Listbox_AtariMusic')     
        self.entry_AtariSpriteFrames = self.builder.get_object('Entry_AtariSpriteFrames')
        self.entry_AtariSpriteWidth = self.builder.get_object('Entry_AtariSpriteWidth')
        self.entry_AtariSpriteHeight = self.builder.get_object('Entry_AtariSpriteHeight')
        self.combobox_AtariDiskSize = self.builder.get_object('Combobox_AtariDiskSize');
        self.combobox_AtariCrunchAssets = self.builder.get_object('Combobox_AtariCrunchAssets');
        self.entry_AtariMusicMemory = self.builder.get_object('Entry_AtariMusicMemory')
        self.entry_AtariChunkMemory = self.builder.get_object('Entry_AtariChunkMemory')
        self.checkbutton_AtariNetwork8bitHub = self.builder.get_variable('AtariNetwork8bitHub');
        self.checkbutton_AtariNetworkDragonCart = self.builder.get_variable('AtariNetworkDragonCart');
        self.checkbutton_AtariNetworkFujinet = self.builder.get_variable('AtariNetworkFujinet');
        self.combobox_AtariNetworkProtocols = self.builder.get_object('Combobox_AtariNetworkProtocols');
        
        self.listbox_C64Bitmap = self.builder.get_object('Listbox_C64Bitmap')        
        self.listbox_C64Charset = self.builder.get_object('Listbox_C64Charset')        
        self.entry_C64CharsetColors = self.builder.get_object('Entry_C64CharsetColors')
        self.listbox_C64Sprites = self.builder.get_object('Listbox_C64Sprites')        
        self.listbox_C64Chunks = self.builder.get_object('Listbox_C64Chunks')        
        self.listbox_C64Music = self.builder.get_object('Listbox_C64Music')     
        self.entry_C64SpriteFrames = self.builder.get_object('Entry_C64SpriteFrames')
        self.entry_C64SpriteWidth = self.builder.get_object('Entry_C64SpriteWidth')
        self.entry_C64SpriteHeight = self.builder.get_object('Entry_C64SpriteHeight')
        self.combobox_C64CrunchAssets = self.builder.get_object('Combobox_C64CrunchAssets');
        self.entry_C64MusicMemory = self.builder.get_object('Entry_C64MusicMemory')
        self.entry_C64ChunkMemory = self.builder.get_object('Entry_C64ChunkMemory')
        self.checkbutton_C64Network8bitHub = self.builder.get_variable('C64Network8bitHub');
        self.checkbutton_C64NetworkRRNet = self.builder.get_variable('C64NetworkRRNet');
        self.checkbutton_C64NetworkUltimate = self.builder.get_variable('C64NetworkUltimate');
        self.combobox_C64NetworkProtocols = self.builder.get_object('Combobox_C64NetworkProtocols');
        
        self.listbox_LynxBitmap = self.builder.get_object('Listbox_LynxBitmap')        
        self.listbox_LynxCharset = self.builder.get_object('Listbox_LynxCharset')        
        self.listbox_LynxSprites = self.builder.get_object('Listbox_LynxSprites')        
        self.listbox_LynxChunks = self.builder.get_object('Listbox_LynxChunks')        
        self.listbox_LynxMusic = self.builder.get_object('Listbox_LynxMusic')     
        self.entry_LynxSpriteFrames = self.builder.get_object('Entry_LynxSpriteFrames')
        self.entry_LynxSpriteWidth = self.builder.get_object('Entry_LynxSpriteWidth')
        self.entry_LynxSpriteHeight = self.builder.get_object('Entry_LynxSpriteHeight')
        self.entry_LynxMusicMemory = self.builder.get_object('Entry_LynxMusicMemory')
        self.entry_LynxChunkMemory = self.builder.get_object('Entry_LynxChunkMemory')
        self.entry_LynxSharedMemory = self.builder.get_object('Entry_LynxSharedMemory')
        self.checkbutton_LynxVirtualKeyboard = self.builder.get_variable('LynxVirtualKeyboard');

        self.listbox_NESBitmap = self.builder.get_object('Listbox_NESBitmap')        
        self.listbox_NESCharset = self.builder.get_object('Listbox_NESCharset')        
        self.listbox_NESChunks = self.builder.get_object('Listbox_NESChunks')        
        self.listbox_NESSprites = self.builder.get_object('Listbox_NESSprites')        
        self.listbox_NESMusic = self.builder.get_object('Listbox_NESMusic')     
        self.entry_NESBitmapTiles = self.builder.get_object('Entry_NESBitmapTiles')
        self.entry_NESSpriteFrames = self.builder.get_object('Entry_NESSpriteFrames')
        self.entry_NESSpriteWidth = self.builder.get_object('Entry_NESSpriteWidth')
        self.entry_NESSpriteHeight = self.builder.get_object('Entry_NESSpriteHeight')
        self.entry_NESMusicMemory = self.builder.get_object('Entry_NESMusicMemory')
        self.entry_NESChunkMemory = self.builder.get_object('Entry_NESChunkMemory')
        
        self.listbox_OricBitmap = self.builder.get_object('Listbox_OricBitmap')        
        self.listbox_OricCharset = self.builder.get_object('Listbox_OricCharset')        
        self.listbox_OricChunks = self.builder.get_object('Listbox_OricChunks')        
        self.listbox_OricSprites = self.builder.get_object('Listbox_OricSprites')        
        self.listbox_OricMusic = self.builder.get_object('Listbox_OricMusic')     
        self.entry_OricSpriteFrames = self.builder.get_object('Entry_OricSpriteFrames')
        self.entry_OricSpriteWidth = self.builder.get_object('Entry_OricSpriteWidth')
        self.entry_OricSpriteHeight = self.builder.get_object('Entry_OricSpriteHeight')
        self.entry_OricDithering = self.builder.get_object('Entry_OricDithering')
        self.entry_OricLeadColors = self.builder.get_object('Entry_OricLeadColors')
        self.entry_OricMusicMemory = self.builder.get_object('Entry_OricMusicMemory')
        self.entry_OricChunkMemory = self.builder.get_object('Entry_OricChunkMemory')
                
        # Make lists of various GUI inputs (adding new inputs to the end of each list will guarantee backward compatibility)
        self.entries = [ self.entry_Disk, 
                         self.entry_AppleSpriteFrames, self.entry_AppleSpriteWidth, self.entry_AppleSpriteHeight, 
                         self.entry_AtariSpriteFrames, self.entry_AtariSpriteWidth, self.entry_AtariSpriteHeight, 
                         self.entry_C64SpriteFrames,   self.entry_C64SpriteWidth,   self.entry_C64SpriteHeight, 
                         self.entry_LynxSpriteFrames,  self.entry_LynxSpriteWidth,  self.entry_LynxSpriteHeight, 
                         self.entry_OricSpriteFrames,  self.entry_OricSpriteWidth,  self.entry_OricSpriteHeight,
                         self.entry_OricDithering,     self.entry_LynxMusicMemory,  self.entry_LynxSharedMemory,
                         self.entry_C64CharsetColors ]
        self.listboxes = [ self.listbox_Code, 
                           self.listbox_AppleBitmapDHR,self.listbox_AppleSpritesDHR,self.listbox_AppleMusic,
                           self.listbox_AtariBitmap,   self.listbox_AtariSprites, self.listbox_AtariMusic,
                           self.listbox_C64Bitmap,     self.listbox_C64Sprites,   self.listbox_C64Music,
                           self.listbox_OricBitmap,    self.listbox_OricSprites,  self.listbox_OricMusic,
                           self.listbox_Shared, 
                           self.listbox_LynxBitmap,    self.listbox_LynxSprites,  self.listbox_LynxMusic,
                           self.listbox_AppleChunks,   self.listbox_AtariChunks,  self.listbox_C64Chunks,
                           self.listbox_LynxChunks,    self.listbox_OricChunks,
                           self.listbox_AppleCharsetDHR,self.listbox_AtariCharset, self.listbox_C64Charset,
                           self.listbox_LynxCharset,   self.listbox_OricCharset,  self.listbox_Charmap,
                           self.listbox_AppleBitmapSHR,self.listbox_AppleSpritesSHR, self.listbox_AppleCharsetSHR ]
        self.comboboxes = [ self.combobox_AtariDiskSize, self.combobox_AppleDiskSize,
                            self.combobox_AppleNetworkProtocols, self.combobox_AtariNetworkProtocols, self.combobox_C64NetworkProtocols,
                            self.combobox_AppleCrunchAssets, self.combobox_AtariCrunchAssets, self.combobox_C64CrunchAssets,
                            self.combobox_TileSize ]
        self.checkbuttons = [self.checkbutton_AppleNetwork8bitHub, self.checkbutton_AppleNetworkUthernet, 
                             self.checkbutton_AtariNetwork8bitHub, self.checkbutton_AtariNetworkDragonCart, self.checkbutton_AtariNetworkFujinet,
                             self.checkbutton_C64Network8bitHub, self.checkbutton_C64NetworkRRNet ]
        
        # Set some defaults
        self.FileNew()        
                       
    def FileNew(self):
        # Reset all fields
        for l in self.listboxes:
            l.delete(0, END)
        self.entry_Disk.delete(0, END)
        self.entry_Disk.insert(0, 'diskname')
        
        # Set some defaults
        self.combobox_TileSize.current(0)
        self.combobox_AppleDiskSize.current(0)
        self.combobox_AppleCrunchAssets.current(0)
        self.checkbutton_AppleNetwork8bitHub.set(False)
        self.checkbutton_AppleNetworkUthernet.set(False)
        self.combobox_AppleNetworkProtocols.current(1)
        self.combobox_AtariDiskSize.current(0)
        self.combobox_AtariCrunchAssets.current(0)
        self.checkbutton_AtariNetwork8bitHub.set(False)
        self.checkbutton_AtariNetworkDragonCart.set(False)
        self.checkbutton_AtariNetworkFujinet.set(False)        
        self.combobox_AtariNetworkProtocols.current(1)
        self.combobox_C64CrunchAssets.current(0)
        self.checkbutton_C64Network8bitHub.set(False)
        self.checkbutton_C64NetworkRRNet.set(False)
        self.checkbutton_C64NetworkUltimate.set(False)
        self.combobox_C64NetworkProtocols.current(1)        
        self.checkbutton_LynxVirtualKeyboard.set(True)        
        
    def FileLoad(self, filename=''):
        if filename == '':
            filename = askopenfilename(initialdir = "../../", title = "Load Builder Project", filetypes = PROJECT_FILE_FORMATS) 
        if filename is not '':
            # Reset UI
            for l in self.listboxes:
                l.delete(0, END)
            self.entry_Disk.delete(0, END)
            self.entry_Disk.insert(0, 'diskname')

            # Try to open as JSON:
            json_tree = None
            try:
                with codecs.open(filename, "r", "utf-8") as fp:
                    json_tree = json.load(fp)
            except ValueError as e:
                # Not a valid JSON
                pass

            if json_tree:
                # It is a JSON file: try to load it
                self.FileLoadJson(json_tree)                
                return

            # It's not a JSON file: assumes it is a pickle file
        
            # Unpickle data
            with open(filename, "r") as fp:
                # Version number
                print "File version: " + str(pickle.load(fp))
                data = pickle.load(fp)
                                
                # Entry boxes
                while data != 'entries':
                    data = pickle.load(fp)                    
                for item in self.entries:
                    data = pickle.load(fp)
                    if data == 'listboxes' or data == 'lists':
                        break   # Legacy file
                    item.delete(0, END)
                    item.insert(0, data)
                    
                # List boxes                     
                while data != 'listboxes' and data != 'lists':
                    data = pickle.load(fp)                    
                for item in self.listboxes:
                    data = pickle.load(fp)
                    if data == 'checkbuttons':
                        break   # Legacy file
                    item.delete(0, END)
                    for row in data:
                        item.insert(END, row)
                        
                # Check buttons
                while data != 'checkbuttons':
                    data = pickle.load(fp)                    
                for item in self.checkbuttons:                
                    data = pickle.load(fp)
                    if data == 'comboboxes':
                        break   # Legacy file
                    if 'selected' in data:
                        item.set(True)
                    else:
                        item.set(False)

                # Combo boxes
                while data != 'comboboxes':
                    data = pickle.load(fp)                    
                for item in self.comboboxes:                
                    data = pickle.load(fp)
                    item.set(data)
                    
    def FileLoadJson(self, json_tree):
        def process_node(child, template):
            for k, v in template:
                if isinstance(v, tuple) and (k in child):
                    (kind, component) = v
                    kind = kind.lower()
                    data = child[k]

                    if kind == 'entry':
                        component.delete(0, END)
                        component.insert(0, data)
                    elif kind == 'listbox':
                        component.delete(0, END)
                        for row in data:
                            component.insert(END, row)
                    elif kind == 'checkbutton':
                        if data:
                            component.set(True)
                        else:
                            component.set(False)
                    elif kind == 'combobox':
                        component.set(data)
                elif k in child:
                    process_node(child[k], v)

        structure_template = self.JsonStructureTemplate()
        process_node(json_tree, structure_template)

    def FileSave(self):
        filename = asksaveasfilename(initialdir = "../../", title = "Save Builder Project as JSON", filetypes = PROJECT_FILE_FORMATS)
        if not filename:
            # No filename chosem
            return

        # Fix extension
        if not filename.lower().endswith('.builder'):
            filename += '.builder'

        # Generates the JSON tree
        def process_node(dest, orig):
            for k, v in orig:
                child = None
                if isinstance(v, tuple):
                    (kind, component) = v
                    kind = kind.lower()

                    if kind == 'entry':
                        child = component.get()
                    elif kind == 'listbox':
                        child = list(component.get(0, END))
                    elif kind == 'checkbutton':
                        child = component.get()
                    elif kind == 'combobox':
                        child = component.get()
                else:
                    child = collections.OrderedDict()
                    process_node(child, v)

                dest[k] = child

        structure_template = self.JsonStructureTemplate()
        json_tree = collections.OrderedDict([
            ('format', '8bit-Unity Project'),
            ('formatVersion', self.version),
        ])

        process_node(json_tree, structure_template)

        # Save the file
        json_string = json.dumps(json_tree, indent=4)
        with codecs.open(filename, "w", "utf-8") as fp:
            fp.write(json_string)

    def FileExit(self):
        sys.exit()
        
    def JsonStructureTemplate(self):        
        return [
            ('general', [
                ('disk', ('entry', self.entry_Disk)),        
                ('code', ('listbox', self.listbox_Code)),    
                ('shared', ('listbox', self.listbox_Shared)),
                ('charmap', ('listbox', self.listbox_Charmap)),
                ('tilesize', ('combobox', self.combobox_TileSize)),
            ]),
            ('platform', [
                ('Apple', [
                    ('spriteFrames', ('entry', self.entry_AppleSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_AppleSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_AppleSpriteHeight)),
                    ('bitmapDHR', ('listbox', self.listbox_AppleBitmapDHR)),
                    ('bitmapSHR', ('listbox', self.listbox_AppleBitmapSHR)),
                    ('charsetDHR', ('listbox', self.listbox_AppleCharsetDHR)),
                    ('charsetSHR', ('listbox', self.listbox_AppleCharsetSHR)),
                    ('spritesDHR', ('listbox', self.listbox_AppleSpritesDHR)),
                    ('spritesSHR', ('listbox', self.listbox_AppleSpritesSHR)),
                    ('music', ('listbox', self.listbox_AppleMusic)),
                    ('chunks', ('listbox', self.listbox_AppleChunks)),
                    ('diskSize', ('combobox', self.combobox_AppleDiskSize)),
                    ('crunchAssets', ('combobox', self.combobox_AppleCrunchAssets)),
                    ('musicMemory', ('entry', self.entry_AppleMusicMemory)),
                    ('chunkMemory', ('entry', self.entry_AppleChunkMemory)),
                    ('network8bitHub', ('checkbutton', self.checkbutton_AppleNetwork8bitHub)),
                    ('networkUthernet', ('checkbutton', self.checkbutton_AppleNetworkUthernet)),
                    ('networkProtocols', ('combobox', self.combobox_AppleNetworkProtocols)),
                ]),
                ('Atari', [
                    ('spriteFrames', ('entry', self.entry_AtariSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_AtariSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_AtariSpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_AtariBitmap)),
                    ('charset', ('listbox', self.listbox_AtariCharset)),
                    ('sprites', ('listbox', self.listbox_AtariSprites)),
                    ('music', ('listbox', self.listbox_AtariMusic)),
                    ('chunks', ('listbox', self.listbox_AtariChunks)),
                    ('diskSize', ('combobox', self.combobox_AtariDiskSize)),
                    ('crunchAssets', ('combobox', self.combobox_AtariCrunchAssets)),
                    ('musicMemory', ('entry', self.entry_AtariMusicMemory)),
                    ('chunkMemory', ('entry', self.entry_AtariChunkMemory)),
                    ('network8bitHub', ('checkbutton', self.checkbutton_AtariNetwork8bitHub)),
                    ('networkDragonCart', ('checkbutton', self.checkbutton_AtariNetworkDragonCart)),
                    ('networkFujinet', ('checkbutton', self.checkbutton_AtariNetworkFujinet)),
                    ('networkProtocols', ('combobox', self.combobox_AtariNetworkProtocols)),
                ]),
                ('C64', [
                    ('spriteFrames', ('entry', self.entry_C64SpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_C64SpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_C64SpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_C64Bitmap)),
                    ('charset', ('listbox', self.listbox_C64Charset)),
                    ('charsetColors', ('entry', self.entry_C64CharsetColors)),
                    ('sprites', ('listbox', self.listbox_C64Sprites)),
                    ('music', ('listbox', self.listbox_C64Music)),
                    ('chunks', ('listbox', self.listbox_C64Chunks)),
                    ('crunchAssets', ('combobox', self.combobox_C64CrunchAssets)),
                    ('musicMemory', ('entry', self.entry_C64MusicMemory)),
                    ('chunkMemory', ('entry', self.entry_C64ChunkMemory)),
                    ('network8bitHub', ('checkbutton', self.checkbutton_C64Network8bitHub)),
                    ('networkRRNet', ('checkbutton', self.checkbutton_C64NetworkRRNet)),
                    ('networkUltimate', ('checkbutton', self.checkbutton_C64NetworkUltimate)),
                    ('networkProtocols', ('combobox', self.combobox_C64NetworkProtocols)),
                ]),
                ('Lynx', [
                    ('spriteFrames', ('entry', self.entry_LynxSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_LynxSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_LynxSpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_LynxBitmap)),
                    ('charset', ('listbox', self.listbox_LynxCharset)),
                    ('sprites', ('listbox', self.listbox_LynxSprites)),
                    ('music', ('listbox', self.listbox_LynxMusic)),
                    ('chunks', ('listbox', self.listbox_LynxChunks)),
                    ('musicMemory', ('entry', self.entry_LynxMusicMemory)),
                    ('chunkMemory', ('entry', self.entry_LynxChunkMemory)),
                    ('sharedMemory', ('entry', self.entry_LynxSharedMemory)),
                    ('virtualKeyboard', ('checkbutton', self.checkbutton_LynxVirtualKeyboard)),
                ]),
                ('NES', [
                    ('bitmapTiles', ('entry', self.entry_NESBitmapTiles)),
                    ('spriteFrames', ('entry', self.entry_NESSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_NESSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_NESSpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_NESBitmap)),
                    ('charset', ('listbox', self.listbox_NESCharset)),
                    ('sprites', ('listbox', self.listbox_NESSprites)),
                    ('music', ('listbox', self.listbox_NESMusic)),
                    ('chunks', ('listbox', self.listbox_NESChunks)),
                    ('musicMemory', ('entry', self.entry_NESMusicMemory)),
                    ('chunkMemory', ('entry', self.entry_NESChunkMemory)),
                ]),                
                ('Oric', [
                    ('spriteFrames', ('entry', self.entry_OricSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_OricSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_OricSpriteHeight)),
                    ('dithering', ('entry', self.entry_OricDithering)),
                    ('leadColors', ('entry', self.entry_OricLeadColors)),
                    ('bitmap', ('listbox', self.listbox_OricBitmap)),
                    ('charset', ('listbox', self.listbox_OricCharset)),
                    ('sprites', ('listbox', self.listbox_OricSprites)),
                    ('music', ('listbox', self.listbox_OricMusic)),
                    ('chunks', ('listbox', self.listbox_OricChunks)),
                    ('musicMemory', ('entry', self.entry_OricMusicMemory)),
                    ('chunkMemory', ('entry', self.entry_OricChunkMemory)),
                ]),
            ]),
        ]

    def CodeAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Code File", filetypes = (("C files","*.c"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Code.insert(END, filename)

    def CodeRem(self):
        self.listbox_Code.delete(ACTIVE)
                
    def CharmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Map", filetypes = (("Charmap/Tileset files","*.map;*.tls"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Charmap.insert(END, filename)

    def CharmapRem(self):
        self.listbox_Charmap.delete(ACTIVE)
            
    def SharedAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Asset File", filetypes = (("All files","*.*"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Shared.insert(END, filename)

    def SharedRem(self):
        self.listbox_Shared.delete(ACTIVE)
        
    def AppleBitmapDHRAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleBitmapDHR.insert(END, filename)

    def AppleBitmapDHRRem(self):
        self.listbox_AppleBitmapDHR.delete(ACTIVE)

    def AppleBitmapSHRAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleBitmapSHR.insert(END, filename)

    def AppleBitmapSHRRem(self):
        self.listbox_AppleBitmapSHR.delete(ACTIVE)

    def AppleCharsetDHRSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleCharsetDHR.delete(0, END)
            self.listbox_AppleCharsetDHR.insert(END, filename)

    def AppleCharsetSHRSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleCharsetSHR.delete(0, END)
            self.listbox_AppleCharsetSHR.insert(END, filename)
            
    def AppleSpritesDHRSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleSpritesDHR.delete(0, END)
            self.listbox_AppleSpritesDHR.insert(END, filename)

    def AppleSpritesSHRSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleSpritesSHR.delete(0, END)
            self.listbox_AppleSpritesSHR.insert(END, filename)
            
    def AppleChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleChunks.insert(END, filename)          

    def AppleChunksRem(self):
        self.listbox_AppleChunks.delete(ACTIVE)

    def AppleMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("DUET M files","*.m"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleMusic.insert(END, filename)

    def AppleMusicRem(self):
        self.listbox_AppleMusic.delete(ACTIVE)
            
    def AtariBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariBitmap.insert(END, filename)

    def AtariBitmapRem(self):
        self.listbox_AtariBitmap.delete(ACTIVE)

    def AtariCharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariCharset.delete(0, END)
            self.listbox_AtariCharset.insert(END, filename)
            
    def AtariSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariSprites.delete(0, END)
            self.listbox_AtariSprites.insert(END, filename)

    def AtariChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariChunks.insert(END, filename)          

    def AtariChunksRem(self):
        self.listbox_AtariChunks.delete(ACTIVE)

    def AtariMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("RMT files","*.rmt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariMusic.insert(END, filename)

    def AtariMusicRem(self):
        self.listbox_AtariMusic.delete(ACTIVE)
    
    def C64BitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Bitmap.insert(END, filename)

    def C64BitmapRem(self):
        self.listbox_C64Bitmap.delete(ACTIVE)

    def C64CharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Charset.delete(0, END)
            self.listbox_C64Charset.insert(END, filename)
            
    def C64SpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Sprites.delete(0, END)
            self.listbox_C64Sprites.insert(END, filename)

    def C64ChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Chunks.insert(END, filename)          

    def C64ChunksRem(self):
        self.listbox_C64Chunks.delete(ACTIVE)        
        
    def C64MusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("SID files","*.sid"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Music.insert(END, filename)

    def C64MusicRem(self):
        self.listbox_C64Music.delete(ACTIVE)        
        
    def LynxBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxBitmap.insert(END, filename)

    def LynxBitmapRem(self):
        self.listbox_LynxBitmap.delete(ACTIVE)
        
    def LynxCharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxCharset.delete(0, END)
            self.listbox_LynxCharset.insert(END, filename)        
        
    def LynxSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxSprites.delete(0, END)
            self.listbox_LynxSprites.insert(END, filename)

    def LynxChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxChunks.insert(END, filename)          

    def LynxChunksRem(self):
        self.listbox_LynxChunks.delete(ACTIVE)
                 
    def LynxMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("Chipper files","*.asm"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxMusic.insert(END, filename)

    def LynxMusicRem(self):
        self.listbox_LynxMusic.delete(ACTIVE) 
        
    def NESBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_NESBitmap.insert(END, filename)

    def NESBitmapRem(self):
        self.listbox_NESBitmap.delete(ACTIVE)
        
    def NESCharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_NESCharset.delete(0, END)
            self.listbox_NESCharset.insert(END, filename)        
        
    def NESSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_NESSprites.delete(0, END)
            self.listbox_NESSprites.insert(END, filename)

    def NESChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_NESChunks.insert(END, filename)          

    def NESChunksRem(self):
        self.listbox_NESChunks.delete(ACTIVE)         
        
    def NESMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("TXT files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_NESMusic.insert(END, filename)

    def NESMusicRem(self):
        self.listbox_NESMusic.delete(ACTIVE)         

    def OricBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricBitmap.insert(END, filename)

    def OricBitmapRem(self):
        self.listbox_OricBitmap.delete(ACTIVE)
        
    def OricCharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricCharset.delete(0, END)
            self.listbox_OricCharset.insert(END, filename)        
        
    def OricSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricSprites.delete(0, END)
            self.listbox_OricSprites.insert(END, filename)

    def OricChunksAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricChunks.insert(END, filename)          

    def OricChunksRem(self):
        self.listbox_OricChunks.delete(ACTIVE)          
        
    def OricMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("YM files","*.ym"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricMusic.insert(END, filename)

    def OricMusicRem(self):
        self.listbox_OricMusic.delete(ACTIVE) 

    def GenerateScripts(self):
    	# Get shared parameters
        diskname = self.entry_Disk.get()
        code = list(self.listbox_Code.get(0, END))
        charmaps = list(self.listbox_Charmap.get(0, END))
        shared = list(self.listbox_Shared.get(0, END))
        if len(code) < 1:
            messagebox.showinfo('Error', 'Please add C source file(s)!')
            return

        ####################################################
        # Apple script
        bitmapsDHR = list(self.listbox_AppleBitmapDHR.get(0, END))
        bitmapsSHR = list(self.listbox_AppleBitmapSHR.get(0, END))
        charsetDHR = list(self.listbox_AppleCharsetDHR.get(0, END))
        charsetSHR = list(self.listbox_AppleCharsetSHR.get(0, END))
        spritesDHR = list(self.listbox_AppleSpritesDHR.get(0, END))
        spritesSHR = list(self.listbox_AppleSpritesSHR.get(0, END))
        chunks = list(self.listbox_AppleChunks.get(0, END))
        music = list(self.listbox_AppleMusic.get(0, END))
        chunkSize = self.entry_AppleChunkMemory.get().replace('$0000','$0001')
        networkOptions = []
        if self.checkbutton_AppleNetwork8bitHub.get():
            networkOptions.append('8bit-Hub')
        if self.checkbutton_AppleNetworkUthernet.get():
            networkOptions.append('Uthernet')
        if len(networkOptions) == 0:
            networkOptions.append('No-Net')        
        
        # Build Single and Double Hires Scripts
        for target in ['64k', '128k']:
        
            # Assign GFX mode and assets
            bitmaps = bitmapsDHR
            charset = charsetDHR
            sprites = spritesDHR
            if target == '128k':
                graphics = 'double'
            else:
                graphics = 'single'
                if len(bitmapsSHR) > 0:
                    bitmaps = bitmapsSHR
                if len(charsetSHR) > 0:
                    charset = charsetSHR
                if len(spritesSHR) > 0:
                    sprites = spritesSHR        
        
            with open('../../' + buildFolder+'/'+diskname+"-apple"+target+sext, "wb") as fp:
                # Info
                fp.write('echo off\n\n')
                fp.write('mkdir apple\n')            
                fp.write('mkdir [libs]\n')            
                fp.write('mkdir [maps]\n')            
                fp.write('cd ..\n\n')
                fp.write(Remove('build/apple/*.*'))
                
                fp.write('\necho --------------- COMPILE PROGRAM ---------------\n\n')

                # Build Unity Library for eah network target
                for network in networkOptions:
                    cTarget = [ 'graphics/pixel.c', 'targets/apple2/clock.c', 'targets/apple2/directory.c', 'targets/apple2/files.c', 'targets/apple2/hires.c', 'targets/apple2/memory.c', 'targets/apple2/pixelDHR.c', 'targets/apple2/pixelSHR.c' ]                            
                    sTarget = [ 'strings/chars.s', 'targets/apple2/blitCharmap.s', 'targets/apple2/blitSprite.s', 'targets/apple2/decrunch.s', 'targets/apple2/DUET.s', 'targets/apple2/hiresLines.s', 'targets/apple2/mocking.s', 'targets/apple2/paddle.s', 'targets/apple2/prodos.s', 'targets/apple2/scroll.s', 'targets/apple2/serial.s' ]
                    symbols = ' -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_AppleSpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_AppleSpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_AppleSpriteHeight.get()
                    
                    # Network settings
                    if network == '8bit-Hub': 
                        cTarget.append('adaptors/hub.c')
                        executable = 'HUB'
                        symbols += ' -D __HUB__ '
                    elif network == 'Uthernet': 
                        executable = 'UTHERNET'                    
                        symbols += ' -D __IP65__ '
                    elif network == 'No-Net': 
                        executable = 'NONET'                    
                        
                    # Graphic settings
                    if graphics == 'double':
                        symbols += ' -D __DHR__'
                    else:
                        symbols += ' -D __SHR__'
                    if self.combobox_AppleCrunchAssets.get() == 'Yes':
                        symbols += ' -D __DECRUNCH__'   

                    # Compile Unity library
                    library = buildFolder + '/[libs]/unity-apple' + target + '-' + executable.lower() + '.lib'
                    BuildUnityLibrary(self, fp, '-t apple2', symbols, cCore+cTarget, sCore+sTarget, library)

                    # Compile Program
                    symbols += ' -Wl -D,__STACKSIZE__=' + addr + '$0400,-D,__HIMEM__=' + addr + '$B800,-D,__LCADDR__=' + addr + '$D000,-D,__LCSIZE__=' + addr + '$1000,-D,CHUNKSIZE=' + addr + chunkSize
                    comp = cl65 + ' -o ' + buildFolder + '/apple/' + executable + '.bin -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-apple' + target + '-' + executable.lower() + '.map -Cl -O -t apple2 ' + symbols + ' -C apple2-hgr.cfg -I unity '
                    for item in code:
                        comp += item + ' '
                    comp += library + ' '
                    if network == 'Uthernet': 
                        if self.combobox_AppleNetworkProtocols.get() == 'TCP/UDP':
                            comp += 'unity/adaptors/ip65_tcp.lib unity/adaptors/ip65_apple2.lib'
                        else:
                            comp += 'unity/adaptors/ip65.lib unity/adaptors/ip65_apple2.lib'                       
                    fp.write(comp + '\n\n')

                    # Compress Program
                    fp.write(ex31 + ' sfx bin ' + buildFolder + '/apple/' + executable + '.bin -B -o ' + buildFolder + '/apple/' + executable + '\n\n')                        
                    
                # Create loader program?
                if len(networkOptions) > 1:
                    # Compile loader
                    symbols = '-Cl -O -t apple2 -C apple2-hgr.cfg '
                    if '8bit-Hub' in networkOptions:
                        symbols += '-D __HUB__ '
                    if 'Uthernet' in networkOptions:
                        symbols += '-D __IP65__ '
                    fp.write(cl65 + ' -o ' + buildFolder + '/apple/LOADER.bin ' + symbols + ' -I unity unity/targets/apple2/loader.c ' + library + '\n\n')
                   
                    # Compress loader
                    fp.write(ex31 + ' sfx bin ' + buildFolder + '/apple/LOADER.bin -B -o ' + buildFolder + '/apple/LOADER\n\n')

                # Clean-up build folder
                fp.write(Remove(buildFolder + '/apple/*.bin'))

                fp.write('\necho --------------- CONVERT ASSETS ---------------  \n\n')

                # Bitmaps
                for item in bitmaps:
                    if self.combobox_AppleCrunchAssets.get() == 'Yes':
                        fp.write(py27 + ' utils/scripts/apple/AppleBitmap.py ' + graphics + ' crunch ' + item + ' ' + buildFolder + '/apple/' + FileBase(item, '.png') + '.img\n')
                    else:
                        fp.write(py27 + ' utils/scripts/apple/AppleBitmap.py ' + graphics + ' raw ' + item + ' ' + buildFolder + '/apple/' + FileBase(item, '.png') + '.img\n')

                # Charset
                for item in charset:
                    fb = FileBase(item, '.png')
                    fp.write(py27 + ' utils/scripts/apple/AppleCharset.py ' + graphics + ' ' + item + ' ' + buildFolder + '/apple/' + fb + '.chr\n')
                    
                # Sprites
                if len(sprites) > 0:
                    spriteHeight = int(self.entry_AppleSpriteHeight.get())
                    fp.write(py27 + ' utils/scripts/apple/AppleSprites.py ' + graphics + ' ' + sprites[0] + ' ' + buildFolder + '/apple/sprites.dat ' + str(spriteHeight) + '\n')

                # Chunks
                for item in chunks:
                    fp.write(py27 + ' utils/scripts/ProcessChunks.py apple-' + graphics + ' ' + item + ' ' + buildFolder + '/apple/\n')

                # Shared Data
                for item in shared:
                    fp.write(Copy(item, buildFolder+'/apple/'+FileBase(item, '')))
                    
                fp.write('\necho --------------- BUILD DISK --------------- \n\n')
                                
                # Disk builder
                if self.combobox_AppleDiskSize.get() == '140KB':                
                    podisk = 'ProDOS190-140K.po'
                    par = '-s7 empty -d1'
                    ext = '.do'
                else:
                    podisk = 'ProDOS190-800K.po'
                    par = '-h1'
                    ext = '.po'
                fp.write(Copy('utils/scripts/apple/' + podisk, buildFolder + '/' + diskname + '-apple' + target + ext))
                
                # Add executables
                if len(networkOptions) > 1: 
                    # Package multiple executables + loader program
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -as ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' LOADER bin 0x0803 < ' + buildFolder + '/apple/LOADER\n')
                    for network in networkOptions:
                        if network == '8bit-Hub': 
                            executable = 'HUB'
                        if network == 'Uthernet': 
                            executable = 'UTHERNET'  
                        if network == 'No-Net': 
                            executable = 'NONET'                            
                        fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -as ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + executable + ' bin 0x0803 < ' + buildFolder + '/apple/' + executable + '\n')
                else:
                    # Only package single executable
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -as ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' LOADER bin 0x0803 < ' + buildFolder + '/apple/' + executable + '\n')
                    
                # Add assets    
                if len(sprites) > 0:
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' SPRITES.DAT bin < ' + buildFolder + '/apple/sprites.dat\n')
                for item in bitmaps:
                    fb = FileBase(item, '.png')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + '.IMG bin < ' + buildFolder + '/apple/' + fb + '.img\n')
                for item in charset:
                    fb = FileBase(item, '.png')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + '.CHR bin < ' + buildFolder + '/apple/' + fb + '.chr\n')
                for item in charmaps:
                    fb = FileBase(item, '')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + ' bin < ' + item + '\n')
                for item in chunks:
                    fb = FileBase(item, '.txt')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + '.CHK bin < ' + buildFolder + '/apple/' + fb + '.chk\n')
                for item in music:
                    fb = FileBase(item, '.m')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + '.MUS bin < ' +item + '\n')
                for item in shared:
                    fb = FileBase(item, '')
                    fp.write(java + ' -jar utils/scripts/apple/AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + ext + ' ' + fb.upper() + ' bin < ' + buildFolder + '/apple/' + fb + '\n')

                fp.write('\necho --------------- APPLE DISK READY --------------- \n\n')
                
                # Start emulator?
                if callEmu:
                    fp.write(Emulate('utils/emulators/AppleWin-1.29.16.0', 'Applewin.exe ' + par,  diskname + '-apple' + target + ext))
            
        ####################################################
        # Atari script
        bitmaps = list(self.listbox_AtariBitmap.get(0, END))
        charset = list(self.listbox_AtariCharset.get(0, END))
        sprites = list(self.listbox_AtariSprites.get(0, END))
        chunks = list(self.listbox_AtariChunks.get(0, END))
        music = list(self.listbox_AtariMusic.get(0, END))
        chunkSize = self.entry_AtariChunkMemory.get().replace('$0000','$0001')
        networkOptions = []
        if self.checkbutton_AtariNetwork8bitHub.get():
            networkOptions.append('8bit-Hub')
        if self.checkbutton_AtariNetworkDragonCart.get():
            networkOptions.append('DragonCart')
        if self.checkbutton_AtariNetworkFujinet.get():
            networkOptions.append('Fujinet')
        if len(networkOptions) == 0:
            networkOptions.append('No-Net')                    
        
        # Build 48 and 64k Versions
        for target in ['48k', '64k']:

            # Bitmap encoding mode
            if target == '64k':
                graphics = 'double'
            else:
                graphics = 'single'
        
            with open('../../' + buildFolder+'/'+diskname+"-atari"+target+sext, "wb") as fp:
                # Info
                fp.write('echo off\n\n')
                fp.write('mkdir atari\n')            
                fp.write('mkdir [libs]\n')            
                fp.write('mkdir [maps]\n')            
                fp.write('cd ..\n\n')
                fp.write(Remove('build/atari/*.*'))
                
                fp.write('\necho --------------- COMPILE PROGRAM ---------------\n\n')

                # Build Unity Library for eah network target
                for network in networkOptions:
                    cTarget = [ 'graphics/pixel.c', 'targets/atari/directory.c', 'targets/atari/display.c', 'targets/atari/files.c', 'targets/atari/pmg.c' ]
                    sTarget = [ 'graphics/scroll.s', 'strings/chars.s', 'targets/atari/blitCharmap.s', 'targets/atari/blitSprites.s', 'targets/atari/decrunch.s', 'targets/atari/DLIST-bmp.s', 'targets/atari/DLIST-chr.s', 'targets/atari/DLIST-plx.s', 'targets/atari/DLI.s', 'targets/atari/ROM.s', 'targets/atari/VBI.s', 'targets/atari/xbios.s' ]
                    symbols = ' -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_AtariSpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_AtariSpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_AtariSpriteHeight.get()
                    
                    # Network settings
                    if network == '8bit-Hub': 
                        cTarget.append('adaptors/hub.c')
                        sTarget.append('targets/atari/PIA.s')
                        symbols += ' -D __HUB__'                        
                        executable = 'hub.xex'
                    elif network == 'DragonCart':    
                        symbols += ' -D __IP65__'                        
                        executable = 'dragon.xex'
                    elif network == 'Fujinet':    
                        cTarget.append('targets/atari/fujinet.c')
                        sTarget.append('targets/atari/fujiIRQ.s')
                        symbols += ' -D __FUJINET__'
                        executable = 'fujinet.xex'
                    elif network == 'No-Net': 
                        executable = 'nonet.xex'  
                        
                    # Graphic settings                
                    if self.combobox_AtariCrunchAssets.get() == 'Yes':
                        symbols += ' -D __DECRUNCH__'  
                        
                    # Compile Unity library
                    library = buildFolder + '/[libs]/unity-atari' + target + '-' + executable[0:-4] + '.lib'
                    if target == '48k':
                        BuildUnityLibrary(self, fp, ' -t atari', symbols, cCore+cTarget, sCore+sTarget, library)
                        symbols += ' -Cl -O -t atari ' 
                    else:
                        BuildUnityLibrary(self, fp, ' -t atarixl', symbols, cCore+cTarget, sCore+sTarget, library)
                        symbols += ' -Cl -O -t atarixl -C atarixl-largehimem.cfg '           
                        
                    # Compile Program
                    if len(networkOptions) > 1:
                        symbols += ' -Wl -D__SYSTEM_CHECK__=1 ' # Loader will perform that check
                    symbols += ' -Wl -D,__STACKSIZE__=' + addr + '$0400 -Wl -D,CHUNKSIZE=' + addr + chunkSize
                    comp = cl65 + ' -o ' + buildFolder + '/atari/' + executable + ' -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-atari' + target + '-' + executable[0:-4] + '.map ' + symbols + ' -I unity '
                    for item in code:
                        comp += (item + ' ')
                    comp += 'unity/targets/atari/POKEY.s ' + library + ' '
                    if network == 'DragonCart':
                        if target == '48k':
                            if self.combobox_AtariNetworkProtocols.get() == 'TCP/UDP':
                                comp += 'unity/adaptors/ip65_tcp.lib unity/adaptors/ip65_atari.lib'
                            else:
                                comp += 'unity/adaptors/ip65.lib unity/adaptors/ip65_atari.lib'
                        else:
                            if self.combobox_AtariNetworkProtocols.get() == 'TCP/UDP':
                                comp += 'unity/adaptors/ip65_tcp.lib unity/adaptors/ip65_atarixl.lib'
                            else:
                                comp += 'unity/adaptors/ip65.lib unity/adaptors/ip65_atarixl.lib'
                    fp.write(comp + '\n')
                    fp.write(py27 + ' utils/scripts/atari/AtariCompress.py ' + buildFolder + '/atari/' + executable + ' \n\n')
                
                # Include loader program?
                if len(networkOptions) > 1:                                    
                    if target == '48k':
                        symbols = '-Cl -O -t atari '
                    else:
                        symbols = '-Cl -O -t atarixl -C atarixl-largehimem.cfg '
                    if '8bit-Hub' in networkOptions:
                        symbols += '-D __HUB__ '
                    if 'DragonCart' in networkOptions:
                        symbols += '-D __IP65__ '
                    if 'Fujinet' in networkOptions:
                        symbols += '-D __FUJINET__ '                
                    fp.write(cl65 + ' -o ' + buildFolder + '/atari/loader.bin ' + symbols + ' -I unity unity/targets/atari/loader.c ' + library + '\n')
                    fp.write(py27 + ' utils/scripts/atari/AtariCompress.py ' + buildFolder + '/atari/loader.bin \n\n')

                # RMT player
                if target == '64k':
                    fp.write(mads + ' -o:' + buildFolder + '/atari/rmt.bin unity/targets/atari/RMT.a65 \n')
                    fp.write(py27 + ' utils/scripts/atari/AtariCompress.py ' + buildFolder + '/atari/rmt.bin \n\n')

                # BASIC disabler
                fp.write(cl65 + ' -o ' + buildFolder + '/atari/basicoff.bin -t atari -C atari-asm.cfg unity/targets/atari/BASICOFF.s \n\n')

                # Merging
                cmd = py27 + ' utils/scripts/atari/AtariMerge.py ' + buildFolder + '/atari/xautorun ' + buildFolder + '/atari/basicoff.bin '
                if len(networkOptions) > 1:  
                    cmd += buildFolder + '/atari/loader.bin '
                else:
                    cmd += buildFolder + '/atari/' + executable + ' '
                if target == '64k':
                    cmd += buildFolder + '/atari/rmt.bin'
                fp.write(cmd + '\n\n')
                
                # Clean-up build folder
                fp.write(Remove(buildFolder + '/atari/*.bin'))
                fp.write(Remove(buildFolder + '/atari/*.raw'))
                fp.write(Remove(buildFolder + '/atari/*.zx0'))
                if len(networkOptions) == 1:  
                    fp.write(Remove(buildFolder + '/atari/*.xex'))
                fp.write('\n\n')                
                                                
                fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
                
                # Bitmaps
                for item in bitmaps:
                    if self.combobox_AtariCrunchAssets.get() == 'Yes':
                        fp.write(py27 + ' utils/scripts/atari/AtariBitmap.py ' + graphics + ' crunch ' + item + ' ' + buildFolder + '/atari/' + FileBase(item, '.png') + '.img\n')
                    else:
                        fp.write(py27 + ' utils/scripts/atari/AtariBitmap.py ' + graphics + ' raw ' + item + ' ' + buildFolder + '/atari/' + FileBase(item, '.png') + '.img\n')
                    
                # Charmaps/Tilesets
                for item in charmaps:
                    fp.write(Copy(item, buildFolder + '/atari/' + FileBase(item, '')))
                    
                # Charsets
                for item in charset:
                    fb = FileBase(item, '.png')
                    fp.write(py27 + ' utils/scripts/atari/AtariCharset.py ' + item + ' ' + buildFolder + '/atari/' + fb + '.chr\n')
                    
                # Sprites    
                if len(sprites) > 0:
                    spriteHeight = int(self.entry_AtariSpriteHeight.get())
                    fp.write(py27 + ' utils/scripts/atari/AtariSprites.py ' + sprites[0] + ' ' + buildFolder + '/atari/sprites.dat ' + str(spriteHeight) + '\n')
                    
                # Chunks
                for item in chunks:
                    fp.write(py27 + ' utils/scripts/ProcessChunks.py atari ' + item + ' ' + buildFolder + '/atari/\n')

                # Shared Data
                for item in shared:
                    fp.write(Copy(item, buildFolder + '/atari/' + FileBase(item, '')))

                # Music
                for item in music:
                    fp.write(Copy(item, buildFolder + '/atari/' + FileBase(item, '.rmt') + '.mus'))

                fp.write('\necho --------------- BUILD DISK --------------- \n\n')
                
                # Copy xBios files
                fp.write(Copy('utils/scripts/atari/xbios4.obx', buildFolder + '/atari/autorun'))

                # Disk builder
                if self.combobox_AtariDiskSize.get() == '360KB':                
                    fp.write(datr + ' -md -B utils/scripts/atari/xboot.obx 1440 ' + buildFolder + '/' + diskname + '-atari' + target + '.atr ' + buildFolder + '/atari\n')
                elif self.combobox_AtariDiskSize.get() == '180KB':                
                    fp.write(datr + ' -mD -B utils/scripts/atari/xboot.obx ' + buildFolder + '/' + diskname + '-atari' + target + '.atr ' + buildFolder + '/atari\n')
                elif self.combobox_AtariDiskSize.get() == '130KB':                
                    fp.write(datr + ' -mE -B utils/scripts/atari/xboot.obx ' + buildFolder + '/' + diskname + '-atari' + target + '.atr ' + buildFolder + '/atari\n')
                elif self.combobox_AtariDiskSize.get() == '90KB':                
                    fp.write(datr + ' -mS -B utils/scripts/atari/xboot.obx ' + buildFolder + '/' + diskname + '-atari' + target + '.atr ' + buildFolder + '/atari\n')

                fp.write('\necho --------------- ATARI DISK READY --------------- \n\n')
                
                # Start emulator?
                if callEmu:
                    fp.write(Emulate('utils/emulators/Altirra-4.00-Hub', 'Altirra.exe', diskname + '-atari' + target + '.atr'))             

        ####################################################
        # C64 script
        bitmaps = list(self.listbox_C64Bitmap.get(0, END))
        charset = list(self.listbox_C64Charset.get(0, END))
        sprites = list(self.listbox_C64Sprites.get(0, END))
        chunks = list(self.listbox_C64Chunks.get(0, END))
        music = list(self.listbox_C64Music.get(0, END))
        chunkSize = self.entry_C64ChunkMemory.get().replace('$0000','$0001')
        networkOptions = []
        if self.checkbutton_C64Network8bitHub.get():
            networkOptions.append('8bit-Hub')
        if self.checkbutton_C64NetworkRRNet.get():
            networkOptions.append('RR-Net')
        if self.checkbutton_C64NetworkUltimate.get():
            networkOptions.append('Ultimate')
        if len(networkOptions) == 0:
            networkOptions.append('No-Net')                    
        files = []
            
        with open('../../' + buildFolder+'/'+diskname+"-c64"+sext, "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir c64\n')            
            fp.write('mkdir [libs]\n')            
            fp.write('mkdir [maps]\n')            
            fp.write('cd ..\n\n')            
            fp.write(Remove('build/c64/*.*'))
            for suffix in ["", "-d1", "-d2", "-d3", "-d4", "-d5", "-d6", "-d7", "-d8", "-d9"]:
                fp.write(Remove('build/' + diskname + '-c64' + suffix + '.d64'))
            
            fp.write('\necho --------------- COMPILE PROGRAM ---------------\n\n')
            
            # Convert Sprites (need to merge with program)
            if len(sprites) > 0:
                fp.write(py27 + ' utils/scripts/c64/C64Sprites.py ' + sprites[0] + ' ' + buildFolder + '/c64/sprites.dat\n')            

            # Build Unity Library for each network target
            for network in networkOptions:
                cTarget = [ 'graphics/pixel.c', 'targets/c64/directory.c', 'targets/c64/files.c', 'targets/c64/VIC2.c' ]
                sTarget = [ 'graphics/scroll.s', 'strings/chars.s', 'targets/c64/decrunch.s', 'targets/c64/DLI.s', 'targets/c64/blitCharmap.s', 'targets/c64/ROM.s', 'targets/c64/SID.s']
                symbols = ' -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_C64SpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_C64SpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_C64SpriteHeight.get()
                
                # Network settings
                if network == '8bit-Hub': 
                    cTarget.append('adaptors/hub.c')
                    sTarget.append('targets/c64/CIA.s')
                    symbols += ' -D __HUB__ '            
                    executable = 'hub'
                if network == 'RR-Net': 
                    sTarget.append('targets/c64/joystick.s')
                    symbols += ' -D __IP65__ '
                    executable = 'rrnet'
                if network == 'Ultimate': 
                    sTarget.append('targets/c64/joystick.s')
                    cTarget.append('targets/c64/ultimate.c')
                    symbols += ' -D __ULTIMATE__ '
                    executable = 'ultimate'
                if network == 'No-Net': 
                    sTarget.append('targets/c64/joystick.s')
                    executable = 'nonet'
                                        
                # Graphic settings
                if self.combobox_C64CrunchAssets.get() == 'Yes':
                    symbols += ' -D __DECRUNCH__ '
                                    
                # Compile Unity library
                library = buildFolder + '/[libs]/unity-c64' + '-' + executable + '.lib'
                BuildUnityLibrary(self, fp, '-t c64', symbols, cCore+cTarget, sCore+sTarget, library)

                # Only 1 network setting?
                if len(networkOptions) == 1:
                    executable = 'loader'
                files.append(executable + '.prg')
                    
                # Compile Program                        
                symbols += ' -Wl -D,CHUNKSIZE=' + addr + chunkSize
                comp = cl65 + ' -o ' + buildFolder + '/c64/' + executable + '.bin -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-c64-' + executable + '.map -Cl -O -t c64 -C unity/targets/c64/c64.cfg ' + symbols + ' -I unity '
                for item in code:
                    comp += (item + ' ')
                comp += library + ' '
                if network == 'RR-Net': 
                    if self.combobox_C64NetworkProtocols.get() == 'TCP/UDP':
                        comp += 'unity/adaptors/ip65_tcp.lib unity/adaptors/ip65_c64.lib'
                    else:
                        comp += 'unity/adaptors/ip65.lib unity/adaptors/ip65_c64.lib'
                fp.write(comp + '\n\n')
                
                # Compress Program
                if len(sprites) > 0:
                    #fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/' + executable + '.bin unity/targets/c64/krill-loader.prg ' + buildFolder + '/c64/sprites.dat -B -o ' + buildFolder + '/c64/' + executable + '.prg\n\n')          
                    fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/' + executable + '.bin ' + buildFolder + '/c64/sprites.dat -B -o ' + buildFolder + '/c64/' + executable + '.prg\n\n')          
                else:
                    #fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/' + executable + '.bin unity/targets/c64/krill-loader.prg -B -o ' + buildFolder + '/c64/' + executable + '.prg\n\n')
                    fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/' + executable + '.bin -B -o ' + buildFolder + '/c64/' + executable + '.prg\n\n')
                
            # Include loader program?
            if len(networkOptions) > 1:   
                # Compile Loader
                files.append('loader.prg')
                symbols = '-Cl -O -t c64 '
                if '8bit-Hub' in networkOptions:
                    symbols += '-D __HUB__ '
                if 'RR-Net' in networkOptions:
                    symbols += '-D __IP65__ '
                if 'Ultimate' in networkOptions:
                    symbols += '-D __ULTIMATE__ '
                fp.write(cl65 + ' -o ' + buildFolder + '/c64/loader.bin ' + symbols + ' -C unity/targets/c64/c64.cfg -I unity unity/targets/c64/loader.c ' + library + '\n\n')
                
                # Compress Loader
                #fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/loader.bin unity/targets/c64/krill-install.prg unity/targets/c64/krill-loader.prg  -B -o ' + buildFolder + '/c64/loader.prg\n\n')
                fp.write(ex30 + ' sfx ' + addr + '$180d ' + buildFolder + '/c64/loader.bin -B -o ' + buildFolder + '/c64/loader.prg\n\n')

            # Clean-up build folder
            fp.write(Remove(buildFolder + '/c64/*.bin'))
            
            fp.write('\necho --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps
            for item in bitmaps:
                fb = FileBase(item, '.png')
                if self.combobox_C64CrunchAssets.get() == 'Yes':
                    fp.write(py27 + ' utils/scripts/c64/C64Bitmap.py crunch ' + item + ' ' + buildFolder + '/c64/' + fb + '.img\n')
                else:
                    fp.write(py27 + ' utils/scripts/c64/C64Bitmap.py raw ' + item + ' ' + buildFolder + '/c64/' + fb + '.img\n')
                files.append(fb + '.img')

            # Charmaps/Tilesets
            for item in charmaps:
                fb = FileBase(item, '')
                fp.write(Copy(item, buildFolder + '/c64/' + fb))
                files.append(fb)
                    
            # Charsets 
            for item in charset:
                fb = FileBase(item, '.png')
                fp.write(py27 + ' utils/scripts/c64/C64Charset.py ' + item + ' ' + buildFolder + '/c64/' + fb + '.chr' + ' ' + self.entry_C64CharsetColors.get() + '\n')
                files.append(fb + '.chr')                        
            
            # Chunks
            for item in chunks:
                fb = FileBase(item, 'txt')
                fp.write(py27 + ' utils/scripts/ProcessChunks.py c64 ' + item + ' ' + buildFolder + '/c64/\n\n')
                files.append(fb + '.chk')
            
            # Music
            for item in music:
                fb = FileBase(item, '.sid')
                fp.write(sidr + ' -v -z 30-ff -p 08 ' + item + ' ' + buildFolder + '/c64/' + fb + '.mus\n')
                if "nt" == os.name:
                    fp.write('if not exist ' + buildFolder + '/c64/' + fb + '.mus (\n')
                    fp.write('    ' + 'echo Relocation impossible, using the original file instead...\n')
                    fp.write('    ' + Copy(item, buildFolder + '/c64/' + fb + '.mus') + ')')
                else:
                    fp.write('if [ ! -a ' + buildFolder + '/c64/' + fb + '.mus ]; then \n')
                    fp.write('    ' + 'echo Relocation impossible, using the original file instead...\n')
                    fp.write('    ' + Copy(item, buildFolder + '/c64/' + fb + '.mus'))
                    fp.write('fi')                
                fp.write('\n')
                files.append(fb + '.mus')           
            
            # Shared Data
            for item in shared:
                fb = FileBase(item, '')
                fp.write(Copy(item, buildFolder + '/c64/' + fb))
                files.append(fb)
                
            fp.write('\necho --------------- BUILD DISK --------------- \n\n')

            # Call file packager script
            fp.write(py27 + ' utils/scripts/c64/C64Disk.py ' + buildFolder + ' ' + diskname + ' ' + ' '.join(files) + '\n\n')

            fp.write('echo --------------- C64 DISK READY --------------- \n\n')
            
            # Start emulator?
            if callEmu:
                fp.write(Emulate('utils/emulators/WinVICE-3.2', 'x64.exe', diskname + '-c64.d64'))   

        ####################################################
        # Lynx script
        bitmaps = list(self.listbox_LynxBitmap.get(0, END))
        charset = list(self.listbox_LynxCharset.get(0, END))
        sprites = list(self.listbox_LynxSprites.get(0, END))
        chunks = list(self.listbox_LynxChunks.get(0, END))
        music = list(self.listbox_LynxMusic.get(0, END))
        chunkSize = self.entry_LynxChunkMemory.get().replace('$0000','$0001')
        
        with open('../../' + buildFolder+'/'+diskname+"-lynx"+sext, "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir lynx\n')           
            fp.write('mkdir [libs]\n')            
            fp.write('mkdir [maps]\n')            
            fp.write('cd ..\n\n')            
            fp.write(Remove(buildFolder + '/lynx/*.*'))
            
            fp.write('\necho --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps
            for item in bitmaps:
                fp.write(py27 + ' utils/scripts/lynx/LynxBitmap.py ' + item + ' ' + buildFolder + '/lynx/' + FileBase(item, '.png') + '.img\n')
            if len(bitmaps) > 0:
                fp.write('\n')
                
            # Charsets
            for item in charset:
                fb = FileBase(item, '.png')
                fp.write(py27 + ' utils/scripts/lynx/LynxCharset.py ' + item + ' ' + buildFolder + '/lynx/' + fb + '.chr\n')
                fp.write('\n')
                
            # Sprites
            if len(sprites) > 0:
                spriteWidth  = int(self.entry_LynxSpriteWidth.get())
                spriteHeight = int(self.entry_LynxSpriteHeight.get())
                fp.write(py27 + ' utils/scripts/lynx/LynxSprites.py ' + sprites[0] + ' ' + buildFolder + '/lynx/sprites.dat ' + str(spriteWidth) + ' ' + str(spriteHeight) + ' literal\n')
                fp.write('\n')

            # Chunks
            for item in chunks:
                fp.write(py27 + ' utils/scripts/ProcessChunks.py lynx ' + item + ' ' + buildFolder + '/lynx/\n')
                fp.write('\n')

            # Charmaps/Tilesets
            if len(charmaps) > 0:
                for item in charmaps:
                    fp.write(Copy(item, buildFolder + '/lynx/' + FileBase(item, '')))
                fp.write('\n')

            # Virtual Keyboard
            if self.checkbutton_LynxVirtualKeyboard.get():
                fp.write(Copy('utils/scripts/lynx/cursor.spr', buildFolder + '/lynx/cursor.dat'))
                fp.write(Copy('utils/scripts/lynx/keyboard.spr', buildFolder + '/lynx/keyboard.dat'))
            fp.write('\n')
                                
            # Transcribe Music data
            fp.write(Copy('unity/targets/lynx/chipper.s', buildFolder + '/lynx/soundbs.mac'))
            for i in range(len(music)):
                fb = FileBase(music[i], '.asm')
                fp.write(py27 + ' utils/scripts/lynx/LynxChipper.py ' + music[i] + ' ' + buildFolder + '/lynx/' + fb + '.asm _musData' + str(i).zfill(2) + ' MUS' + str(i) + 'DATA \n')
            fp.write('\n')

            # Copy Shared files
            if len(shared) > 0:             
                for item in shared:
                    fp.write(Copy(item, buildFolder + '/lynx/' + FileBase(item, '')))
                fp.write('\n')

            fp.write('echo ---------------- LINK ASSETS ----------------\n\n')

            # Make lists of asset files
            bmpList = '"'
            for item in bitmaps:
                bmpList += FileBase(item, '.png') + '.img,'
            if len(bmpList) > 1: bmpList = bmpList[:-1]
            bmpList += '"'    

            mapList = '"'
            for item in charmaps:
                mapList += FileBase(item, '') + ','
            if len(mapList) > 1: mapList = mapList[:-1]
            mapList += '"'    

            chrList = '"'    
            for item in charset:
                chrList += FileBase(item, '.png') + '.chr,'
            if len(chrList) > 1: chrList = chrList[:-1]
            chrList += '"'    

            musList = '"'    
            for item in music:
                musList += FileBase(item, '.asm') + '.asm,'
            if len(musList) > 1: musList = musList[:-1]
            musList += '"'    

            shrList = '"'
            for item in shared:
                shrList += FileBase(item, '') + ','
            if len(shrList) > 1: shrList = shrList[:-1]
            shrList += '"'    

            chkList = '"'    
            for item in chunks:
                chkList += FileBase(item, '.txt') + '.chk,'
            if len(chkList) > 1: chkList = chkList[:-1]
            chkList += '"'    
            
            if len(sprites) > 0:
                sprList = '"' + FileBase(sprites[0], '') + '.dat"'
            else:
                sprList = '""'
                
            # Generate data, config and directory Files
            fp.write(py27 + ' utils/scripts/lynx/LynxData.py ' + buildFolder + '/lynx/data.asm ' + str(int(self.checkbutton_LynxVirtualKeyboard.get())) + ' ' + bmpList + ' ' + mapList + ' ' + chrList + ' ' + musList + ' ' + shrList + ' ' + chkList + ' ' + sprList + '\n')
            if "nt" == os.name:
                fp.write(py27 + ' utils/scripts/lynx/LynxConfig.py unity/targets/lynx/lynx.cfg ' + buildFolder + '/lynx/lynx.cfg ' + self.entry_LynxMusicMemory.get() + ' ' + self.entry_LynxSharedMemory.get() + ' ' + chunkSize + ' ' + str(len(bitmaps)+len(charmaps)+len(charset)) + ' ' + str(len(music)) + ' ' + str(len(shared)) + ' ' + str(len(chunks)) + '\n')
            else:
                fp.write(py27 + ' utils/scripts/lynx/LynxConfig.py unity/targets/lynx/lynx.cfg ' + buildFolder + '/lynx/lynx.cfg \\' + self.entry_LynxMusicMemory.get() + ' \\' + self.entry_LynxSharedMemory.get() + ' \\' + chunkSize + ' ' + str(len(bitmaps)+len(charmaps)+len(charset)) + ' ' + str(len(music)) + ' ' + str(len(shared)) + ' ' + str(len(chunks)) + '\n')
            fp.write(py27 + ' utils/scripts/lynx/LynxDirectory.py unity/targets/lynx/directory.s ' + buildFolder + '/lynx/directory.asm ' + str(len(bitmaps)+len(charmaps)+len(charset)) + ' ' + str(len(music)) + ' ' + str(len(shared)) + ' ' + str(len(chunks)) + '\n')
            fp.write('\n')
            
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            cTarget = [ 'adaptors/hub.c', 'graphics/pixel.c', 'targets/lynx/cgetc.c', 'targets/lynx/display.c', 'targets/lynx/files.c', 'targets/lynx/keyboard.c', 'targets/lynx/screen.c', 'targets/lynx/text.c' ]
            sTarget = [ 'graphics/scroll.s', 'strings/chars.s', 'targets/lynx/header.s', 'targets/lynx/blitCharmap.s', 'targets/lynx/serial.s', 'targets/lynx/suzy.s' ]
            symbols = ' -D __HUB__ -D MUSICSIZE='  + self.entry_LynxMusicMemory.get().replace('$','0x') + ' -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SHAREDSIZE='  + self.entry_LynxSharedMemory.get().replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_LynxSpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_LynxSpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_LynxSpriteHeight.get()
            if self.checkbutton_LynxVirtualKeyboard.get():
                symbols += ' -D __KEYBOARD__'

            # Build Unity Library
            library = buildFolder + '/[libs]/unity-lynx-hub.lib'
            BuildUnityLibrary(self, fp, '-t lynx --cpu 65SC02', symbols, cCore+cTarget, sCore+sTarget, library)
                                     
            # Compile Program 
            symbols += ' -Wl -D,MUSICSIZE=' + addr + self.entry_LynxMusicMemory.get() + ' -Wl -D,CHUNKSIZE=' + addr + chunkSize + ',-D,SHAREDSIZE=' + addr + self.entry_LynxSharedMemory.get()
            comp = cl65 + ' -o ' + buildFolder + '/' + diskname.lower() + '-lynx.lnx -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-lynx-hub.map -Cl -O -t lynx' + symbols + ' -C ' + buildFolder + '/lynx/lynx.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            for item in music:
                comp += buildFolder + '/lynx/' + FileBase(item, '.asm') + '.asm '
            fp.write(comp + 'unity/targets/lynx/sfx.s ' + buildFolder + '/lynx/directory.asm ' + buildFolder + '/lynx/data.asm ' + library + '\n')
            fp.write('\n')
                        
            fp.write('echo --------------- LYNX ROM READY --------------- \n\n')
            
            # Start emulator?
            if callEmu:
                fp.write(Emulate('utils/emulators/Handy-0.98-Hub', 'handy.exe', diskname + '-lynx.lnx'))   
                

        ####################################################
        # NES script
        bitmaps = list(self.listbox_NESBitmap.get(0, END))
        charset = list(self.listbox_NESCharset.get(0, END))
        sprites = list(self.listbox_NESSprites.get(0, END))
        chunks = list(self.listbox_NESChunks.get(0, END))
        music = list(self.listbox_NESMusic.get(0, END))
        maxTiles = int(self.entry_NESBitmapTiles.get())
        chunkSize = self.entry_NESChunkMemory.get().replace('$0000','$0001')
        
        with open('../../' + buildFolder+'/'+diskname+"-nes"+sext, "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir nes\n')            
            fp.write('mkdir [libs]\n')            
            fp.write('mkdir [maps]\n')            
            fp.write('cd ..\n\n') 
            fp.write(Remove(buildFolder + '/nes/*.*'))            
            
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Process Bitmaps / Chunks / Sprites / Shared                         
            if len(bitmaps) > 0:
                for item in bitmaps:
                    fb = FileBase(item, '.png')
                    fp.write(py27 + ' utils/scripts/nes/NESBitmap.py ' + item + ' ' + buildFolder + '/nes/'+ fb + '.chr ' + buildFolder + '/nes/'+ fb + '.img ' + str(maxTiles) + '\n')
                fp.write('\n')

            if len(charmaps) > 0:
                for item in charmaps:
                    fp.write(Copy(item, buildFolder + '/nes/' + FileBase(item, '')))
                fp.write('\n')
                
            if len(charset) > 0:
                for item in charset:
                    fb = FileBase(item, '.png')
                    fp.write(py27 + ' utils/scripts/nes/NESCharset.py ' + item + ' ' + buildFolder + '/nes/'+ fb + '.chr ' + buildFolder + '/nes/'+ fb + '.dat\n')
                fp.write('\n')
            
            if len(music) > 0:            
                for item in music:
                    target = buildFolder + '/nes/' + FileBase(item, '.txt') + '.txt'
                    fp.write(Copy(item, target))
                    fp.write(famt + ' -ca65 ' + target + '\n')
                    fp.write(Remove(target) + '\n')

            if len(chunks) > 0:            
                for item in chunks:
                    fp.write(py27 + ' utils/scripts/nes/NESChunks.py ' + item + ' ' + buildFolder + '/nes/ ' + str(maxTiles) + '\n')
                fp.write('\n')                

            if len(shared) > 0:             
                for item in shared:
                    fp.write(Copy(item, buildFolder + '/nes/' + FileBase(item, '')))
                fp.write('\n')

            if len(sprites) > 0:
                spriteWidth  = int(self.entry_NESSpriteWidth.get())
                spriteHeight = int(self.entry_NESSpriteHeight.get())
                fp.write(py27 + ' utils/scripts/nes/NESSprites.py ' + sprites[0] + ' ' + buildFolder + '/nes/sprites.chr ' + str(spriteWidth) + ' ' + str(spriteHeight) + '\n')
            else:
                fp.write(Copy('utils/scripts/nes/font.chr', buildFolder + '/nes/sprites.chr'))
            fp.write('\n')

            # Copy default charset (font only)        
            fp.write(Copy('utils/scripts/nes/font.chr', buildFolder + '/nes/font.chr'))
            
            fp.write('\necho ---------------- LINK ASSETS ----------------  \n\n')

            # Make lists of asset files
            bmpList = '"'
            for item in bitmaps:
                bmpList += FileBase(item, '.png') + '.img,'
            if len(bmpList) > 1: bmpList = bmpList[:-1]
            bmpList += '"'    

            mapList = '"'
            for item in charmaps:
                mapList += FileBase(item, '') + ','
            if len(mapList) > 1: mapList = mapList[:-1]
            mapList += '"'    

            chrList = '"'    
            for item in charset:
                chrList += FileBase(item, '.png') + '.dat,'
            if len(chrList) > 1: chrList = chrList[:-1]
            chrList += '"'    

            musList = '"'    
            for item in music:
                musList += FileBase(item, '.txt') + '.s,'
            if len(musList) > 1: musList = musList[:-1]
            musList += '"'    

            shrList = '"'
            for item in shared:
                shrList += FileBase(item, '') + ','
            if len(shrList) > 1: shrList = shrList[:-1]
            shrList += '"'    

            chkList = '"'    
            for item in chunks:
                chkList += FileBase(item, '.txt') + '.chk,'
            if len(chkList) > 1: chkList = chkList[:-1]
            chkList += '"'    
                
            # Generate data, config and directory Files
            fp.write(py27 + ' utils/scripts/nes/NESData.py ' + buildFolder + '/nes/data.asm ' + bmpList + ' ' + mapList + ' ' + chrList + ' ' + musList + ' ' + shrList + ' ' + chkList + '\n')            
            fp.write('\n')
            
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            cTarget = [ 'adaptors/hub.c', 'graphics/pixel.c', 'targets/nes/cgetc.c', 'targets/nes/conio.c', 'targets/nes/display.c', 'targets/nes/files.c', 'targets/nes/keyboard.c', 'targets/nes/memory.c', 'targets/nes/text.c' ]
            sTarget = [ 'targets/nes/blitCharmap.s', 'targets/nes/crt0.s', 'targets/nes/expansion.s', 'targets/nes/joypad.s' ]
            symbols = ' -D __HUB__ -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_NESSpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_NESSpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_NESSpriteHeight.get()

            # Build Unity Library
            library = buildFolder + '/[libs]/unity-nes-hub.lib'
            BuildUnityLibrary(self, fp, '-t nes', symbols, cCore+cTarget, sCore+sTarget, library)

            # Compile Program
            symbols += ' -Wl -D,CHUNKSIZE=' + addr + chunkSize
            comp = cl65 + ' -o ' + buildFolder + '/' + diskname.lower() + '-nes.nes -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-nes-hub.map -t nes -Cl -Oirs ' + symbols + ' -C unity/targets/nes/MMC1/MMC1_128_128.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            fp.write(comp  + buildFolder + '/nes/data.asm ' + library + ' nes.lib\n\n')

            fp.write('echo --------------- NES ROM READY --------------- \n\n')

            # Start emulator?
            if callEmu:
                fp.write(Emulate('utils/emulators/FCEUX-2.5.0-Hub', 'fceux.exe', diskname + '-nes.nes'))  
                

        ####################################################
        # Oric script
        bitmaps = list(self.listbox_OricBitmap.get(0, END))
        charset = list(self.listbox_OricCharset.get(0, END))
        sprites = list(self.listbox_OricSprites.get(0, END))
        chunks = list(self.listbox_OricChunks.get(0, END))
        music = list(self.listbox_OricMusic.get(0, END))
        chunkSize = self.entry_OricChunkMemory.get().replace('$0000','$0001')
        
        with open('../../' + buildFolder+'/'+diskname+"-oric48k"+sext, "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir oric\n')            
            fp.write('mkdir [libs]\n')            
            fp.write('mkdir [maps]\n')            
            fp.write('cd ..\n\n')            
            fp.write(Remove(buildFolder + '/oric/*.*'))

            fp.write('\necho --------------- COMPILE PROGRAM ---------------\n\n')

            cTarget = [ 'adaptors/hub.c', 'graphics/pixel.c', 'targets/oric/directory.c', 'targets/oric/files.c' ]
            sTarget = [ 'graphics/scroll.s', 'strings/chars.s', 'targets/oric/blitCharmap.s', 'targets/oric/blitSprite.s', 'targets/oric/paseIJK.s', 'targets/oric/keyboard.s', 'targets/oric/sedoric.s', 'targets/oric/MYM.s', 'targets/oric/VIA.s' ]
            symbols = ' -D __HUB__ -D CHUNKSIZE='  + chunkSize.replace('$','0x') + ' -D SPRITEFRAMES=' + self.entry_OricSpriteFrames.get() + ' -D SPRITEWIDTH=' + self.entry_OricSpriteWidth.get() + ' -D SPRITEHEIGHT=' + self.entry_OricSpriteHeight.get()
    
            # Build Unity Library
            library = buildFolder + '/[libs]/unity-oric48k-hub.lib'
            BuildUnityLibrary(self, fp, '-t atmos', symbols, cCore+cTarget, sCore+sTarget, library)

            # Compile Program
            symbols += ' -Wl -D,CHUNKSIZE=' + addr + chunkSize                     
            comp = cl65 + ' -o ' + buildFolder + '/oric/' + diskname.lower() + '.bin -m ' + buildFolder + '/[maps]/' + diskname.lower() + '-oric48k-hub.map -Cl -O -t atmos' + symbols + ' -C unity/targets/oric/oric.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            fp.write(comp + library + '\n\n')

            # Fix header
            fp.write(orih + ' ' + buildFolder + '/oric/' + diskname.lower() + '.bin ' + buildFolder + '/oric/' + diskname.lower() + '.com ' + addr + '$0501\n\n')

            # Compress program
            fp.write(ex30 + ' sfx bin ' + buildFolder + '/oric/' + diskname.lower() + '.com -B -o ' + buildFolder + '/oric/launch.com\n\n')            
            
            # Clean-up
            fp.write(Remove(buildFolder + '/oric/' + diskname.lower() + '.com'))
            fp.write(Remove(buildFolder + '/oric/' + diskname.lower() + '.bin'))
                        
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            if "nt" == os.name:
                pyth = '..\\..\\py27\\python.exe'
                head = 'header.exe'
            else:
                pyth = 'python2'
                head = 'wine header.exe'
                
            # Process Bitmaps / Chunks / Sprites / Shared
            fp.write(CD('utils/scripts/oric'))
            for item in bitmaps:
                fb = FileBase(item, '.png')
                fp.write(pyth + ' OricBitmap.py ../../../' + item + ' ../../../' + buildFolder + '/oric/' + fb + '.dat ' + self.entry_OricDithering.get() + ' ' + self.entry_OricLeadColors.get() + '\n')
                fp.write(head + ' -a0 ../../../' + buildFolder + '/oric/' + fb + '.dat ../../../' + buildFolder + '/oric/' + fb + '.img ' + addr + '$A000\n')
                fp.write(Remove('../../../' + buildFolder + '/oric/' + fb + '.png'))
                fp.write(Remove('../../../' + buildFolder + '/oric/' + fb + '.dat'))
                
            for item in charset:
                fb = FileBase(item, '.png')
                fp.write(pyth + ' OricCharset.py ../../../' + item + ' ../../../' + buildFolder + '/oric/' + fb + '.dat ' + self.entry_OricDithering.get() +  '\n') 
                fp.write(head + ' -a0 ../../../' + buildFolder + '/oric/' + fb + '.dat ../../../' + buildFolder + '/oric/' + fb + '.chr ' + addr + '$A000\n')
                
                
            for item in chunks:
                fb = FileBase(item, '.txt')
                fp.write(pyth + ' OricChunks.py ../../../' + item + ' ../../../' + buildFolder + '/oric/ ' + self.entry_OricDithering.get() + '\n')
                fp.write(head + ' -a0 ../../../' + buildFolder + '/oric/' + fb + '.dat ../../../' + buildFolder + '/oric/' + fb + '.chk ' + addr + '$8000\n')
            fp.write(CD('../../..'))

            for item in charmaps:
                fb = FileBase(item, '')
                fp.write(orih + ' -a0 ' + item + ' ' + buildFolder + '/oric/' + fb + ' ' + addr + '$A000\n')
            
            if len(sprites) > 0:
                spriteHeight = int(self.entry_OricSpriteHeight.get())
                fp.write(py27 + ' utils/scripts/oric/OricSprites.py ' + sprites[0] + ' ' + buildFolder + '/oric/sprites.dat ' + str(spriteHeight) + '\n')
                fp.write(orih + ' -a0 ' + buildFolder + '/oric/sprites.dat ' + buildFolder + '/oric/sprites.dat ' + addr + '$7800\n')
                
            for item in music:
                fb = FileBase(item, '.ym')
                fp.write(orim + ' ' + item + ' ' + buildFolder + '/oric/' + fb + '.mus\n')
                fp.write(orih + ' -h1 -a0 ' + buildFolder + '/oric/' + fb + '.mus ' + buildFolder + '/oric/' + fb + '.mus ' + addr + '$8000\n')
                
            for item in shared:
                fb = FileBase(item, '')
                fp.write(orih + ' -a0 ' + item + ' ' + buildFolder + '/oric/' + fb + ' ' + addr + '$A000\n')
                
            fp.write('echo --------------- BUULD DISK --------------- \n\n')
            
            # Disk builder
            cmd = orit + ' -iLAUNCH.COM ' + buildFolder + '/oric/launch.com'
            if len(sprites) > 0:
                cmd += ' ' + buildFolder + '/oric/sprites.dat'
            for item in bitmaps:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '.png') + '.img'
            for item in charset:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '.png') + '.chr'
            for item in charmaps:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '')
            for item in chunks:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '.txt') + '.chk'
            for item in music:
                fb = FileBase(item, '.ym')
                cmd += ' ' + buildFolder + '/oric/' + fb + '.mus'
            for item in shared:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '')
            fp.write(cmd + ' ' + buildFolder + '/' + diskname + '-oric48k.dsk\n')
            fp.write(orio + ' ' + buildFolder + '/' + diskname + '-oric48k.dsk\n')

            fp.write('echo --------------- ORIC DISK READY --------------- \n\n')
            
            # Start emulator?
            if callEmu:
                fp.write(Emulate('utils/emulators/Oricutron-1.2-Hub', 'oricutron.exe -d', diskname + '-oric48k.dsk')     )  
   
        # Done!
        if useGUI:
            messagebox.showinfo('Completed', 'Scripts succesfully written to the build folder!')
        
if __name__ == '__main__':
    if useGUI:
        root = Tk()
        app = Application(root)
        root.iconbitmap(bitmap = icon)
        root.resizable(False, False)
        root.mainloop()
    else:
        root = Tk()
        root.withdraw()
        app = Application(root)
        app.FileLoad('../../'+projectFile)
        app.GenerateScripts()
