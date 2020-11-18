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
 
from AppleHires import *
from PIL import Image
import io, os, sys, csv
from copy import *

mode = sys.argv[1]
charFile = sys.argv[2]
output   = sys.argv[3]

flagFile = charFile.replace('-apple.png', '.csv')

#################################
# Read source bitmap and palette
charImg = Image.open(charFile)
charRaw = list(charImg.getdata())
colors = max(charRaw)
print "Charmap size: {%i,%i}; Colors: %i" % (charImg.size[0], charImg.size[1], colors)

#######################################
# Rearrange into 2 sets of 3.5*8 blocks
dataL = []; dataR = []
for k in range(0, 8):
    
    if mode == 'double':  
        auxL = []; auxR = []
        mainL = []; mainR = []

    for row in range(0, charImg.size[1], 8):
        for col in range(0, charImg.size[0], 7):
            pixels = charRaw[(row+k)*charImg.size[0]+col:(row+k)*charImg.size[0]+col+7]
            if mode == 'single':   
                # Reduce palette?
                if colors > 6:
                    pixels = RemapDHR2SHR(pixels)
                res = AssignColorGroup(pixels)    
                pixels = res[0]

                # Left position
                block1 = deepcopy(res[1])
                for j in range(7):
                    SetSHRColor(block1, j, pixels[j])
                
                # Right position
                block2 = deepcopy(res[1])
                for j in range(7):
                    SetSHRColor(block2, (j+3)%7, pixels[j])

                # Save in respective banks
                dataL.append(chr(block1[0]))
                dataL.append(chr(block2[0]))
                
                dataR.append(chr(block2[1]))
                dataR.append(chr(block1[1]))
                
            else:
                # Left position
                block1 = [0,0,0,0]
                for j in range(7):
                    SetDHRColor(block1, j, pixels[j])
                    
                # Right position
                block2 = [0,0,0,0]
                for j in range(7):
                    SetDHRColor(block2, (j+3)%7, pixels[j])

                # Save in respective banks
                auxL.append(chr(block1[0]))
                auxL.append(chr(block2[0]))
                mainL.append(chr(block1[1]))
                mainL.append(chr(block2[1]))
                
                auxR.append(chr(block2[2]))
                auxR.append(chr(block1[2]))
                mainR.append(chr(block2[3]))
                mainR.append(chr(block1[3]))

    if mode == 'double':  
        dataL.append(''.join(auxL))
        dataL.append(''.join(mainL))
        dataR.append(''.join(auxR))
        dataR.append(''.join(mainR))

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
            
############################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(dataL))
f2.write(''.join(dataR))
f2.write(''.join(flagData))
f2.close()
