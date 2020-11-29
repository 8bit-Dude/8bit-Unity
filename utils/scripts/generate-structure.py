SOURCE = """
        self.entries = [ self.entry_Disk, 
                         self.entry_AppleSpriteFrames, self.entry_AppleSpriteWidth, self.entry_AppleSpriteHeight, 
                         self.entry_AtariSpriteFrames, self.entry_AtariSpriteWidth, self.entry_AtariSpriteHeight, 
                         self.entry_C64SpriteFrames,   self.entry_C64SpriteWidth,   self.entry_C64SpriteHeight, 
                         self.entry_LynxSpriteFrames,  self.entry_LynxSpriteWidth,  self.entry_LynxSpriteHeight, 
                         self.entry_OricSpriteFrames,  self.entry_OricSpriteWidth,  self.entry_OricSpriteHeight,
                         self.entry_OricDithering,     self.entry_LynxMusicMemory,  self.entry_LynxSharedMemory ]
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
        self.comboboxes = [ self.Combobox_AtariDiskSize, self.combobox_OricImageQuality ]
"""

import re, collections

matches = re.findall(r'self\.(\w+)_([A-Z][a-z0-9]+)?([A-Z]\w+)', SOURCE)

#print(matches)

tree = collections.OrderedDict([
    ('general', collections.OrderedDict()),
    ('platform', collections.OrderedDict())
])

for mat in matches:
    (kind, platform, name) = mat
    key = name[0].lower() + name[1:]

    if platform:
        if not platform in tree['platform']:
            tree['platform'][platform] = collections.OrderedDict()

        tree['platform'][platform][key] = (kind, kind + '_' + platform + name)
    else:
        tree['general'][key] = (kind, kind + '_' + name)

#print(tree)

def dict_to_source(d, indent):
    for k, v in d.items():
        if isinstance(v, tuple):
            print("{}'{}': ('{}', self.{}),".format(indent, k, v[0], v[1]))
        else:
            print("{}'{}': {{".format(indent, k))
            dict_to_source(v, indent + '    ')
            print("{}}},".format(indent))

print("self.json_structure = {")
dict_to_source(tree, '    ')
print("}")
