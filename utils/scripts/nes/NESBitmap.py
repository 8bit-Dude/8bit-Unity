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
 
import io,struct, sys
from PIL import Image
from NESTools import GetPaletteIndex, EncodeTiles, CrunchTiles, ExportNametable, ExportCharset

input = sys.argv[1]
outCHR = sys.argv[2]
outIMG = sys.argv[3]
maxTiles = int(sys.argv[4])

#################################
# Read source bitmap
img1 = Image.open(input)
colors = max(list(img1.getdata()))
print "Bitmap size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

#################################
# Redistribute colors
pixels = img1.load() # create the pixel map
for i in range(img1.size[0]): # for every pixel:
    for j in range(img1.size[1]):
        if int(i/2+j)%2:
        #if (i+j)%3:
            pixels[i,j] %= 4
        else:
            pixels[i,j] /= 4        

#######################################
# Create palette from RGB data
#rgb = img1.getpalette()
#pal = [ ]
#for i in range(4):
#    pal.append(GetPaletteIndex(rgb[i*3:i*3+3], pal))
pal = [0x0f, 0x17, 0x21, 0x2A]
palette = ''.join( [chr(p) for p in pal] )

#######################################
# Encode data to Chars
tileData = ''.join(EncodeTiles(img1, 8, 8))

#######################################
# Create initial name table and charset
chars = [ [ 0 ] * 16 ]
for i in range(768):
    chars.append( [ ord(c) for c in tileData[i*16:(i+1)*16] ] )
names = range(1,769)

#######################################
# Crunch name table by dropping similar tiles
names, chars = CrunchTiles(names, chars, maxTiles) 
    
#######################################
# Export data
ExportCharset(outCHR, chars, maxTiles)
ExportNametable(outIMG, names, palette)
