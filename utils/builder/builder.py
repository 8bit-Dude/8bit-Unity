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
    listbox_AppleMusic = None
    listbox_AtariBitmap = None
    listbox_AtariSprites = None
    listbox_AtariMusic = None
    listbox_C64Bitmap = None
    listbox_C64Sprites = None
    listbox_C64Music = None
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
        self.entry_Disk = self.builder.get_object('Entry_Disk');
        self.listbox_Code = self.builder.get_object('Listbox_Code')
        self.listbox_AppleBitmap = self.builder.get_object('Listbox_AppleBitmap')        
        self.listbox_AppleSprites = self.builder.get_object('Listbox_AppleSprites')        
        self.listbox_AppleMusic = self.builder.get_object('Listbox_AppleMusic')        
        self.listbox_AtariBitmap = self.builder.get_object('Listbox_AtariBitmap')        
        self.listbox_AtariSprites = self.builder.get_object('Listbox_AtariSprites')        
        self.listbox_AtariMusic = self.builder.get_object('Listbox_AtariMusic')     
        self.listbox_C64Bitmap = self.builder.get_object('Listbox_C64Bitmap')        
        self.listbox_C64Sprites = self.builder.get_object('Listbox_C64Sprites')        
        self.listbox_C64Music = self.builder.get_object('Listbox_C64Music')     

        # Make list of boxes
        self.entries = [ self.entry_Disk ]
        self.lists = [ self.listbox_Code, 
                       self.listbox_AppleBitmap, self.listbox_AppleSprites, self.listbox_AppleMusic,
                       self.listbox_AtariBitmap, self.listbox_AtariSprites, self.listbox_AtariMusic,
                       self.listbox_C64Bitmap,   self.listbox_C64Sprites,   self.listbox_C64Music ]

    def FileNew(self):
        # Reset all fields
        for l in self.lists:
            l.delete(0, END)
        self.entry_Disk.delete(0, END)
        self.entry_Disk.insert(0, 'diskname')
        
    def FileLoad(self):
        filename = askopenfilename(initialdir = "../../", title = "Load Builder Project", filetypes = (("Project files","*.8bu"),)) 
        if filename is not '':
            # Unpickle data
            with open(filename, "rb") as fp:
                # Version number
                print pickle.load(fp)
                
                # Entry boxes
                print pickle.load(fp)
                for item in self.entries:
                    data = pickle.load(fp)
                    item.delete(0, END)
                    item.insert(0, data)
                    
                # List boxes                     
                print pickle.load(fp)
                for item in self.lists:
                    data = pickle.load(fp)
                    item.delete(0, END)
                    for d in data:
                        item.insert(END, d)
            
    def FileSave(self):
        filename = asksaveasfilename(initialdir = "../../", title = "Save Builder Project", filetypes = (("Project files","*.8bu"),))
        if filename is not '':
            # Fix extension
            if ".8bu" not in filename.lower():
                filename += ".8bu"
                
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
                pickle.dump('lists', fp)
                for item in self.lists:
                    data = list(item.get(0, END))
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

    def AppleMusicSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("DUET files","*.duet"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleMusic.delete(0, END)
            self.listbox_AppleMusic.insert(END, filename)
            
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

    def AtariMusicSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("RMT files","*.rmt"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AtariMusic.delete(0, END)
            self.listbox_AtariMusic.insert(END, filename)

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

    def C64MusicSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Music Track", filetypes = (("SID files","*.sid"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_C64Music.delete(0, END)
            self.listbox_C64Music.insert(END, filename)              
        
    def CodeAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Code File", filetypes = (("C files","*.c"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Code.insert(END, filename)

    def CodeRem(self):
        self.listbox_Code.delete(0, ACTIVE)
        
    def GenerateBuilder(self):
        diskname = self.entry_Disk.get()
        code = list(self.listbox_Code.get(0, END))
        if len(code) < 1:
            messagebox.showinfo('Error', 'Please add C source file(s)!')
            return

        ####################################################
        # Apple script
        bitmaps = list(self.listbox_AppleBitmap.get(0, END))
        sprites = list(self.listbox_AppleSprites.get(0, END))
        music = list(self.listbox_AppleMusic.get(0, END))
        with open("../../build-"+diskname+"-apple.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('rm [build]\\apple\\*.* \n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\apple\\AppleBitmap.py ' + item + ' [build]/apple/' + FileBase(item, '-apple.png') + '.map\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\apple\\AppleSprites.py ' + sprites[0] + ' [build]/apple/sprites.dat\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Compilation
            comp = 'utils\\cc65\\bin\\cl65 -o [build]/apple/' + diskname.lower() + '.bin -Cl -O -t apple2 -C config/apple2-hgr.cfg -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'unity/bitmap.c unity/chars.s unity/sfx.c unity/sprites.c unity/Apple/CLOCK.c unity/Apple/DHR.c unity/Apple/DUET.s unity/Apple/JOY.s unity/Apple/MOCKING.s unity/IP65/ip65.lib unity/IP65/ip65_apple2.lib\n')
            
            # Compression
            cmd = 'utils\\exomizer sfx $0803 -t162 -Di_load_addr=$0803 [build]/apple/' + diskname.lower() + '.bin@$0803,4'
            if len(sprites) > 0:
                cmd += ' [build]/apple/sprites.dat'
            if len(music) > 0:    
                cmd += ' ' + music[0]
            cmd += ' -o [build]/apple/loader\n'
            fp.write(cmd)

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- APPLE DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('copy utils\\apple\\AppleProDOS190.dsk [build]\\' + diskname + '.dsk\n')
            fp.write('java -jar utils\\apple\\AppleCommander.jar -cc65 [build]/' + diskname + '.dsk LOADER bin 0x0803 < [build]/apple/loader\n')
            for item in bitmaps:                
                fp.write('java -jar utils\\apple\\AppleCommander.jar -p [build]/' + diskname + '.dsk ' + FileBase(item, '-apple.png').upper() + '.MAP bin < [build]/apple/' + FileBase(item, '-apple.png') + '.map\n')

            # Info
            fp.write('\necho DONE\n')
            fp.write('pause')
            
        ####################################################
        # Atari script
        bitmaps = list(self.listbox_AtariBitmap.get(0, END))
        sprites = list(self.listbox_AtariSprites.get(0, END))
        music = list(self.listbox_AtariMusic.get(0, END))
        with open("../../build-"+diskname+"-atari.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('rm [build]\\atari\\*.* \n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\atari\\AtariBitmap.py ' + item + ' [build]/atari/' + FileBase(item, '-atari.png') + '.map\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\atari\\AtariSprites.py ' + sprites[0] + ' [build]/atari/sprites.dat\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Compilation
            comp = 'utils\\cc65\\bin\\cl65 -o [build]/atari/program.bin -Cl -O -t atarixl -C config/atarixl.cfg -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'unity/bitmap.c unity/chars.s unity/sfx.c unity/sprites.c unity/Atari/POKEY.s unity/IP65/ip65.lib unity/IP65/ip65_atarixl.lib\n')
            fp.write('utils\\cc65\\bin\\cl65 -t atarixl -C atari-asm.cfg -o [build]/atari/basicoff.bin unity/Atari/BASICOFF.s\n')
            fp.write('utils\\atari\\mads -o:[build]/atari/dli.bin unity/Atari/DLI.a65\n')
            fp.write('utils\\atari\\mads -o:[build]/atari/rmt.bin unity/Atari/RMT.a65\n')

            # Merging
            cmd = 'utils\\py27\\python utils\\atari\\AtariMerge.py [build]/atari/autorun [build]/atari/basicoff.bin [build]/atari/program.bin [build]/atari/dli.bin [build]/atari/rmt.bin'
            if len(music) > 0:    
                cmd += ' ' + music[0]
            if len(sprites) > 0:
                cmd += ' [build]/atari/sprites.dat'
            cmd += '\n'                
            fp.write(cmd)
            
            # Clean-up
            fp.write('rm [build]\\atari\\*.bin\n')
            fp.write('rm [build]\\atari\\*.dat\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- ATARI DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('utils\\atari\\dir2atr -dm -B utils/atari/AtariXboot.obx 720 [build]/' + diskname + '.atr [build]\\atari\n')

            # Info
            fp.write('\necho DONE\n')
            fp.write('pause')            

        ####################################################
        # C64 script
        bitmaps = list(self.listbox_C64Bitmap.get(0, END))
        sprites = list(self.listbox_C64Sprites.get(0, END))
        music = list(self.listbox_C64Music.get(0, END))
        with open("../../build-"+diskname+"-c64.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('rm [build]\\c64\\*.* \n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps / Sprites
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\c64\\C64Bitmap.py ' + item + ' [build]/c64/' + FileBase(item, '-c64.png') + '.map\n')
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\c64\\C64Sprites.py ' + sprites[0] + ' [build]/c64/sprites.dat\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')
            
            # Compilation
            comp = 'utils\\cc65\\bin\\cl65 -o [build]/c64/' + diskname.lower() + '.bin -Cl -O -t c64 -C config/c64.cfg -I unity '
            for item in code:
                comp += item
                comp += ' '
            fp.write(comp + 'unity/bitmap.c unity/chars.s unity/sfx.c unity/sprites.c unity/c64/JOY.s unity/c64/ROM.s unity/C64/SID.s unity/IP65/ip65.lib unity/IP65/ip65_c64.lib\n')
            
            # Compression
            cmd = 'utils\\exomizer.exe sfx 2061 [build]/c64/' + diskname.lower() + '.bin'
            if len(sprites) > 0:
                cmd += ' [build]/c64/sprites.dat'
            if len(music) > 0: 
                cmd += ' ' + music[0]
            cmd += ' -o [build]/c64/loader.prg\n'
            fp.write(cmd)

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- C64 DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('utils\\c64\\c1541 -format loader,666 d64 [build]/' + diskname + '.d64 -attach [build]/' + diskname + '.d64 ')
            fp.write('-write [build]/c64/loader.prg loader.prg ')
            for item in bitmaps:
                fp.write('-write [build]/c64/' + FileBase(item, '-c64.png') + '.map ' + FileBase(item, '-c64.png') + '.map ')                
                
            # Info
            fp.write('\n\necho DONE\n')
            fp.write('pause')
   
        # Done!
        messagebox.showinfo('Completed', 'Scripts generated in 8bit-Unity root folder!')
        
        
if __name__ == '__main__':
    root = Tk()
    app = Application(root)
    root.iconbitmap('builder.ico')
    root.resizable(False, False)
    root.mainloop()
