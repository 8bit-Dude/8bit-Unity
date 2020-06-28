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
from tkFileDialog import askopenfilename, asksaveasfilename
from PIL import Image, ImageTk
import os, pickle, pygubu

def FileBase(filepath, suffix):
    # Return asset file base
    return os.path.basename(filepath).lower().replace(suffix, '')

class Application:

    version = 1
    
    listbox_AppleBitmap = None
    listbox_AppleSprites = None
    listbox_AppleChunks = None
    listbox_AppleMusic = None
    
    listbox_AtariBitmap = None
    listbox_AtariSprites = None
    listbox_AtariChunks = None
    listbox_AtariMusic = None
    
    listbox_C64Bitmap = None
    listbox_C64Sprites = None
    listbox_C64Chunks = None
    listbox_C64Music = None
    
    listbox_LynxBitmap = None
    listbox_LynxSprites = None
    listbox_LynxChunks = None
    listbox_LynxMusic = None    
    
    listbox_OricBitmap = None
    listbox_OricSprites = None
    listbox_OricChunks = None
    listbox_OricMusic = None
    
    listbox_Shared = None
    listbox_Code = None
    
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

        # Load an ui file
        self.builder.add_from_file('builder.ui')

        # Show tite
        master.title("8bit-Unity Disk Builder") 
        
        # Add file menu
        menubar = Menu(master)
        fileMenu = Menu(menubar)
        fileMenu.add_command(label="New", command=self.FileNew)
        fileMenu.add_command(label="Load", command=self.FileLoad)
        fileMenu.add_command(label="Save", command=self.FileSave)
        fileMenu.add_separator()
        fileMenu.add_command(label="Exit", command=self.FileExit)        
        menubar.add_cascade(label="File", menu=fileMenu)
        master.config(menu=menubar)
        
        # Create widgets using a master as parent
        self.mainwindow = self.builder.get_object('mainwindow', master)        
                
        # Connect callbacks
        self.builder.connect_callbacks(self)
        
        # Get list boxes and fields
        self.listbox_AppleBitmap = self.builder.get_object('Listbox_AppleBitmap')        
        self.listbox_AppleSprites = self.builder.get_object('Listbox_AppleSprites')        
        self.listbox_AppleChunks = self.builder.get_object('Listbox_AppleChunks')        
        self.listbox_AppleMusic = self.builder.get_object('Listbox_AppleMusic')        
        self.entry_AppleSpriteFrames = self.builder.get_object('Entry_AppleSpriteFrames')
        self.entry_AppleSpriteWidth = self.builder.get_object('Entry_AppleSpriteWidth')
        self.entry_AppleSpriteHeight = self.builder.get_object('Entry_AppleSpriteHeight')
        
        self.listbox_AtariBitmap = self.builder.get_object('Listbox_AtariBitmap')        
        self.listbox_AtariSprites = self.builder.get_object('Listbox_AtariSprites')        
        self.listbox_AtariChunks = self.builder.get_object('Listbox_AtariChunks')        
        self.listbox_AtariMusic = self.builder.get_object('Listbox_AtariMusic')     
        self.entry_AtariSpriteFrames = self.builder.get_object('Entry_AtariSpriteFrames')
        self.entry_AtariSpriteWidth = self.builder.get_object('Entry_AtariSpriteWidth')
        self.entry_AtariSpriteHeight = self.builder.get_object('Entry_AtariSpriteHeight')
        self.Checkbutton_AtariNoText = self.builder.get_object('Checkbutton_AtariNoText');
        self.Combobox_AtariDiskSize = self.builder.get_object('Combobox_AtariDiskSize');
        
        self.listbox_C64Bitmap = self.builder.get_object('Listbox_C64Bitmap')        
        self.listbox_C64Sprites = self.builder.get_object('Listbox_C64Sprites')        
        self.listbox_C64Chunks = self.builder.get_object('Listbox_C64Chunks')        
        self.listbox_C64Music = self.builder.get_object('Listbox_C64Music')     
        self.entry_C64SpriteFrames = self.builder.get_object('Entry_C64SpriteFrames')
        self.entry_C64SpriteWidth = self.builder.get_object('Entry_C64SpriteWidth')
        self.entry_C64SpriteHeight = self.builder.get_object('Entry_C64SpriteHeight')
        
        self.listbox_LynxBitmap = self.builder.get_object('Listbox_LynxBitmap')        
        self.listbox_LynxSprites = self.builder.get_object('Listbox_LynxSprites')        
        self.listbox_LynxChunks = self.builder.get_object('Listbox_LynxChunks')        
        self.listbox_LynxMusic = self.builder.get_object('Listbox_LynxMusic')     
        self.entry_LynxSpriteFrames = self.builder.get_object('Entry_LynxSpriteFrames')
        self.entry_LynxSpriteWidth = self.builder.get_object('Entry_LynxSpriteWidth')
        self.entry_LynxSpriteHeight = self.builder.get_object('Entry_LynxSpriteHeight')
        
        self.listbox_OricBitmap = self.builder.get_object('Listbox_OricBitmap')        
        self.listbox_OricChunks = self.builder.get_object('Listbox_OricChunks')        
        self.listbox_OricSprites = self.builder.get_object('Listbox_OricSprites')        
        self.listbox_OricMusic = self.builder.get_object('Listbox_OricMusic')     
        self.entry_OricSpriteFrames = self.builder.get_object('Entry_OricSpriteFrames')
        self.entry_OricSpriteWidth = self.builder.get_object('Entry_OricSpriteWidth')
        self.entry_OricSpriteHeight = self.builder.get_object('Entry_OricSpriteHeight')
        self.entry_OricDithering = self.builder.get_object('Entry_OricDithering')
        self.combobox_OricImageQuality = self.builder.get_object('Combobox_OricImageQuality')
        
        self.listbox_Shared = self.builder.get_object('Listbox_Shared')
        self.listbox_Code = self.builder.get_object('Listbox_Code')
        self.entry_Disk = self.builder.get_object('Entry_Disk')
        
        # Set some defaults
        self.Checkbutton_AtariNoText.state(['!selected'])
        self.Combobox_AtariDiskSize.current(0)
        self.combobox_OricImageQuality.current(1)

        # Make lists of various GUI inputs (adding new inputs to the end of each list will guarantee backward compatibility)
        self.entries = [ self.entry_Disk, 
                         self.entry_AppleSpriteFrames, self.entry_AppleSpriteWidth, self.entry_AppleSpriteHeight, 
                         self.entry_AtariSpriteFrames, self.entry_AtariSpriteWidth, self.entry_AtariSpriteHeight, 
                         self.entry_C64SpriteFrames,   self.entry_C64SpriteWidth,   self.entry_C64SpriteHeight, 
                         self.entry_LynxSpriteFrames,  self.entry_LynxSpriteWidth,  self.entry_LynxSpriteHeight, 
                         self.entry_OricSpriteFrames,  self.entry_OricSpriteWidth,  self.entry_OricSpriteHeight,
                         self.entry_OricDithering ]
        self.listboxes = [ self.listbox_Code, 
                           self.listbox_AppleBitmap, self.listbox_AppleSprites, self.listbox_AppleMusic,
                           self.listbox_AtariBitmap, self.listbox_AtariSprites, self.listbox_AtariMusic,
                           self.listbox_C64Bitmap,   self.listbox_C64Sprites,   self.listbox_C64Music,
                           self.listbox_OricBitmap,  self.listbox_OricSprites,  self.listbox_OricMusic,
                           self.listbox_Shared,
                           self.listbox_LynxBitmap,  self.listbox_LynxSprites,  self.listbox_LynxMusic,
                           self.listbox_AppleChunks, self.listbox_AtariChunks,  self.listbox_C64Chunks,
                           self.listbox_LynxChunks,  self.listbox_OricChunks ]
        self.checkbuttons = [ self.Checkbutton_AtariNoText ]
        self.comboboxes = [ self.Combobox_AtariDiskSize, self.combobox_OricImageQuality ]
                       
    def FileNew(self):
        # Reset all fields
        for l in self.listboxes:
            l.delete(0, END)
        self.entry_Disk.delete(0, END)
        self.entry_Disk.insert(0, 'diskname')
        
    def FileLoad(self):
        filename = askopenfilename(initialdir = "../../", title = "Load Builder Project", filetypes = (("Project files","*.builder"),)) 
        if filename is not '':
            # Reset UI
            for l in self.listboxes:
                l.delete(0, END)
            self.entry_Disk.delete(0, END)
            self.entry_Disk.insert(0, 'diskname')
        
            # Unpickle data
            with open(filename, "rb") as fp:
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
                    item.state(data)

                # Combo boxes
                while data != 'comboboxes':
                    data = pickle.load(fp)                    
                for item in self.comboboxes:                
                    data = pickle.load(fp)
                    item.set(data)
                    
    def FileSave(self):
        filename = asksaveasfilename(initialdir = "../../", title = "Save Builder Project", filetypes = (("Project files","*.builder"),))
        if filename is not '':
            # Fix extension
            if ".builder" not in filename.lower():
                filename += ".builder"
                
            # Pickle data
            with open(filename, "wb") as fp:
                # Version number
                pickle.dump(self.version, fp)
                
                # Entry boxes
                pickle.dump('entries', fp)
                for item in self.entries:
                    data = item.get()
                    pickle.dump(data, fp)
                    
                # List boxes                     
                pickle.dump('listboxes', fp)
                for item in self.listboxes:
                    data = list(item.get(0, END))
                    pickle.dump(data, fp)

                # Check buttons
                pickle.dump('checkbuttons', fp)
                for item in self.checkbuttons:
                    data = item.state()
                    pickle.dump(data, fp)

                # Check buttons
                pickle.dump('comboboxes', fp)
                for item in self.comboboxes:
                    data = item.get()
                    pickle.dump(data, fp)
                    
    def FileExit(self):
        sys.exit()
        
    def AppleBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleBitmap.insert(END, filename)

    def AppleBitmapRem(self):
        self.listbox_AppleBitmap.delete(0, ACTIVE)

    def AppleSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleSprites.delete(0, END)
            self.listbox_AppleSprites.insert(END, filename)

    def AppleChunksSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleChunks.delete(0, END)
            self.listbox_AppleChunks.insert(END, filename)            

    def AppleMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("DUET M files","*.m"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleMusic.insert(END, filename)

    def AppleMusicRem(self):
        self.listbox_AppleMusic.delete(0, ACTIVE)
            
    def AtariBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariBitmap.insert(END, filename)

    def AtariBitmapRem(self):
        self.listbox_AtariBitmap.delete(0, ACTIVE)
        
    def AtariSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariSprites.delete(0, END)
            self.listbox_AtariSprites.insert(END, filename)

    def AtariChunksSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariChunks.delete(0, END)
            self.listbox_AtariChunks.insert(END, filename)            
        
    def AtariMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("RMT files","*.rmt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariMusic.insert(END, filename)

    def AtariMusicRem(self):
        self.listbox_AtariMusic.delete(0, ACTIVE)
    
    def C64BitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Bitmap.insert(END, filename)

    def C64BitmapRem(self):
        self.listbox_C64Bitmap.delete(0, ACTIVE)
        
    def C64SpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Sprites.delete(0, END)
            self.listbox_C64Sprites.insert(END, filename)

    def C64ChunksSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Chunks.delete(0, END)
            self.listbox_C64Chunks.insert(END, filename)            
        
    def C64MusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("SID files","*.sid"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Music.insert(END, filename)

    def C64MusicRem(self):
        self.listbox_C64Music.delete(0, ACTIVE)        
        
    def LynxBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxBitmap.insert(END, filename)

    def LynxBitmapRem(self):
        self.listbox_LynxBitmap.delete(0, ACTIVE)
        
    def LynxSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxSprites.delete(0, END)
            self.listbox_LynxSprites.insert(END, filename)

    def LynxChunksSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxChunks.delete(0, END)
            self.listbox_LynxChunks.insert(END, filename)            
        
    def LynxMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("Chipper files","*.asm"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_LynxMusic.insert(END, filename)

    def LynxMusicRem(self):
        self.listbox_LynxMusic.delete(0, ACTIVE) 
    
    def OricBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricBitmap.insert(END, filename)

    def OricBitmapRem(self):
        self.listbox_OricBitmap.delete(0, ACTIVE)
        
    def OricSpritesSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Sprite Sheet", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricSprites.delete(0, END)
            self.listbox_OricSprites.insert(END, filename)

    def OricChunksSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Chunks Definition", filetypes = (("Text files","*.txt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricChunks.delete(0, END)
            self.listbox_OricChunks.insert(END, filename)            
        
    def OricMusicAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("YM files","*.ym"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_OricMusic.insert(END, filename)

    def OricMusicRem(self):
        self.listbox_OricMusic.delete(0, ACTIVE) 
        
    def SharedAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Asset File", filetypes = (("All files","*.*"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Shared.insert(END, filename)

    def SharedRem(self):
        self.listbox_Shared.delete(0, ACTIVE)
            
    def CodeAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Code File", filetypes = (("C files","*.c"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Code.insert(END, filename)

    def CodeRem(self):
        self.listbox_Code.delete(0, ACTIVE)
        
    def GenerateBuilder(self):
        diskname = self.entry_Disk.get()
        shared = list(self.listbox_Shared.get(0, END))
        code = list(self.listbox_Code.get(0, END))
        if len(code) < 1:
            messagebox.showinfo('Error', 'Please add C source file(s)!')
            return

        ####################################################
        # Apple script
        bitmaps = list(self.listbox_AppleBitmap.get(0, END))
        sprites = list(self.listbox_AppleSprites.get(0, END))
        chunks = list(self.listbox_AppleChunks.get(0, END))
        music = list(self.listbox_AppleMusic.get(0, END))
        with open("../../build/"+diskname+"-apple.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('mkdir apple\n')            
            fp.write('cd ..\n\n')
            fp.write('del build\\apple\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\scripts\\apple\\AppleBitmap.py ' + item + ' build/apple/' + FileBase(item, '-apple.png') + '.img\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\apple\\AppleSprites.py ' + sprites[0] + ' build/apple/sprites.dat\n')
            if len(chunks) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py apple ' + chunks[0] + ' build/apple/\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'chunks.c', 'geom2d.c', 'network.c', 'print.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Apple\\CLOCK.c', 'Apple\\DHR.c']
            SList = ['chars.s', 'math.s', 'Apple\\blit.s', 'Apple\\DUET.s', 'Apple\\JOY.s', 'Apple\\MOCKING.s', 'Apple\\PADDLE.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t apple2 -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r build/apple/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation
            comp = 'utils\\cc65\\bin\\cl65 -o build/apple/' + diskname.lower() + '.bin -Cl -O -t apple2 -C unity/Apple/apple2e.cfg -I unity '
            for item in code:
                comp += item + ' '
            fp.write(comp + 'build/apple/unity.lib unity/IP65/ip65.lib unity/IP65/ip65_apple2.lib\n\n')
            
            # Compression
            cmd = 'utils\\scripts\\exomizer sfx $0803 -t162 -Di_load_addr=$0803 build/apple/' + diskname.lower() + '.bin@$0803,4'
            if len(sprites) > 0:
                cmd += ' build/apple/sprites.dat'
            cmd += ' -o build/apple/loader\n'
            fp.write(cmd)

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- APPLE DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('copy utils\\scripts\\apple\\AppleProDOS190.dsk build\\' + diskname + '-apple.do\n')
            fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander.jar -cc65 build/' + diskname + '-apple.do LOADER bin 0x0803 < build/apple/loader\n')
            if len(chunks) > 0:
                fp.write('for /f "tokens=*" %%A in (build\\apple\\chunks.lst) do utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander.jar -p build/' + diskname + '-apple.do %%~nxA bin < %%A \n')
            for item in shared:
                fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander.jar -p build/' + diskname + '-apple.do ' + FileBase(item, '').upper() + ' bin < ' + item + '\n')
            for item in music:
                fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander.jar -p build/' + diskname + '-apple.do ' + FileBase(item, '-apple.m').upper() + '.MUS bin < ' +item + '\n')
            for item in bitmaps:
                fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander.jar -p build/' + diskname + '-apple.do ' + FileBase(item, '-apple.png').upper() + '.IMG bin < build/apple/' + FileBase(item, '-apple.png') + '.img\n')
            
            # Info
            fp.write('\necho DONE\n')
            fp.write('pause\n\n')
            
            # Start emulator?
            fp.write('cd "utils\emulators\AppleWin-1.26.3.1"\n')
            fp.write('Applewin.exe -d1 "..\\..\\..\\build\\' + diskname + '-apple.do"\n')
            
        ####################################################
        # Atari script
        bitmaps = list(self.listbox_AtariBitmap.get(0, END))
        sprites = list(self.listbox_AtariSprites.get(0, END))
        chunks = list(self.listbox_AtariChunks.get(0, END))
        music = list(self.listbox_AtariMusic.get(0, END))
        with open("../../build/"+diskname+"-atari.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('mkdir atari\n')            
            fp.write('cd ..\n\n')
            fp.write('del build\\atari\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\scripts\\atari\\AtariBitmap.py ' + item + ' build/atari/' + FileBase(item, '-atari.png') + '.img\n')
            if len(sprites) > 0:
                spriteHeight = int(self.entry_AtariSpriteHeight.get())
                fp.write('utils\\py27\\python utils\\scripts\\atari\\AtariSprites.py ' + sprites[0] + ' build/atari/sprites.dat ' + str(spriteHeight) + '\n')
            if len(chunks) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py atari ' + chunks[0] + ' build/atari/\n')

            # Shared Data
            for item in shared:
                fp.write('copy ' + item.replace('/','\\') + ' build\\atari\n')

            # Music Data
            for item in music:
                fp.write('copy ' + item.replace('/','\\') + ' build\\atari\\' + FileBase(item, '-atari.rmt') + '.mus\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'chunks.c', 'geom2d.c', 'network.c', 'print.c', 'sfx.c', 'sprites.c', 'widgets.c']
            SList = ['chars.s', 'math.s', 'Atari/blit.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t atarixl -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r build/atari/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation
            if len(self.Checkbutton_AtariNoText.state()):
                configFile = 'atarixl-notext.cfg'
            else:
                configFile = 'atarixl.cfg'
            comp = 'utils\\cc65\\bin\\cl65 -o build/atari/' + diskname.lower() + '.bin -Cl -O -t atarixl -C unity/Atari/' + configFile + ' -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'unity/Atari/POKEY.s build/atari/unity.lib unity/IP65/ip65.lib unity/IP65/ip65_atarixl.lib\n\n')
            
            fp.write('utils\\cc65\\bin\\cl65 -t atarixl -C atari-asm.cfg -o build/atari/basicoff.bin unity/Atari/BASICOFF.s\n')
            fp.write('utils\\scripts\\atari\\mads.exe -o:build/atari/dli.bin unity/Atari/DLI.a65\n')
            fp.write('utils\\scripts\\atari\\mads.exe -o:build/atari/rmt.bin unity/Atari/RMT.a65\n\n')

            # Merging
            cmd = 'utils\\py27\\python utils\\scripts\\atari\\AtariMerge.py build/atari/autorun build/atari/basicoff.bin build/atari/' + diskname.lower() + '.bin build/atari/dli.bin build/atari/rmt.bin'
            if len(sprites) > 0:
                cmd += ' build/atari/sprites.dat'
            cmd += '\n'                
            fp.write(cmd)

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- ATARI DISK BUILDER --------------- \n\n')

            # Clean-up build folder
            fp.write('del build\\atari\\*.bin\n')
            fp.write('del build\\atari\\*.lib\n')
            fp.write('del build\\atari\\*.lst\n')
            fp.write('del build\\atari\\sprites.dat\n')

            # Disk builder
            if self.Combobox_AtariDiskSize.get() == '180KB':                
                diskSize = '720'
            else:
                diskSize = '1440'
            fp.write('utils\\scripts\\atari\\dir2atr.exe -dm -B utils/scripts/atari/AtariXboot.obx ' + diskSize + ' build/' + diskname + '-atari.atr build\\atari\n')

            # Info
            fp.write('\necho DONE\n')
            fp.write('pause\n\n')
            
            # Start emulator?
            fp.write('cd "utils\emulators\Altirra-3.20"\n')
            fp.write('Altirra.exe "..\\..\\..\\build\\' + diskname + '-atari.atr"\n')             

        ####################################################
        # C64 script
        bitmaps = list(self.listbox_C64Bitmap.get(0, END))
        sprites = list(self.listbox_C64Sprites.get(0, END))
        chunks = list(self.listbox_C64Chunks.get(0, END))
        music = list(self.listbox_C64Music.get(0, END))
        with open("../../build/"+diskname+"-c64.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir c64\n')            
            fp.write('cd ..\n\n')            
            fp.write('del build\\c64\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\scripts\\c64\\C64Bitmap.py ' + item + ' build/c64/' + FileBase(item, '-c64.png') + '.img\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\c64\\C64Sprites.py ' + sprites[0] + ' build/c64/sprites.dat\n')
            if len(chunks) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py c64 ' + chunks[0] + ' build/c64/\n\n')
                
            # Music
            for item in music:
                fb = FileBase(item, '-c64.sid')
                fp.write('utils\\scripts\\c64\\sidreloc.exe -v -z 30-ff -p 08 ' + item + ' build/c64/' + fb + '.sid\n')
                fp.write('if exist build/c64/' + fb + '.sid (\n')
                fp.write('    utils\\scripts\\c64\\psid64.exe -n build/c64/' + fb + '.sid\n')
                fp.write(') else (\n')
                fp.write('    echo Relocation impossible, using the original file instead...\n')
                fp.write('    copy ' + item.replace('/', '\\') + ' build\\c64\\' + fb + '.prg\n')
                fp.write(')\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'chunks.c', 'geom2d.c', 'network.c', 'print.c', 'sfx.c', 'sprites.c', 'widgets.c']
            SList = ['chars.s', 'math.s', 'C64\\JOY.s', 'C64\\ROM.s', 'C64\\SID.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t c64 -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r build/c64/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation                        
            comp = 'utils\\cc65\\bin\\cl65 -o build/c64/' + diskname.lower() + '.bin -Cl -O -t c64 -C unity/C64/c64.cfg -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'build/c64/unity.lib unity/IP65/ip65.lib unity/IP65/ip65_c64.lib\n\n')
            
            # Compression
            cmd = 'utils\\scripts\\exomizer.exe sfx $180d build/c64/' + diskname.lower() + '.bin'
            if len(sprites) > 0:
                cmd += ' build/c64/sprites.dat'
            cmd += ' -o build/c64/loader.prg\n'
            fp.write(cmd)

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- C64 DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('set C1541=utils\\scripts\\c64\\c1541 -format loader,666 d64 build/' + diskname + '-c64.d64 -attach build/' + diskname + '-c64.d64 ')
            fp.write('-write build/c64/loader.prg loader.prg ')
            for item in bitmaps:
                fb = FileBase(item, '-c64.png')
                fp.write('-write build/c64/' + fb + '.img ' + fb + '.img ')                
            for item in music:
                fb = FileBase(item, '-c64.sid')
                fp.write('-write build/c64/' + fb + '.prg ' + fb + '.mus ')              
            for item in shared:
                fp.write('-write ' + item + ' ' + FileBase(item, '') + ' ')                
            fp.write('\nfor /f "tokens=*" %%A in (build\c64\chunks.lst) do set C1541=!C1541!-write %%A %%~nxA \n')
            fp.write('%C1541%\n')
               
            # Info
            fp.write('\n\necho DONE\n')
            fp.write('pause\n\n')
            
            # Start emulator?
            fp.write('cd "utils\emulators\WinVICE-2.4"\n')
            fp.write('x64.exe "..\\..\\..\\build\\' + diskname + '-c64.d64"\n')

        ####################################################
        # Lynx script
        bitmaps = list(self.listbox_LynxBitmap.get(0, END))
        sprites = list(self.listbox_LynxSprites.get(0, END))
        chunks = list(self.listbox_LynxChunks.get(0, END))
        music = list(self.listbox_LynxMusic.get(0, END))
        with open("../../build/"+diskname+"-lynx.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir lynx\n')           
            fp.write('cd lynx\n')
            fp.write('del *.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Convert Keyboard/Bitmap/Sprite files
            for item in ['cursor', 'keyboard']:
                fp.write('copy ..\\..\\utils\\scripts\\lynx\\' + item + '.png ' + item + '.png\n')
                fp.write('..\\..\\utils\\scripts\\png2bmp ' + item + '.png\n')
                fp.write('..\\..\\utils\\scripts\\lynx\\sprpck -t6 -p2 -u ' + item + '.bmp\n')         
            for item in bitmaps:
                fb = FileBase(item, '-lynx.png')
                fp.write('copy ..\\..\\' + item.replace('/', '\\') + ' ' + fb + '.png\n')
                fp.write('..\\..\\utils\\scripts\\png2bmp ' + fb + '.png\n')
                fp.write('..\\..\\utils\\scripts\\lynx\\sprpck -t6 -p2 -u ' + fb + '.bmp\n')
            if len(sprites) > 0:
                spriteFrames = int(self.entry_LynxSpriteFrames.get())
                spriteWidth  = int(self.entry_LynxSpriteWidth.get())
                spriteHeight = int(self.entry_LynxSpriteHeight.get())
                fp.write('copy ..\\..\\' + sprites[0].replace('/', '\\') + ' sprites.png\n')
                fp.write('..\\..\\utils\\scripts\\png2bmp sprites.png\n')
                fp.write('..\\..\\utils\\scripts\\lynx\\sprpck -t6 -p2 -u -r001' + str(spriteFrames).zfill(3) + \
                                            ' -S' + str(spriteWidth).zfill(3) + str(spriteHeight).zfill(3) + \
                                            ' -a' + str(spriteWidth/2).zfill(3) + str(spriteHeight/2).zfill(3) + ' sprites.bmp\n')
                if spriteFrames == 1: 
                    fp.write('ren sprites.spr sprites000000.spr\n')
                    
            # Convert Chunk Data
            fp.write('set /a FILENUM=' + str(len(bitmaps)+len(music)+len(shared)) + '\n')
            fp.write('set CHUNKNAMES=\n')
            fp.write('set CHUNKDATAS=\n')
            if len(chunks) > 0:
                fp.write('..\\..\\utils\\py27\\python ..\\..\\utils\\scripts\\ProcessChunks.py lynx ../../' + chunks[0] + ' ../../build/lynx/\n')
                fp.write('for /f "tokens=*" %%A in (chunks.lst) do set /a FILENUM+=1\n')
                fp.write('for /f "tokens=*" %%A in (chunks.lst) do set CHUNKNAMES=!CHUNKNAMES!_%%~nAName,\n')
                fp.write('for /f "tokens=*" %%A in (chunks.lst) do set CHUNKDATAS=!CHUNKDATAS!_%%~nAData,\n') 

            # Copy Chipper sfx and music data
            fp.write('copy ..\\..\\unity\\Lynx\\chipper.s soundbs.mac\n')    
            for i in range(len(music)):
                fp.write('..\\..\\utils\\py27\\python ../../utils/scripts/lynx/LynxChipper.py ../../' + music[i] + ' music' + str(i).zfill(2) + '.asm _musData' + str(i).zfill(2) + ' MUS' + str(i) + 'DATA"\n')
                
            # Clean-up
            fp.write('del *.png /F /Q\n')
            fp.write('del *.bmp /F /Q\n')
            fp.write('del *.pal /F /Q\n')
            fp.write('\n')
            
            # Copy Shared files
            if len(shared) > 0:             
                for item in shared:
                    fb = FileBase(item, '')
                    fp.write('copy ..\\..\\' + item.replace('/', '\\') + ' ' + fb + '\n')
                fp.write('\n')
                
            # Get Size of various files
            filelist = ''
            for i in range(len(bitmaps)):
                fb = FileBase(bitmaps[i], '-lynx.png')
                filelist += fb + '.spr,'
            for i in range(len(music)):
                filelist += 'music' + str(i).zfill(2) + '.asm,'
            for item in shared:
                fb = FileBase(item, '')
                filelist += fb + ','
            fp.write('set FILESIZES=\n')
            fp.write('for %%I in (' + filelist[0:-1] + ') do set FILESIZES=!FILESIZES!%%~zI,\n\n')
            fp.write('for /f "tokens=*" %%A in (chunks.lst) do set FILESIZES=!FILESIZES!%%~zA,\n') 

            # Generate declare file for read-only data
            fp.write('@echo .global _fileNum  >> data.asm\n')
            fp.write('@echo .global _fileSizes >> data.asm\n')
            fp.write('@echo .global _fileNames >> data.asm\n')
            fp.write('@echo .global _fileDatas >> data.asm\n')
            fp.write('@echo .global _spriteNum  >> data.asm\n')
            fp.write('@echo .global _spriteData >> data.asm\n')
            fp.write('@echo .global _cursorData >> data.asm\n')
            fp.write('@echo .global _keybrdData >> data.asm\n')
            fp.write('@echo ; >> data.asm\n')
            
            # Num and sizes of files
            fp.write('@echo .segment "RODATA" >> data.asm\n')
            fp.write('@echo _fileNum: .byte %FILENUM% >> data.asm\n')  
            fp.write('@echo _fileSizes: .word %FILESIZES:~0,-1% >> data.asm\n')

            # List of file names and data
            if len(bitmaps) > 0 or len(music) > 0 or len(shared) > 0 or len(chunks) > 0:
                # Declare all Bitmap, Shared and Chunk files
                fp.write('@echo _fileNames: .addr ')
                for i in range(len(bitmaps)):
                    if i > 0:
                        fp.write(', ')
                    fp.write('_bmpName' + str(i).zfill(2))
                for i in range(len(music)):
                    fp.write(', '); fp.write('_musName' + str(i).zfill(2))
                for i in range(len(shared)):
                    fp.write(', '); fp.write('_shrName' + str(i).zfill(2))
                if len(chunks) > 0:
                    fp.write(', '); fp.write('%CHUNKNAMES:~0,-1%')
                fp.write(' >> data.asm\n')

                # Write list of Bitmaps
                for i in range(len(bitmaps)):
                    fb = FileBase(bitmaps[i], '-lynx.png')
                    fp.write('@echo _bmpName' + str(i).zfill(2) + ': .byte "' + fb + '.img",0 >> data.asm\n')

                # Write list of Musics
                for i in range(len(music)):
                    fb = FileBase(music[i], '-lynx.asm')
                    fp.write('@echo _musName' + str(i).zfill(2) + ': .byte "' + fb + '.mus",0 >> data.asm\n')
                    
                # Write list of Shared
                for i in range(len(shared)):
                    fb = FileBase(shared[i], '')
                    fp.write('@echo _shrName' + str(i).zfill(2) + ': .byte "' + fb + '",0 >> data.asm\n')
                    
                # Write list of Chunks
                if len(chunks) > 0:
                    fp.write('for /f "tokens=*" %%A in (chunks.lst) do @echo _%%~nAName: .byte "%%~nxA",0 >> data.asm\n')
            
                # Declare all Bitmap, Shared, and Chunk datas
                fp.write('@echo _fileDatas: .addr ')
                for i in range(len(bitmaps)):
                    if i > 0:
                        fp.write(', '); 
                    fp.write('_bmpData' + str(i).zfill(2))
                for i in range(len(music)):
                    fp.write(', '); fp.write('_musData' + str(i).zfill(2))
                for i in range(len(shared)):
                    fp.write(', '); fp.write('_shrData' + str(i).zfill(2))
                if len(chunks) > 0:
                    fp.write(', '); fp.write('%CHUNKDATAS:~0,-1%')
                fp.write(' >> data.asm\n')

                # Link list of bitmaps
                for i in range(len(bitmaps)):
                    fb = FileBase(bitmaps[i], '-lynx.png')
                    fp.write('@echo .segment "BMP' + str(i) + 'DATA" >> data.asm\n')
                    fp.write('@echo _bmpData' + str(i).zfill(2) + ': .incbin "' + fb + '.spr" >> data.asm\n')                    

                # Link list of musics
                for i in range(len(music)):
                    fp.write('@echo .import _musData' + str(i).zfill(2) + ' >> data.asm\n')
                    
                # Link list of shared
                fp.write('@echo .segment "RODATA" >> data.asm\n')
                for i in range(len(shared)):
                    fb = FileBase(shared[i], '')
                    fp.write('@echo _shrData' + str(i).zfill(2) + ': .incbin "' + fb + '" >> data.asm\n')
                    
                # Link list of chunks
                if len(chunks) > 0:
                    fp.write('for /f "tokens=*" %%A in (chunks.lst) do @echo _%%~nAData: .incbin "%%~nxA" >> data.asm\n')
                    
            else:
                fp.write('@echo _fileNames: .addr _dummyName >> data.asm\n')
                fp.write('@echo _dummyName: .byte 0 >> data.asm\n')
                fp.write('@echo _fileDatas: .addr _dummyAddr >> data.asm\n')
                fp.write('@echo _dummyAddr: .byte 0 >> data.asm\n')
            fp.write('@echo ; >> data.asm\n')
            
            # List of Sprite Binary Data 
            fp.write('@echo _spriteNum: .byte ' + self.entry_LynxSpriteFrames.get() + ' >> data.asm\n')            
            if len(sprites) > 0:            
                fp.write('@echo _spriteData: .addr ')    
                for i in range(int(self.entry_LynxSpriteFrames.get())):
                    if i > 0:
                        fp.write(', ')
                    fp.write('_spr' + str(i).zfill(3))
                fp.write(' >> data.asm\n')
                for i in range(int(self.entry_LynxSpriteFrames.get())):
                    fp.write('@echo _spr' + str(i).zfill(3) + ': .incbin "sprites' + str(i).zfill(3) + '000.spr" >> data.asm\n')
            else:
                fp.write('@echo _spriteData: .byte 0 >> data.asm\n')
            fp.write('@echo ; >> data.asm\n')

            # Keybard Binary Data
            fp.write('@echo _cursorData: .incbin "cursor.spr" >> data.asm\n')             
            fp.write('@echo _keybrdData: .incbin "keyboard.spr" >> data.asm\n')                                                 
            
            # Done, return to base folder
            fp.write('\n')
            fp.write('cd ..\n')
            fp.write('cd ..\n')
            fp.write('\n')            

            # Generate config and directory Files
            fp.write('utils\\py27\\python utils/scripts/lynx/LynxConfig.py unity/Lynx/lynx.cfg build/lynx/lynx.cfg ' + str(len(bitmaps)) + ' ' + str(len(music)) + '\n')
            fp.write('utils\\py27\\python utils/scripts/lynx/LynxDirectory.py unity/Lynx/directory.s build/lynx/directory.asm ' + str(len(bitmaps)) + ' ' + str(len(music)) + '\n')
                        
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'chunks.c', 'geom2d.c', 'hub.c', 'network.c', 'print.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Lynx\\display.c', 'Lynx\\files.c', 'Lynx\\joysticks.c']
            SList = ['chars.s', 'math.s', 'Lynx\\header.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t lynx -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r build/lynx/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation 
            comp = 'utils\\cc65\\bin\\cl65 -o build/' + diskname.lower() + '-lynx.lnx -Cl -O -t lynx -C build/lynx/lynx.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            for i in range(len(music)):
                comp += 'build/lynx/music' + str(i).zfill(2) + '.asm '
            fp.write(comp + 'unity/Lynx/sfx.s build/lynx/directory.asm build/lynx/data.asm build/lynx/unity.lib\n')
            
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('pause\n\n')
            
            # Start emulator?
            fp.write('cd "utils\emulators\Handy-0.99"\n')
            fp.write('handy.exe "..\\..\\..\\build\\' + diskname + '-lynx.lnx"\n')


        ####################################################
        # Oric script
        bitmaps = list(self.listbox_OricBitmap.get(0, END))
        sprites = list(self.listbox_OricSprites.get(0, END))
        chunks = list(self.listbox_OricChunks.get(0, END))
        music = list(self.listbox_OricMusic.get(0, END))
        with open("../../build/"+diskname+"-oric.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir oric\n')            
            fp.write('cd ..\n\n')            
            fp.write('del build\\oric\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Process Bitmaps / Chunks / Sprites / Shared
            fp.write('cd utils\\scripts\\oric\n')
            for item in bitmaps:
                filebase = FileBase(item, '-oric.png')
                if self.combobox_OricImageQuality.get() == 'Hires(Noisy)':
                    fp.write('luajit PictOric.lua ' + self.entry_OricDithering.get() + ' ../../../' + item + ' ../../../build/oric/' + filebase + '.dat\n')
                else:
                    fp.write('..\\..\\py27\\python OricBitmap.py ../../../' + item + ' ../../../build/oric/' + filebase + '.dat\n')
                fp.write('header -a0 ../../../build/oric/' + filebase + '.dat ../../../build/oric/' + filebase + '.img $A000\n')
            if len(chunks) > 0:
                fp.write('..\\..\\py27\\python ProcessChunks.py ' + self.combobox_OricImageQuality.get() + ' ' + self.entry_OricDithering.get() + ' ../../../' + chunks[0] + ' ../../../build/oric/\n')
                fp.write('for /f "tokens=*" %%A in (..\\..\\..\\build\\oric\\chunks.lst) do header -a0 ../../../%%A ../../../%%A $8000\n')
            fp.write('cd ..\\..\\..\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\oric\\OricSprites.py ' + sprites[0] + ' build/oric/sprites.dat\n')
                fp.write('utils\\scripts\\oric\\header -a0 build/oric/sprites.dat build/oric/sprites.dat $7800\n')
            for item in music:
                filebase = FileBase(item, '-oric.ym')
                fp.write('utils\\scripts\\oric\\ym2mym ' + item + ' build/oric/' + filebase + '.mus\n')
                fp.write('utils\\scripts\\oric\\header -h1 -a0 build/oric/' + filebase + '.mus build/oric/' + filebase + '.mus $8000\n')
            for item in shared:
                filebase = FileBase(item, '')
                fp.write('utils\\scripts\\oric\\header -a0 ' + item + ' build/oric/' + filebase + ' $A000\n')
                
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'chunks.c', 'geom2d.c', 'hub.c', 'network.c', 'print.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Oric\\files.c', 'Oric\\joysticks.c']
            SList = ['chars.s', 'math.s', 'Oric\\blit.s', 'Oric\\JOY.s', 'Oric\\keyboard.s', 'Oric\\libsedoric.s', 'Oric\\MYM.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t atmos -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r build/oric/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation 
            comp = 'utils\\cc65\\bin\\cl65 -o build/oric/' + diskname.lower() + '.bin -Cl -O -t atmos -C unity/Oric/oric.cfg -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'build/oric/unity.lib\n\n')

            # Fix header
            fp.write('utils\\scripts\\oric\\header.exe build/oric/' + diskname.lower() + '.bin build/oric/launch.com $0501\n\n')
            
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- ORIC DISK BUILDER --------------- \n\n')
            
            # Disk builder
            cmd = 'set TAP2DSK=utils\\scripts\\oric\\tap2dsk.exe -iLAUNCH.COM build/oric/launch.com'
            if len(sprites) > 0:
                cmd += ' build/oric/sprites.dat'
            for item in shared:
                cmd += ' build/oric/' + FileBase(item, '')
            for item in bitmaps:
                cmd += ' build/oric/' + FileBase(item, '-oric.png') + '.img'
            for item in music:
                filebase = FileBase(item, '-oric.ym')
                cmd += ' build/oric/' + filebase + '.mus'
            fp.write(cmd + '\n')
            if len(chunks) > 0:
                fp.write('for /f "tokens=*" %%A in (build\oric\chunks.lst) do set TAP2DSK=!TAP2DSK! %%A\n')
            fp.write('set TAP2DSK=%TAP2DSK% build/' + diskname + '-oric.dsk\n')
            fp.write('%TAP2DSK%\n')
            fp.write('utils\\scripts\\oric\\old2mfm.exe build/' + diskname + '-oric.dsk\n')
            
            # Info
            fp.write('\necho DONE\n')
            fp.write('pause\n\n')
            
            # Start emulator?
            fp.write('cd "utils\emulators\Oricutron-1.3"\n')
            fp.write('oricutron.exe -d "..\\..\\..\\build\\' + diskname + '-oric.dsk"\n')            
   
        # Done!
        messagebox.showinfo('Completed', 'Scripts succesfully written to the build folder!')
        
        
if __name__ == '__main__':
    root = Tk()
    app = Application(root)
    root.iconbitmap('builder.ico')
    root.resizable(False, False)
    root.mainloop()
