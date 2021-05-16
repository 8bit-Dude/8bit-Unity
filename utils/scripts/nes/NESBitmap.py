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
from NESTools import GetPaletteIndex, EncodeTiles, TileCompare, RLECompression

input = sys.argv[1]
outCHR = sys.argv[2]
outIMG = sys.argv[3]
maxTiles = int(sys.argv[4])

#######################################
# Read default font file
f1 = io.open("utils/scripts/nes/font.chr", 'rb')
font = f1.read()
f1.close()

#################################
# Read source bitmap and palette
img1 = Image.open(input)
colors = max(list(img1.getdata()))
print "Bitmap size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

#######################################
# Get palette RGB
dump = img1.getpalette()
pal = [ ]
for i in range(4):
    rgb = dump[i*3:i*3+3]
    pal.append(GetPaletteIndex(rgb, pal))
palData = ''.join( [chr(p) for p in pal] )

#######################################
# Encode data to Chars
tileData = ''.join(EncodeTiles(img1, 8, 8))

#######################################
# Create initial name table and charset
chars = [ [ 0 ] * 16 ]
for i in range(800):
    chars.append( [ ord(c) for c in tileData[i*16:(i+1)*16] ] )
names = range(1,801)

#######################################
# Drop identical chars
ch = 1; nm = 0
while ch<len(chars):
    matched = False
    for i in range(0, ch):
        if (chars[i] == chars[ch]):
            matched = True
            break
    if matched:
        names[nm] = i
        for j in range(nm+1, 800):
            names[j] -= 1
        chars.pop(ch)
    else:
        ch += 1
    nm += 1

#######################################
# Compute Tile Popularity
popular = [ 0 ] * len(chars) 
popular[0] = 999 # Always keep first tile
for i in range(len(names)):
    popular[names[i]] += 1

#######################################
# Compute Tile Similarity
similar = [ [], [], [], ]
for i in range(len(chars)-1):
    for j in range(i+1, len(chars)):
        n = TileCompare(chars[i], chars[j])
        similar[0].append(n)
        similar[1].append(i)
        similar[2].append(j)

#######################################
# Sort Index of Similarity
indices = sorted(range(len(similar[0])), key=lambda k: similar[0][k])

#######################################
# Inspect most similar Tiles for dropping
i = -1; drop = []; swap = []
while len(chars) > maxTiles+len(drop):
    ind = indices[i]
    tile1 = similar[1][ind]
    tile2 = similar[2][ind]
    
    # Check that neither tile dropped yet
    if tile1 not in drop and tile2 not in drop:
        # Pop least popular tile  
        if popular[tile1]<popular[tile2]:
            drop.append(tile1)
            swap.append(tile2)
        else:
            drop.append(tile2)
            swap.append(tile1)
    i -= 1

#######################################
# Swap indices of Tiles to be dropped
for i in range(len(drop)):
    d = drop[i]
    s = swap[i]
    for i in range(800):
        if names[i] == d:
            names[i] = s

#######################################
# Drop unused tiles
for tile in sorted(drop, reverse=True):
    for i in range(800):
        if names[i] > tile:
            names[i] -= 1
    chars.pop(tile)
    
#######################################
# Encode and write Charset
for i in range(len(chars)):
    char = chars[i]
    chars[i] = ''.join([ chr(c) for c in char ])
chars = ''.join(chars)
f2 = io.open(outCHR, 'wb')
f2.write(chars[0:16*maxTiles])
f2.write(font[16*maxTiles:16*256])
f2.close()

#######################################
# Encode and write Nametable
head = [0] * 64
foot = [0] * 160
names = head+names+foot
rle = RLECompression(names)
data = ''.join([chr(b) for b in rle])
f2 = io.open(outIMG, 'wb')
f2.write(palData)
f2.write(data)
f2.close()
