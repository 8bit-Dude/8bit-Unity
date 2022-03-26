"""
 * Copyright (c) 2018 Anthony Beaucamp.
 *
 * This software is provided 'as-is',without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications,and to alter it and redistribute it
 * freely,subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product,an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such,and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
"""
 
import io,os,sys,csv
from PIL import Image

charFile = sys.argv[1]
output   = sys.argv[2]

flagFile = charFile.replace('-atari.png','.csv')
            
#################################
# Read source bitmap and palette
charImg = Image.open(charFile)
charRaw = list(charImg.getdata())
print "Charmap size: {%i,%i}; Colors: %i" % (charImg.size[0],charImg.size[1],max(charRaw))

#################################
# Get image palette
rgb = []
dump = charImg.getpalette()
for i in range(len(dump)):
    rgb.append(dump[i*3:i*3+3])

#################################
# Find the 5 most popular colors
distrib = [0] * 128
for row in range(0, charImg.size[1], 8):
    for col in range(0, charImg.size[0], 4):
        # Collect block data
        block = []
        for j in range(0, 8):
            index = (row+j)*charImg.size[0]+col
            for i in range(0, 4):
                block.append(charRaw[index+i])
        for i in range(0, 128):
            if i in block:
                distrib[i] += 1                
sharedColors = sorted(range(len(distrib)), key=distrib.__getitem__)[123:128]
sharedColors.reverse()

####################################
# Make sure black is first color
if 0 in sharedColors:
    dump = [ 0 ]
    for i in sharedColors:
        if i !=0:
            dump.append(i)
    sharedColors = dump
colData = [chr(2*c) for c in sharedColors]

############################
# Rearrange into 4*8 blocks
charBlocks = []
attrData = [chr(0)] * 128
for row in range(0,charImg.size[1],8):
    for col in range(0,charImg.size[0],4):
        for j in range(0,8):
            for i in range(0,4):
                # Get color index, and re-assign to 1 of 5 colors available if needed
                c = charRaw[(row+j)*charImg.size[0]+col+i]
                if c not in sharedColors:
                    dist = [0]*5
                    nshColor = rgb[c]
                    for c in range(0,5):
                        shColor = rgb[c]
                        dist[c] = abs(shColor[0] - nshColor[0]) + abs(shColor[1] - nshColor[1]) + abs(shColor[2] - nshColor[2])
                    c = sharedColors[dist.index(min(dist))]
                index = sharedColors.index(c)
                if index < 4:
                    # Normal color
                    charBlocks.append(index)
                else:
                    # 5th color
                    charBlocks.append(3)
                    attrData[row*4+col/4] = chr(128)              

############################################
# Convert char and font data to Atari format
charData = [chr(0)] * (128*8)
for i in range(0,len(charBlocks),4):
    charData[i/4] = chr((charBlocks[i+0]<<6) + (charBlocks[i+1]<<4) + (charBlocks[i+2]<<2) + (charBlocks[i+3]<<0))

#######################
# Read character flags
flagData = [chr(0)] * 128
with open(flagFile) as csvfile:
    i = 0
    rows = csv.reader(csvfile,delimiter=',')
    for row in rows:
        for elt in row:
            flagData[i] = chr(int(elt))
            i += 1

###########################
# Write output binary file
f2 = io.open(output,'wb')
f2.write(''.join(colData))
f2.write(''.join(charData))
f2.write(''.join(attrData))
f2.write(''.join(flagData))
f2.close()
