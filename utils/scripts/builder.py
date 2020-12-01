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
import os, pickle, pygubu, sys, collections, json, codecs

# Useful functions
def Str2Bool(v):
    return v.lower() in ("yes", "true", "1")

def FileBase(filepath, suffix):
    # Return asset file base
    return os.path.basename(filepath).lower().replace(suffix, '')

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
    
    listbox_AppleBitmap = None
    listbox_AppleCharset = None
    listbox_AppleSprites = None
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
        self.listbox_Code = self.builder.get_object('Listbox_Code')
        self.listbox_Charmap = self.builder.get_object('Listbox_Charmap')
        self.listbox_Shared = self.builder.get_object('Listbox_Shared')
        self.entry_Disk = self.builder.get_object('Entry_Disk')
        
        self.listbox_AppleBitmap = self.builder.get_object('Listbox_AppleBitmap')        
        self.listbox_AppleCharset = self.builder.get_object('Listbox_AppleCharset')        
        self.listbox_AppleSprites = self.builder.get_object('Listbox_AppleSprites')        
        self.listbox_AppleChunks = self.builder.get_object('Listbox_AppleChunks')        
        self.listbox_AppleMusic = self.builder.get_object('Listbox_AppleMusic')        
        self.entry_AppleSpriteFrames = self.builder.get_object('Entry_AppleSpriteFrames')
        self.entry_AppleSpriteWidth = self.builder.get_object('Entry_AppleSpriteWidth')
        self.entry_AppleSpriteHeight = self.builder.get_object('Entry_AppleSpriteHeight')
        
        self.listbox_AtariBitmap = self.builder.get_object('Listbox_AtariBitmap')        
        self.listbox_AtariCharset = self.builder.get_object('Listbox_AtariCharset')        
        self.listbox_AtariSprites = self.builder.get_object('Listbox_AtariSprites')        
        self.listbox_AtariChunks = self.builder.get_object('Listbox_AtariChunks')        
        self.listbox_AtariMusic = self.builder.get_object('Listbox_AtariMusic')     
        self.entry_AtariSpriteFrames = self.builder.get_object('Entry_AtariSpriteFrames')
        self.entry_AtariSpriteWidth = self.builder.get_object('Entry_AtariSpriteWidth')
        self.entry_AtariSpriteHeight = self.builder.get_object('Entry_AtariSpriteHeight')
        self.Checkbutton_AtariNoText = self.builder.get_object('Checkbutton_AtariNoText');
        self.Combobox_AtariDiskSize = self.builder.get_object('Combobox_AtariDiskSize');
        
        self.listbox_C64Bitmap = self.builder.get_object('Listbox_C64Bitmap')        
        self.listbox_C64Charset = self.builder.get_object('Listbox_C64Charset')        
        self.listbox_C64Sprites = self.builder.get_object('Listbox_C64Sprites')        
        self.listbox_C64Chunks = self.builder.get_object('Listbox_C64Chunks')        
        self.listbox_C64Music = self.builder.get_object('Listbox_C64Music')     
        self.entry_C64SpriteFrames = self.builder.get_object('Entry_C64SpriteFrames')
        self.entry_C64SpriteWidth = self.builder.get_object('Entry_C64SpriteWidth')
        self.entry_C64SpriteHeight = self.builder.get_object('Entry_C64SpriteHeight')
        
        self.listbox_LynxBitmap = self.builder.get_object('Listbox_LynxBitmap')        
        self.listbox_LynxCharset = self.builder.get_object('Listbox_LynxCharset')        
        self.listbox_LynxSprites = self.builder.get_object('Listbox_LynxSprites')        
        self.listbox_LynxChunks = self.builder.get_object('Listbox_LynxChunks')        
        self.listbox_LynxMusic = self.builder.get_object('Listbox_LynxMusic')     
        self.entry_LynxSpriteFrames = self.builder.get_object('Entry_LynxSpriteFrames')
        self.entry_LynxSpriteWidth = self.builder.get_object('Entry_LynxSpriteWidth')
        self.entry_LynxSpriteHeight = self.builder.get_object('Entry_LynxSpriteHeight')
        self.entry_LynxMusicMemory = self.builder.get_object('Entry_LynxMusicMemory')
        self.entry_LynxSharedMemory = self.builder.get_object('Entry_LynxSharedMemory')
        
        self.listbox_OricBitmap = self.builder.get_object('Listbox_OricBitmap')        
        self.listbox_OricCharset = self.builder.get_object('Listbox_OricCharset')        
        self.listbox_OricChunks = self.builder.get_object('Listbox_OricChunks')        
        self.listbox_OricSprites = self.builder.get_object('Listbox_OricSprites')        
        self.listbox_OricMusic = self.builder.get_object('Listbox_OricMusic')     
        self.entry_OricSpriteFrames = self.builder.get_object('Entry_OricSpriteFrames')
        self.entry_OricSpriteWidth = self.builder.get_object('Entry_OricSpriteWidth')
        self.entry_OricSpriteHeight = self.builder.get_object('Entry_OricSpriteHeight')
        self.entry_OricEnforcedColors = self.builder.get_object('Entry_OricEnforcedColors')
        self.entry_OricDithering = self.builder.get_object('Entry_OricDithering')
                
        # Set some defaults
        self.Checkbutton_AtariNoText.state(['!selected'])
        self.Combobox_AtariDiskSize.current(0)

        # Make lists of various GUI inputs (adding new inputs to the end of each list will guarantee backward compatibility)
        self.entries = [ self.entry_Disk, 
                         self.entry_AppleSpriteFrames, self.entry_AppleSpriteWidth, self.entry_AppleSpriteHeight, 
                         self.entry_AtariSpriteFrames, self.entry_AtariSpriteWidth, self.entry_AtariSpriteHeight, 
                         self.entry_C64SpriteFrames,   self.entry_C64SpriteWidth,   self.entry_C64SpriteHeight, 
                         self.entry_LynxSpriteFrames,  self.entry_LynxSpriteWidth,  self.entry_LynxSpriteHeight, 
                         self.entry_OricSpriteFrames,  self.entry_OricSpriteWidth,  self.entry_OricSpriteHeight,
                         self.entry_OricDithering,     self.entry_LynxMusicMemory,  self.entry_LynxSharedMemory,
                         self.entry_OricEnforcedColors ]
        self.listboxes = [ self.listbox_Code, 
                           self.listbox_AppleBitmap,  self.listbox_AppleSprites, self.listbox_AppleMusic,
                           self.listbox_AtariBitmap,  self.listbox_AtariSprites, self.listbox_AtariMusic,
                           self.listbox_C64Bitmap,    self.listbox_C64Sprites,   self.listbox_C64Music,
                           self.listbox_OricBitmap,   self.listbox_OricSprites,  self.listbox_OricMusic,
                           self.listbox_Shared, 
                           self.listbox_LynxBitmap,   self.listbox_LynxSprites,  self.listbox_LynxMusic,
                           self.listbox_AppleChunks,  self.listbox_AtariChunks,  self.listbox_C64Chunks,
                           self.listbox_LynxChunks,   self.listbox_OricChunks,
                           self.listbox_AppleCharset, self.listbox_AtariCharset, self.listbox_C64Charset,
                           self.listbox_LynxCharset,  self.listbox_OricCharset,
                           self.listbox_Charmap ]
        self.checkbuttons = [ self.Checkbutton_AtariNoText ]
        self.comboboxes = [ self.Combobox_AtariDiskSize ]
                       
    def FileNew(self):
        # Reset all fields
        for l in self.listboxes:
            l.delete(0, END)
        self.entry_Disk.delete(0, END)
        self.entry_Disk.insert(0, 'diskname')
        
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
                        item.state(('selected',))
                    else:
                        item.state(('!selected',))

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
                            component.state(('selected',))
                        else:
                            component.state(('!selected',))
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
                        state = component.state()
                        child = state[0] == 'selected' if state else False
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
            ]),
            ('platform', [
                ('Apple', [
                    ('spriteFrames', ('entry', self.entry_AppleSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_AppleSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_AppleSpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_AppleBitmap)),
                    ('sprites', ('listbox', self.listbox_AppleSprites)),
                    ('music', ('listbox', self.listbox_AppleMusic)),
                    ('chunks', ('listbox', self.listbox_AppleChunks)),
                    ('charset', ('listbox', self.listbox_AppleCharset)),
                ]),
                ('Atari', [
                    ('spriteFrames', ('entry', self.entry_AtariSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_AtariSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_AtariSpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_AtariBitmap)),
                    ('sprites', ('listbox', self.listbox_AtariSprites)),
                    ('music', ('listbox', self.listbox_AtariMusic)),
                    ('chunks', ('listbox', self.listbox_AtariChunks)),
                    ('charset', ('listbox', self.listbox_AtariCharset)),
                    ('noText', ('Checkbutton', self.Checkbutton_AtariNoText)),
                    ('diskSize', ('Combobox', self.Combobox_AtariDiskSize)),
                ]),
                ('C64', [
                    ('spriteFrames', ('entry', self.entry_C64SpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_C64SpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_C64SpriteHeight)),
                    ('bitmap', ('listbox', self.listbox_C64Bitmap)),
                    ('sprites', ('listbox', self.listbox_C64Sprites)),
                    ('music', ('listbox', self.listbox_C64Music)),
                    ('chunks', ('listbox', self.listbox_C64Chunks)),
                    ('charset', ('listbox', self.listbox_C64Charset)),
                ]),
                ('Lynx', [
                    ('spriteFrames', ('entry', self.entry_LynxSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_LynxSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_LynxSpriteHeight)),
                    ('musicMemory', ('entry', self.entry_LynxMusicMemory)),
                    ('sharedMemory', ('entry', self.entry_LynxSharedMemory)),
                    ('bitmap', ('listbox', self.listbox_LynxBitmap)),
                    ('sprites', ('listbox', self.listbox_LynxSprites)),
                    ('music', ('listbox', self.listbox_LynxMusic)),
                    ('chunks', ('listbox', self.listbox_LynxChunks)),
                    ('charset', ('listbox', self.listbox_LynxCharset)),
                ]),
                ('Oric', [
                    ('spriteFrames', ('entry', self.entry_OricSpriteFrames)),
                    ('spriteWidth', ('entry', self.entry_OricSpriteWidth)),
                    ('spriteHeight', ('entry', self.entry_OricSpriteHeight)),
                    ('dithering', ('entry', self.entry_OricDithering)),
                    ('enforcedColors', ('entry', self.entry_OricEnforcedColors)),
                    ('bitmap', ('listbox', self.listbox_OricBitmap)),
                    ('sprites', ('listbox', self.listbox_OricSprites)),
                    ('music', ('listbox', self.listbox_OricMusic)),
                    ('chunks', ('listbox', self.listbox_OricChunks)),
                    ('charset', ('listbox', self.listbox_OricCharset)),
                ]),
            ]),
        ]

    def CodeAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Code File", filetypes = (("C files","*.c"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Code.insert(END, filename)

    def CodeRem(self):
        self.listbox_Code.delete(0, ACTIVE)
                
    def CharmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Map", filetypes = (("Charmap/Tileset files","*.map;*.tls"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Charmap.insert(END, filename)

    def CharmapRem(self):
        self.listbox_Charmap.delete(0, ACTIVE)
            
    def SharedAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Asset File", filetypes = (("All files","*.*"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_Shared.insert(END, filename)

    def SharedRem(self):
        self.listbox_Shared.delete(0, ACTIVE)
        
    def AppleBitmapAdd(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Bitmap", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleBitmap.insert(END, filename)

    def AppleBitmapRem(self):
        self.listbox_AppleBitmap.delete(0, ACTIVE)

    def AppleCharsetSel(self):
        filename = askopenfilename(initialdir = "../../", title = "Select Character Set", filetypes = (("PNG files","*.png"),)) 
        if filename is not '':
            filename = filename.replace(self.cwd, '')
            self.listbox_AppleCharset.delete(0, END)
            self.listbox_AppleCharset.insert(END, filename)
            
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

    def GenerateBuilder(self):
        diskname = self.entry_Disk.get()
        code = list(self.listbox_Code.get(0, END))
        charmaps = list(self.listbox_Charmap.get(0, END))
        shared = list(self.listbox_Shared.get(0, END))
        if len(code) < 1:
            messagebox.showinfo('Error', 'Please add C source file(s)!')
            return

        ####################################################
        # Apple script
        bitmaps = list(self.listbox_AppleBitmap.get(0, END))
        charset = list(self.listbox_AppleCharset.get(0, END))
        sprites = list(self.listbox_AppleSprites.get(0, END))
        chunks = list(self.listbox_AppleChunks.get(0, END))
        music = list(self.listbox_AppleMusic.get(0, END))
        
        # Build Single and Double Hires Scripts
        for target in ['64k', '128k']:
        
            with open('../../' + buildFolder+'/'+diskname+"-apple"+target+".bat", "wb") as fp:
                # Info
                fp.write('echo off\n\n')
                fp.write('mkdir apple\n')            
                fp.write('cd ..\n\n')
                fp.write('del build\\apple\\*.* /F /Q\n\n')
                fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
                
                # Assign GFX mode
                if target == '128k':
                    graphics = 'double'
                else:
                    graphics = 'single'
                
                # Bitmaps
                for item in bitmaps:
                    fp.write('utils\\py27\\python utils\\scripts\\apple\\AppleBitmap.py ' + graphics + ' ' + item + ' ' + buildFolder + '/apple/' + FileBase(item, '-apple.png') + '.img\n')

                # Charset
                if len(charset) > 0:
                    fb = FileBase(charset[0], '-apple.png')
                    fp.write('utils\\py27\python utils\\scripts\\apple\\AppleCharset.py ' + graphics + ' ' + charset[0] + ' ' + buildFolder + '/apple/' + fb + '.chr\n')
                    
                # Sprites
                if len(sprites) > 0:
                    spriteHeight = int(self.entry_AppleSpriteHeight.get())
                    fp.write('utils\\py27\\python utils\\scripts\\apple\\AppleSprites.py ' + graphics + ' ' + sprites[0] + ' ' + buildFolder + '/apple/sprites.dat ' + str(spriteHeight) + '\n')

                # Chunks
                if len(chunks) > 0:
                    fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py apple-' + graphics + ' ' + chunks[0] + ' ' + buildFolder + '/apple/\n')

                # Info
                fp.write('\necho DONE!\n\n')
                fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

                # Build Unity Library
                CList = ['bitmap.c', 'charmap.c', 'chunks.c', 'geom2d.c', 'mouse.c', 'music.c', 'net-base.c', 'net-url.c', 'net-tcp.c', 'net-udp.c', 'net-web.c', 'pixel.c', 'print.c', 'scaling.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Apple\\CLOCK.c', 'Apple\\directory.c', 'Apple\\files.c', 'Apple\\hires.c', 'Apple\\pixelDHR.c', 'Apple\\pixelSHR.c']
                SList = ['atan2.s', 'chars.s', 'tiles.s', 'Apple\\blitDHR.s', 'Apple\\blitSHR.s', 'Apple\\DUET.s', 'Apple\\hiresLines.s', 'Apple\\joystick.s', 'Apple\\MOCKING.s', 'Apple\\PADDLE.s', 'Apple\\prodos.s', 'Apple\\scrollDHR.s', 'Apple\\scrollSHR.s']
                             
                for file in CList:
                    if graphics == 'double':
                        fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t apple2 -D __DHR__ -I unity unity\\' + file + '\n')
                    else:
                        fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t apple2 -I unity unity\\' + file + '\n')
                    fp.write('utils\\cc65\\bin\\ca65 -t apple2 unity\\' + file[0:-2] + '.s\n')
                    fp.write('del unity\\' + file[0:-2] + '.s\n')

                for file in SList:            
                    fp.write('utils\\cc65\\bin\\ca65 -t apple2 unity\\' + file + '\n')
                
                fp.write('utils\\cc65\\bin\\ar65 r ' + buildFolder + '/apple/unity.lib ')
                for file in CList:
                    fp.write('unity\\' + file[0:-2] + '.o ')
                for file in SList:            
                    fp.write('unity\\' + file[0:-2] + '.o ')
                fp.write('\n')
                
                # Compilation
                if graphics == 'double':
                    symbols = '-D __DHR__ -Wl -D,__STACKSIZE__=$0400,-D,__HIMEM__=$BC00,-D,__LCADDR__=$D000,-D,__LCSIZE__=$1000'
                else:
                    symbols = '-Wl -D,__STACKSIZE__=$0400,-D,__HIMEM__=$BC00,-D,__LCADDR__=$D000,-D,__LCSIZE__=$1000'
                comp = 'utils\\cc65\\bin\\cl65 -o ' + buildFolder + '/apple/' + diskname.lower() + '.bin -m ' + buildFolder + '/' + diskname.lower() + '-apple' + target + '.map -Cl -O -t apple2 ' + symbols + ' -C apple2-hgr.cfg -I unity '
                for item in code:
                    comp += item + ' '
                fp.write(comp + buildFolder + '/apple/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_apple2.lib\n\n')
                
                # Compression
                fp.write('utils\\scripts\\exomizer-3.0.2.exe sfx bin ' + buildFolder + '/apple/' + diskname.lower() + '.bin -o ' + buildFolder + '/apple/loader\n\n')

                # Info
                fp.write('echo DONE!\n\n')
                fp.write('echo --------------- APPLE DISK BUILDER --------------- \n\n')

                # Disk builder
                fp.write('copy utils\\scripts\\apple\\ProDOS190.dsk ' + buildFolder + '\\' + diskname + '-apple' + target + '.do\n')
                fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -as ' + buildFolder + '/' + diskname + '-apple' + target + '.do LOADER bin 0x0803 < ' + buildFolder + '/apple/loader\n')
                if len(sprites) > 0:
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do SPRITES.DAT bin < ' + buildFolder + '/apple/sprites.dat\n')
                for item in bitmaps:
                    fb = FileBase(item, '-apple.png')
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do ' + fb.upper() + '.IMG bin < ' + buildFolder + '/apple/' + fb + '.img\n')
                if len(charset) > 0:
                    fb = FileBase(charset[0], '-apple.png')
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do ' + fb.upper() + '.CHR bin < ' + buildFolder + '/apple/' + fb + '.chr\n')
                for item in charmaps:
                    fb = FileBase(item, '')
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do ' + fb.upper() + ' bin < ' + item + '\n')
                for item in music:
                    fb = FileBase(item, '-apple.m')
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do ' + fb.upper() + '.MUS bin < ' +item + '\n')
                for item in shared:
                    fb = FileBase(item, '')
                    fp.write('utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do ' + fb.upper() + ' bin < ' + item + '\n')
                if len(chunks) > 0:
                    fp.write('for /f "tokens=*" %%A in (' + buildFolder + '\\apple\\chunks.lst) do utils\\java\\bin\\java -jar utils\\scripts\\apple\\AppleCommander-1.6.0.jar -p ' + buildFolder + '/' + diskname + '-apple' + target + '.do %%~nxA bin < %%A \n')
                
                # Info
                fp.write('\necho DONE\n')
                
                # Start emulator?
                if callEmu:
                    fp.write('pause\n\n')
                    fp.write('cd "utils\emulators\AppleWin-1.26.3.1"\n')
                    fp.write('Applewin.exe -d1 "..\\..\\..\\' + buildFolder + '\\' + diskname + '-apple' + target + '.do"\n')
            
        ####################################################
        # Atari script
        bitmaps = list(self.listbox_AtariBitmap.get(0, END))
        charset = list(self.listbox_AtariCharset.get(0, END))
        sprites = list(self.listbox_AtariSprites.get(0, END))
        chunks = list(self.listbox_AtariChunks.get(0, END))
        music = list(self.listbox_AtariMusic.get(0, END))
        with open('../../' + buildFolder+'/'+diskname+"-atari.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('mkdir atari\n')            
            fp.write('cd ..\n\n')
            fp.write('del ' + buildFolder + '\\atari\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\scripts\\atari\\AtariBitmap.py ' + item + ' ' + buildFolder + '/atari/' + FileBase(item, '-atari.png') + '.img\n')
                
            # Charmaps/Tilesets
            for item in charmaps:
                fp.write('copy ' + item.replace('/', '\\') + ' ' + buildFolder + '\\atari\n')
                
            # Charsets
            if len(charset) > 0:
                fb = FileBase(charset[0], '-atari.png')
                fp.write('utils\\py27\python utils\\scripts\\atari\\AtariCharset.py ' + charset[0] + ' ' + buildFolder + '/atari/' + fb + '.chr\n')
                
            # Sprites    
            if len(sprites) > 0:
                spriteHeight = int(self.entry_AtariSpriteHeight.get())
                fp.write('utils\\py27\\python utils\\scripts\\atari\\AtariSprites.py ' + sprites[0] + ' ' + buildFolder + '/atari/sprites.dat ' + str(spriteHeight) + '\n')
                
            # Chunks
            if len(chunks) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py atari ' + chunks[0] + ' ' + buildFolder + '/atari/\n')

            # Shared Data
            for item in shared:
                fp.write('copy ' + item.replace('/','\\') + ' ' + buildFolder + '\\atari\n')

            # Music
            for item in music:
                fp.write('copy ' + item.replace('/','\\') + ' ' + buildFolder + '\\atari\\' + FileBase(item, '-atari.rmt') + '.mus\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'charmap.c', 'chunks.c', 'geom2d.c', 'joystick.c', 'mouse.c', 'music.c', 'net-base.c', 'net-url.c', 'net-tcp.c', 'net-udp.c', 'net-web.c', 'pixel.c', 'print.c', 'scaling.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Atari\\directory.c', 'Atari\\files.c']
            SList = ['atan2.s', 'chars.s', 'tiles.s', 'Atari\\DLI.s', 'Atari\\ROM.s', 'Atari\\scroll.s', 'Atari\\xbios.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t atarixl -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 -t atarixl unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 -t atarixl unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r ' + buildFolder + '/atari/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation
            fp.write('del utils\\cc65\\lib\\atarixl.lib\n')
            if len(self.Checkbutton_AtariNoText.state()):
                fp.write('copy utils\\cc65\\lib\\atarixl-notext.lib utils\\cc65\\lib\\atarixl.lib\n')
                symbols = '-Wl -D,__STACKSIZE__=$0400,-D,__CHARGENSIZE__=$0000 '
            else:
                fp.write('copy utils\\cc65\\lib\\atarixl-text.lib utils\\cc65\\lib\\atarixl.lib\n')
                symbols = '-Wl -D,__STACKSIZE__=$0400 '
            comp = 'utils\\cc65\\bin\\cl65 -o ' + buildFolder + '/atari/' + diskname.lower() + '.bin -m ' + buildFolder + '/' + diskname.lower() + '-atari.map -Cl -O -t atarixl ' + symbols + '-C atarixl-largehimem.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            fp.write(comp + 'unity/Atari/POKEY.s ' + buildFolder + '/atari/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_atarixl.lib\n')
            fp.write('utils\\cc65\\bin\\cl65 -t atarixl -C atari-asm.cfg -o ' + buildFolder + '/atari/basicoff.bin unity/Atari/BASICOFF.s\n')
            fp.write('utils\\scripts\\atari\\mads.exe -o:' + buildFolder + '/atari/rmt.bin unity/Atari/RMT.a65\n\n')

            # Merging
            fp.write('utils\\py27\\python utils\\scripts\\atari\\AtariMerge.py ' + buildFolder + '/atari/xautorun ' + buildFolder + '/atari/basicoff.bin ' + buildFolder + '/atari/' + diskname.lower() + '.bin ' + buildFolder + '/atari/rmt.bin\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- ATARI DISK BUILDER --------------- \n\n')

            # Clean-up build folder
            fp.write('del ' + buildFolder + '\\atari\\*.bin\n')
            fp.write('del ' + buildFolder + '\\atari\\*.lib\n')
            fp.write('del ' + buildFolder + '\\atari\\*.lst\n')
            
            # Copy xBios files
            fp.write('copy utils\\scripts\\atari\\xbios.com ' + buildFolder + '\\atari\\autorun\n')
            fp.write('copy utils\\scripts\\atari\\xbios.cfg ' + buildFolder + '\\atari\\xbios.cfg\n')

            # Disk builder
            if self.Combobox_AtariDiskSize.get() == '180KB':                
                diskSize = '720'
            else:
                diskSize = '1440'
            fp.write('utils\\scripts\\atari\\dir2atr.exe -d -B utils/scripts/atari/xboot.obx ' + diskSize + ' ' + buildFolder + '/' + diskname + '-atari.atr ' + buildFolder + '\\atari\n')

            # Info
            fp.write('\necho DONE\n')
            
            # Start emulator?
            if callEmu:
                fp.write('pause\n\n')
                fp.write('cd "utils\emulators\Altirra-3.20"\n')
                fp.write('Altirra.exe "..\\..\\..\\' + buildFolder + '\\' + diskname + '-atari.atr"\n')             

        ####################################################
        # C64 script
        bitmaps = list(self.listbox_C64Bitmap.get(0, END))
        charset = list(self.listbox_C64Charset.get(0, END))
        sprites = list(self.listbox_C64Sprites.get(0, END))
        chunks = list(self.listbox_C64Chunks.get(0, END))
        music = list(self.listbox_C64Music.get(0, END))
        with open('../../' + buildFolder+'/'+diskname+"-c64.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir c64\n')            
            fp.write('cd ..\n\n')            
            fp.write('del ' + buildFolder + '\\c64\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Bitmaps
            for item in bitmaps:
                fp.write('utils\\py27\\python utils\\scripts\\c64\\C64Bitmap.py ' + item + ' ' + buildFolder + '/c64/' + FileBase(item, '-c64.png') + '.img\n')
                
            # Charset    
            if len(charset) > 0:
                fb = FileBase(charset[0], '-c64.png')
                fp.write('utils\\py27\python utils\\scripts\\c64\\C64Charset.py ' + charset[0] + ' ' + buildFolder + '/c64/' + fb + '.chr\n')
                
            # Sprites
            if len(sprites) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\c64\\C64Sprites.py ' + sprites[0] + ' ' + buildFolder + '/c64/sprites.dat\n')
                
            # Chunks
            if len(chunks) > 0:
                fp.write('utils\\py27\\python utils\\scripts\\ProcessChunks.py c64 ' + chunks[0] + ' ' + buildFolder + '/c64/\n\n')
                
            # Music
            for item in music:
                fb = FileBase(item, '-c64.sid')
                fp.write('utils\\scripts\\c64\\sidreloc.exe -v -z 30-ff -p 08 ' + item + ' ' + buildFolder + '/c64/' + fb + '.sid\n')
                fp.write('if exist ' + buildFolder + '/c64/' + fb + '.sid (\n')
                fp.write('    utils\\scripts\\c64\\psid64.exe -n ' + buildFolder + '/c64/' + fb + '.sid\n')
                fp.write(') else (\n')
                fp.write('    echo Relocation impossible, using the original file instead...\n')
                fp.write('    copy ' + item.replace('/', '\\') + ' ' + buildFolder + '\\c64\\' + fb + '.prg\n')
                fp.write(')\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'charmap.c', 'chunks.c', 'geom2d.c', 'mouse.c', 'music.c', 'net-base.c', 'net-url.c', 'net-tcp.c', 'net-udp.c', 'net-web.c', 'pixel.c', 'print.c', 'scaling.c', 'sfx.c', 'sprites.c', 'widgets.c', 'C64\\directory.c', 'C64\\VIC2.c']
            SList = ['atan2.s', 'chars.s', 'tiles.s', 'C64\\joystick.s', 'C64\\scroll.s', 'C64\\ROM.s', 'C64\\SID.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t c64 -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 -t c64 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 -t c64 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r ' + buildFolder + '/c64/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation                        
            comp = 'utils\\cc65\\bin\\cl65 -o ' + buildFolder + '/c64/' + diskname.lower() + '.bin -m ' + buildFolder + '/' + diskname.lower() + '-c64.map -Cl -O -t c64 -C unity/C64/c64.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            fp.write(comp + buildFolder + '/c64/unity.lib unity/IP65/ip65_tcp.lib unity/IP65/ip65_c64.lib\n\n')
            
            # Compression
            if len(sprites) > 0:
                fp.write('utils\\scripts\\exomizer-3.0.2.exe sfx $180d ' + buildFolder + '/c64/' + diskname.lower() + '.bin ' + buildFolder + '/c64/sprites.dat -o ' + buildFolder + '/c64/loader.prg\n')          
            else:
                fp.write('utils\\scripts\\exomizer-3.0.2.exe sfx $180d ' + buildFolder + '/c64/' + diskname.lower() + '.bin -o ' + buildFolder + '/c64/loader.prg\n')

            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- C64 DISK BUILDER --------------- \n\n')

            # Disk builder
            fp.write('set C1541=utils\\scripts\\c64\\c1541 -format loader,666 d64 ' + buildFolder + '/' + diskname + '-c64.d64 -attach ' + buildFolder + '/' + diskname + '-c64.d64 ')
            fp.write('-write ' + buildFolder + '/c64/loader.prg loader.prg ')
            for item in bitmaps:
                fb = FileBase(item, '-c64.png')
                fp.write('-write ' + buildFolder + '/c64/' + fb + '.img ' + fb + '.img ')
            if len(charset) > 0:
                fb = FileBase(charset[0], '-c64.png')            
                fp.write('-write ' + buildFolder + '/c64/' + fb + '.chr ' + fb + '.chr ')                           
            for item in charmaps:
                fb = FileBase(item, '')
                fp.write('-write ' + item + ' ' + fb + ' ')
            for item in music:
                fb = FileBase(item, '-c64.sid')
                fp.write('-write ' + buildFolder + '/c64/' + fb + '.prg ' + fb + '.mus ')              
            for item in shared:
                fp.write('-write ' + item + ' ' + FileBase(item, '') + ' ')                
            fp.write('\nfor /f "tokens=*" %%A in (' + buildFolder + '\c64\chunks.lst) do set C1541=!C1541!-write %%A %%~nxA \n')
            fp.write('%C1541%\n')
               
            # Info
            fp.write('\n\necho DONE\n')
            
            # Start emulator?
            if callEmu:
                fp.write('pause\n\n')            
                fp.write('cd "utils\emulators\WinVICE-2.4"\n')
                fp.write('x64.exe "..\\..\\..\\' + buildFolder + '\\' + diskname + '-c64.d64"\n')

        ####################################################
        # Lynx script
        bitmaps = list(self.listbox_LynxBitmap.get(0, END))
        charset = list(self.listbox_LynxCharset.get(0, END))
        sprites = list(self.listbox_LynxSprites.get(0, END))
        chunks = list(self.listbox_LynxChunks.get(0, END))
        music = list(self.listbox_LynxMusic.get(0, END))
        with open('../../' + buildFolder+'/'+diskname+"-lynx.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir lynx\n')           
            fp.write('cd lynx\n')
            fp.write('del *.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Keyboard
            for item in ['cursor', 'keyboard']:
                fp.write('copy ..\\..\\utils\\scripts\\lynx\\' + item + '.png ' + item + '.png\n')
                fp.write('..\\..\\utils\\scripts\\png2bmp ' + item + '.png\n')
                fp.write('..\\..\\utils\\scripts\\lynx\\sprpck -t6 -p2 -u ' + item + '.bmp\n\n')         
                
            # Bitmaps
            for item in bitmaps:
                fb = FileBase(item, '-lynx.png')
                fp.write('copy ..\\..\\' + item.replace('/', '\\') + ' ' + fb + '.png\n')
                fp.write('..\\..\\utils\\scripts\\png2bmp ' + fb + '.png\n')
                fp.write('..\\..\\utils\\scripts\\lynx\\sprpck -t6 -p2 -u ' + fb + '.bmp\n')
            if len(bitmaps) > 0:
                fp.write('\n')
                
            # Charset
            if len(charset) > 0:
                fb = FileBase(charset[0], '-lynx.png')
                fp.write('..\\..\\utils\\py27\python ..\\..\\utils\\scripts\\lynx\\LynxCharset.py ..\\..\\' + charset[0].replace('/', '\\') + ' ' +fb + '.chr\n')
                fp.write('\n')
                
            # Sprites
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
                fp.write('\n')

            # Charmaps/Tilesets
            for item in charmaps:
                fb = FileBase(item, '')
                fp.write('copy ..\\..\\' + item.replace('/', '\\') + ' ' + fb + '\n')
            if len(charmaps) > 0:
                fp.write('\n')
                
            # Chunks
            fp.write('set /a CHUNKNUM=0\n')
            if len(chunks) > 0:
                fp.write('..\\..\\utils\\py27\\python ..\\..\\utils\\scripts\\ProcessChunks.py lynx ../../' + chunks[0] + ' ../../' + buildFolder + '/lynx/\n')
                fp.write('for /f "tokens=*" %%A in (chunks.lst) do set CHUNKNAMES=!CHUNKNAMES!_shkName!CHUNKNUM!,&&set /a CHUNKNUM+=1\n')
            fp.write('set /a FILENUM=!CHUNKNUM!+' + str(len(bitmaps)+len(charmaps)+len(charset)+len(music)+len(shared)) + '\n')

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
            for item in charmaps:
                fb = FileBase(item, '')
                filelist += fb + ','
            for i in range(len(charset)):
                fb = FileBase(charset[i], '-lynx.png')
                filelist += fb + '.chr,'
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
            fp.write('@echo .global _spriteNum  >> data.asm\n')
            fp.write('@echo .global _spriteData >> data.asm\n')
            fp.write('@echo .global _cursorData >> data.asm\n')
            fp.write('@echo .global _keybrdData >> data.asm\n')
            fp.write('@echo ; >> data.asm\n')
            
            # Num and sizes of files
            fp.write('@echo .segment "RODATA" >> data.asm\n')
            fp.write('@echo _fileNum: .byte %FILENUM% >> data.asm\n')  

            # List of file names and data
            if len(bitmaps) > 0 or len(charmaps) > 0 or len(charset) > 0 or len(music) > 0 or len(shared) > 0 or len(chunks) > 0:
                # Declare all Bitmap, Shared and Chunk files
                fp.write('@echo _fileSizes: .word %FILESIZES:~0,-1% >> data.asm\n')
                fp.write('@echo _fileNames: .addr ')
                counter = 0
                for i in range(len(bitmaps)):
                    if counter > 0:
                        fp.write(',')
                    fp.write('_bmpName' + str(i).zfill(2))
                    counter += 1
                for i in range(len(charmaps)):
                    if counter > 0:
                        fp.write(',')
                    fp.write('_mapName' + str(i).zfill(2))
                    counter += 1
                for i in range(len(charset)):
                    if counter > 0:
                        fp.write(',')
                    fp.write('_chrName' + str(i).zfill(2))
                    counter += 1
                for i in range(len(music)):
                    if counter > 0:
                        fp.write(',')
                    fp.write('_musName' + str(i).zfill(2))
                    counter += 1
                for i in range(len(shared)):
                    if counter > 0:
                        fp.write(',')
                    fp.write('_shrName' + str(i).zfill(2))
                    counter += 1
                if len(chunks) > 0:
                    if counter > 0:
                        fp.write(',')
                    fp.write('%CHUNKNAMES:~0,-1%')
                    counter += 1
                fp.write(' >> data.asm\n')

                # Write list of Bitmaps
                for i in range(len(bitmaps)):
                    fb = FileBase(bitmaps[i], '-lynx.png')
                    fp.write('@echo _bmpName' + str(i).zfill(2) + ': .byte "' + fb + '.img",0 >> data.asm\n')

                # Write list of Charmaps/Tilesets
                for i in range(len(charmaps)):
                    fb = FileBase(charmaps[i], '')
                    fp.write('@echo _mapName' + str(i).zfill(2) + ': .byte "' + fb + '",0 >> data.asm\n')

                # Write list of Charsets
                for i in range(len(charset)):
                    fb = FileBase(charset[i], '-lynx.png')
                    fp.write('@echo _chrName' + str(i).zfill(2) + ': .byte "' + fb + '.chr",0 >> data.asm\n')

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
                    fp.write('set /a IND=0\n')
                    fp.write('for /f "tokens=*" %%A in (chunks.lst) do @echo _shkName!IND!: .byte "%%~nxA",0 >> data.asm && set /a IND+=1\n')

                # Link list of bitmaps
                fp.write('@echo ; >> data.asm\n')
                for i in range(len(bitmaps)):
                    fb = FileBase(bitmaps[i], '-lynx.png')
                    fp.write('@echo .segment "BMP' + str(i) + 'DATA" >> data.asm\n')
                    fp.write('@echo _bmpData' + str(i).zfill(2) + ': .incbin "' + fb + '.spr" >> data.asm\n')                    

                # Link list of charmaps
                fp.write('@echo ; >> data.asm\n')
                for i in range(len(charmaps)):
                    fb = FileBase(charmaps[i], '')
                    fp.write('@echo .segment "BMP' + str(len(bitmaps)+i) + 'DATA" >> data.asm\n')
                    fp.write('@echo _mapData' + str(i).zfill(2) + ': .incbin "' + fb +'" >> data.asm\n')

                # Link list of charsets
                fp.write('@echo ; >> data.asm\n')
                for i in range(len(charset)):
                    fb = FileBase(charset[i], '-lynx.png')
                    fp.write('@echo .segment "BMP' + str(len(bitmaps)+len(charmaps)+i) + 'DATA" >> data.asm\n')
                    fp.write('@echo _chrData' + str(i).zfill(2) + ': .incbin "' + fb + '.chr" >> data.asm\n')                    
                    
                # Link list of musics
                for i in range(len(music)):
                    fp.write('@echo .segment "MUS' + str(i) + 'DATA" >> data.asm\n')
                    fp.write('@echo .import _musData' + str(i).zfill(2) + ' >> data.asm\n')
                    
                # Link list of shared
                for i in range(len(shared)):
                    fb = FileBase(shared[i], '')
                    fp.write('@echo .segment "SHR' + str(i) + 'DATA" >> data.asm\n')
                    fp.write('@echo _shrData' + str(i).zfill(2) + ': .incbin "' + fb + '" >> data.asm\n')
                    
                # Link list of chunks
                if len(chunks) > 0:
                    fp.write('set /a IND=0\n')
                    fp.write('for /f "tokens=*" %%A in (chunks.lst) do @echo .segment "SHK!IND!DATA" >> data.asm && @echo _shkData!IND!: .incbin "%%~nxA" >> data.asm && set /a IND+=1\n')
                    
            else:
                fp.write('@echo _fileSizes: .word 0 >> data.asm\n')
                fp.write('@echo _fileNames: .addr _dummyName >> data.asm\n')
                fp.write('@echo _dummyName: .byte 0 >> data.asm\n')
            fp.write('@echo ; >> data.asm\n')
            
            # Sprite Data 
            fp.write('@echo .segment "RODATA" >> data.asm\n')                
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
            fp.write('utils\\py27\\python utils/scripts/lynx/LynxConfig.py unity/Lynx/lynx.cfg ' + buildFolder + '/lynx/lynx.cfg ' + str(len(bitmaps)+len(charmaps)+len(charset)) + ' ' + str(len(music)) + ' ' + str(len(shared)) + ' %CHUNKNUM%\n')
            fp.write('utils\\py27\\python utils/scripts/lynx/LynxDirectory.py unity/Lynx/directory.s ' + buildFolder + '/lynx/directory.asm ' + str(len(bitmaps)+len(charmaps)+len(charset)) + ' ' + str(len(music)) + ' ' + str(len(shared)) + ' %CHUNKNUM%\n')
                        
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            symbols = ' -D __MUSSIZE__='  + self.entry_LynxMusicMemory.get().replace('$','0x') + ' -D __SHRSIZE__='  + self.entry_LynxSharedMemory.get().replace('$','0x')
            config  = ' -Wl -D,__MUSSIZE__='  + self.entry_LynxMusicMemory.get() + ',-D,__SHRSIZE__='  + self.entry_LynxSharedMemory.get()
            CList = ['bitmap.c', 'charmap.c', 'chunks.c', 'geom2d.c', 'hub.c', 'joystick.c', 'mouse.c', 'music.c', 'net-base.c', 'net-url.c', 'net-tcp.c', 'net-udp.c', 'net-web.c', 'pixel.c', 'print.c', 'scaling.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Lynx\\display.c', 'Lynx\\files.c']
            SList = ['atan2.s', 'chars.s', 'tiles.s', 'Lynx\\header.s', 'Lynx\\scroll.s', 'Lynx\\serial.s', 'Lynx\\suzy.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t lynx' + symbols + ' -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 -t lynx --cpu 65SC02 unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 -t lynx --cpu 65SC02 unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r ' + buildFolder + '/lynx/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation 
            comp = 'utils\\cc65\\bin\\cl65 -o ' + buildFolder + '/' + diskname.lower() + '-lynx.lnx -m ' + buildFolder + '/' + diskname.lower() + '-lynx.map -Cl -O -t lynx' + symbols + config + ' -C ' + buildFolder + '/lynx/lynx.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            for i in range(len(music)):
                comp += buildFolder + '/lynx/music' + str(i).zfill(2) + '.asm '
            fp.write(comp + 'unity/Lynx/sfx.s ' + buildFolder + '/lynx/directory.asm ' + buildFolder + '/lynx/data.asm ' + buildFolder + '/lynx/unity.lib\n')
            
            # Info
            fp.write('\necho DONE!\n\n')
            
            # Start emulator?
            if callEmu:
                fp.write('pause\n\n')
                fp.write('cd "utils\emulators\Handy-0.98-Hub"\n')
                fp.write('handy.exe "..\\..\\..\\' + buildFolder + '\\' + diskname + '-lynx.lnx"\n')


        ####################################################
        # Oric script
        bitmaps = list(self.listbox_OricBitmap.get(0, END))
        charset = list(self.listbox_OricCharset.get(0, END))
        sprites = list(self.listbox_OricSprites.get(0, END))
        chunks = list(self.listbox_OricChunks.get(0, END))
        music = list(self.listbox_OricMusic.get(0, END))
        with open('../../' + buildFolder+'/'+diskname+"-oric48k.bat", "wb") as fp:
            # Info
            fp.write('echo off\n\n')
            fp.write('setlocal enableextensions enabledelayedexpansion\n\n')
            fp.write('mkdir oric\n')            
            fp.write('cd ..\n\n')            
            fp.write('del ' + buildFolder + '\\oric\\*.* /F /Q\n\n')
            fp.write('echo --------------- CONVERT ASSETS ---------------  \n\n')
            
            # Process Bitmaps / Chunks / Sprites / Shared
            fp.write('cd utils\\scripts\\oric\n')
            for item in bitmaps:
                fb = FileBase(item, '-oric.png')
                fp.write('..\\..\\py27\\python OricBitmap.py ../../../' + item + ' ../../../' + buildFolder + '/oric/' + fb + '.dat ' + self.entry_OricDithering.get() + ' ' + self.entry_OricEnforcedColors.get() + '\n')
                fp.write('header -a0 ../../../' + buildFolder + '/oric/' + fb + '.dat ../../../' + buildFolder + '/oric/' + fb + '.img $A000\n')
                
            if len(charset) > 0:
                fb = FileBase(charset[0], '-oric.png')
                fp.write('..\\..\\py27\python OricCharset.py ../../../' + charset[0] + ' ../../../' + buildFolder + '/oric/' + fb + '.dat ' + self.entry_OricDithering.get() +  '\n') 
                fp.write('header -a0 ../../../' + buildFolder + '/oric/' + fb + '.dat ../../../' + buildFolder + '/oric/' + fb + '.chr $A000\n')
                
            if len(chunks) > 0:
                fp.write('..\\..\\py27\\python ProcessChunks.py ../../../' + chunks[0] + ' ../../../' + buildFolder + '/oric/ ' + self.entry_OricDithering.get() + '\n')
                fp.write('for /f "tokens=*" %%A in (..\\..\\..\\' + buildFolder + '\\oric\\chunks.lst) do header -a0 ../../../%%A ../../../%%A $8000\n')
            fp.write('cd ..\\..\\..\n')

            for item in charmaps:
                fb = FileBase(item, '')
                fp.write('utils\\scripts\\oric\\header -a0 ' + item + ' ' + buildFolder + '/oric/' + fb + ' $A000\n')
            
            if len(sprites) > 0:
                spriteHeight = int(self.entry_OricSpriteHeight.get())
                fp.write('utils\\py27\\python utils\\scripts\\oric\\OricSprites.py ' + sprites[0] + ' ' + buildFolder + '/oric/sprites.dat ' + str(spriteHeight) + '\n')
                fp.write('utils\\scripts\\oric\\header -a0 ' + buildFolder + '/oric/sprites.dat ' + buildFolder + '/oric/sprites.dat $7800\n')
                
            for item in music:
                fb = FileBase(item, '-oric.ym')
                fp.write('utils\\scripts\\oric\\ym2mym ' + item + ' ' + buildFolder + '/oric/' + fb + '.mus\n')
                fp.write('utils\\scripts\\oric\\header -h1 -a0 ' + buildFolder + '/oric/' + fb + '.mus ' + buildFolder + '/oric/' + fb + '.mus $8000\n')
                
            for item in shared:
                fb = FileBase(item, '')
                fp.write('utils\\scripts\\oric\\header -a0 ' + item + ' ' + buildFolder + '/oric/' + fb + ' $A000\n')
                
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- COMPILE PROGRAM ---------------\n\n')

            # Build Unity Library
            CList = ['bitmap.c', 'charmap.c', 'chunks.c', 'geom2d.c', 'hub.c', 'joystick.c', 'mouse.c', 'music.c', 'net-base.c', 'net-url.c', 'net-tcp.c', 'net-udp.c', 'net-web.c', 'pixel.c', 'print.c', 'scaling.c', 'sfx.c', 'sprites.c', 'widgets.c', 'Oric\\directory.c', 'Oric\\files.c']
            SList = ['atan2.s', 'chars.s', 'tiles.s', 'Oric\\blit.s', 'Oric\\paseIJK.s', 'Oric\\keyboard.s', 'Oric\\scroll.s', 'Oric\\sedoric.s', 'Oric\\MYM.s']
                         
            for file in CList:
                fp.write('utils\\cc65\\bin\\cc65 -Cl -O -t atmos -I unity unity\\' + file + '\n')
                fp.write('utils\\cc65\\bin\\ca65 -t atmos unity\\' + file[0:-2] + '.s\n')
                fp.write('del unity\\' + file[0:-2] + '.s\n')

            for file in SList:            
                fp.write('utils\\cc65\\bin\\ca65 -t atmos unity\\' + file + '\n')
            
            fp.write('utils\\cc65\\bin\\ar65 r ' + buildFolder + '/oric/unity.lib ')
            for file in CList:
                fp.write('unity\\' + file[0:-2] + '.o ')
            for file in SList:            
                fp.write('unity\\' + file[0:-2] + '.o ')
            fp.write('\n')
            
            # Compilation 
            comp = 'utils\\cc65\\bin\\cl65 -o ' + buildFolder + '/oric/' + diskname.lower() + '.bin -m ' + buildFolder + '/' + diskname.lower() + '-oric48k.map -Cl -O -t atmos -C unity/Oric/oric.cfg -I unity '
            for item in code:
                comp += (item + ' ')
            fp.write(comp + buildFolder + '/oric/unity.lib\n\n')

            # Fix header
            fp.write('utils\\scripts\\oric\\header.exe ' + buildFolder + '/oric/' + diskname.lower() + '.bin ' + buildFolder + '/oric/' + diskname.lower() + '.com $0501\n\n')
            
            # Compression
            fp.write('utils\\scripts\\exomizer-3.0.2.exe sfx bin ' + buildFolder + '/oric/' + diskname.lower() + '.com -o ' + buildFolder + '/oric/launch.com\n\n')            
            
            # Info
            fp.write('\necho DONE!\n\n')
            fp.write('echo --------------- ORIC DISK BUILDER --------------- \n\n')
            
            # Disk builder
            cmd = 'set TAP2DSK=utils\\scripts\\oric\\tap2dsk.exe -iLAUNCH.COM ' + buildFolder + '/oric/launch.com'
            if len(sprites) > 0:
                cmd += ' ' + buildFolder + '/oric/sprites.dat'
            for item in bitmaps:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '-oric.png') + '.img'
            if len(charset) > 0:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(charset[0], '-oric.png') + '.chr'
            for item in charmaps:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '')
            for item in music:
                fb = FileBase(item, '-oric.ym')
                cmd += ' ' + buildFolder + '/oric/' + fb + '.mus'
            for item in shared:
                cmd += ' ' + buildFolder + '/oric/' + FileBase(item, '')
            fp.write(cmd + '\n')
            if len(chunks) > 0:
                fp.write('for /f "tokens=*" %%A in (' + buildFolder + '\oric\chunks.lst) do set TAP2DSK=!TAP2DSK! %%A\n')
            fp.write('set TAP2DSK=%TAP2DSK% ' + buildFolder + '/' + diskname + '-oric48k.dsk\n')
            fp.write('%TAP2DSK%\n')
            fp.write('utils\\scripts\\oric\\old2mfm.exe ' + buildFolder + '/' + diskname + '-oric48k.dsk\n')
            
            # Info
            fp.write('\necho DONE\n')
            
            # Start emulator?
            if callEmu:
                fp.write('pause\n\n')
                fp.write('cd "utils\emulators\Oricutron-1.2-Hub"\n')
                fp.write('oricutron.exe -d "..\\..\\..\\' + buildFolder + '\\' + diskname + '-oric48k.dsk"\n')            
   
        # Done!
        if useGUI:
            messagebox.showinfo('Completed', 'Scripts succesfully written to the build folder!')
        
if __name__ == '__main__':
    if useGUI:
        root = Tk()
        app = Application(root)
        root.iconbitmap('builder.ico')
        root.resizable(False, False)
        root.mainloop()
    else:
        root = Tk()
        root.withdraw()
        app = Application(root)
        app.FileLoad('../../'+projectFile)
        app.GenerateBuilder()
