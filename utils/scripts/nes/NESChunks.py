"""
 * Copyright (c) 2021 Anthony Beaucamp.
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
 
import io,os,struct, sys
from PIL import Image
from NESTools import EncodeTiles, CrunchTiles, ExportNametable, ExportCharset

def FileBase(filepath, suffix):
    # Return asset file base
    name = os.path.basename(filepath).lower().replace(suffix, '')
    name = name.split("-")
    return name[0]

############################################    
# Process chunks definition file
chunkfile = sys.argv[1]
outfolder = sys.argv[2]
maxTiles  = int(sys.argv[3])
imgfile = chunkfile[0:-4] + ".png"

script = open(chunkfile, "r")
lines = script.readlines() 
script.close()

coorLst = []
cropLst = []

for line in lines:
    if line[0] == '\'':
        # Parse charset name
        offset1 = 1
        offset2 = 1    
        while line[offset2] != '\'':
            offset2 += 1
        chrfile = outfolder + '/' + line[offset1:offset2]
        chrfile = chrfile.replace('//','/')
    
    if line[0] == '[':
        # Parse coordinates
        offset1 = 1
        offset2 = 1
        while line[offset2] != ']':
            offset2 += 1
        coords = [int(s) for s in line[offset1:offset2].split(',')]
        coorLst.append(coords)

        # Crop required section of image
        img = Image.open(imgfile)
        cropLst.append(img.crop((8*coords[0], 8*coords[1], 8*coords[0]+8*coords[2], 8*coords[1]+8*coords[3])))

##########################    
# Read reference charset
f = io.open(chrfile, 'rb')
charRaw = f.read()
f.close()

# Convert back to numerals
chars = []
for i in range(maxTiles):
    chars.append( [ ord(c) for c in charRaw[i*16:(i+1)*16] ] )
    
###########################    
# Read reference nametable
namfile = chrfile.replace('chr','nam')
f = io.open(namfile, 'rb')
names = f.read()
f.close()

# Convert back to numerals
names = [ ord(c) for c in names ]

###########################    
# Read reference nametable
imgfile = chrfile.replace('chr','img')
f = io.open(imgfile, 'rb')
palette = f.read(4)

############################
# Create list of chunks
sizeLst  = []
rangeLst = []
attrLst  = []
for i in range(len(cropLst)):
    # Add names and chars
    range1 = len(names)
    tileData = ''.join(EncodeTiles(cropLst[i], 8, 8))
    for j in range( len(tileData) / 16 ):
        names.append( len(chars) )
        chars.append( [ ord(c) for c in tileData[j*16:(j+1)*16] ] )
    range2 = len(names)
    rangeLst.append( [ range1, range2 ] )
    
    # Compute number of attributes
    coords = coorLst[i]
    h = coords[2]/4+1; 
    w = coords[3]/4+1;
    attrLst.append( h*w )
    
    # Compute size of chunk
    sizeLst.append(4+(range2-range1)+h*w)
    print 'Adding Chunk ', coords

#######################################
# Re-crunch name table by dropping similar tiles
names, chars = CrunchTiles(names, chars, maxTiles) 
  
#######################################
# Re-export updated reference names/chars
ExportCharset(chrfile, chars, maxTiles)
ExportNametable(imgfile, names[0:768], palette)

#######################################
# Output chunk data
fb = FileBase(chunkfile, '.txt')
f2 = io.open(outfolder + fb + '.chk', 'wb')

# Write header information
f2.write(chr(len(sizeLst)))
for i in range(len(sizeLst)):
    f2.write(struct.pack('H', sizeLst[i]))

# Write coords/names/attributes
for i in range(len(sizeLst)):

    # Write binary table
    cor = ''.join([chr(b) for b in coorLst[i]])
    nam = ''.join([chr(b) for b in names[rangeLst[i][0]:rangeLst[i][1]]])
    att = ''.join([chr(0) for b in range(attrLst[i])])
    f2.write(cor)
    f2.write(nam)
    f2.write(att)

f2.close()
    
    
