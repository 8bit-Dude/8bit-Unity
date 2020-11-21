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
 
import io, os, sys, csv
from PIL import Image

charFile = sys.argv[1]
output   = sys.argv[2]

flagFile = charFile.replace('-atari.png', '.csv')

#################################
# Read source bitmap and palette
charImg = Image.open(charFile)
charRaw = list(charImg.getdata())
print "Charmap size: {%i,%i}; Colors: %i" % (charImg.size[0], charImg.size[1], max(charRaw))

############################
# Rearrange into 4*8 blocks
charBlocks = []
for row in range(0, charImg.size[1], 8):
    for col in range(0, charImg.size[0], 4):
        for j in range(0, 8):
            for i in range(0, 4):
                charBlocks.append(charRaw[(row+j)*charImg.size[0]+col+i])
                
###############################
# Process character attributes
attrData = [chr(0)] * 128
for i in range(0, len(charBlocks)/32):
    attr = max(charBlocks[i*32:(i+1)*32])
    if attr is 4:
        for j in range(0, 32):
            if charBlocks[i*32+j] == 4:
                charBlocks[i*32+j] = 3
        attrData[i] = chr(128)                

############################################
# Convert char and font data to C64 format
charData = [chr(0)] * (128*8)
for i in range(0, len(charBlocks), 4):
    charData[i/4] = chr((charBlocks[i+0]<<6) + (charBlocks[i+1]<<4) + (charBlocks[i+2]<<2) + (charBlocks[i+3]<<0))

#######################
# Read character flags
flagData = [chr(0)] * 128
with open(flagFile) as csvfile:
    i = 0
    rows = csv.reader(csvfile, delimiter=',')
    for row in rows:
        for elt in row:
            flagData[i] = chr(int(elt))
            i += 1

###########################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(charData))
f2.write(''.join(attrData))
f2.write(''.join(flagData))
f2.close()
