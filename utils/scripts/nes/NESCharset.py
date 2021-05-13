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
 
import io,struct, sys, csv
from PIL import Image
from NESTools import GetPaletteIndex, EncodeTiles

input = sys.argv[1]
outCHR = sys.argv[2]
outFLG = sys.argv[3]

#################################
# Read source bitmap and palette
img1 = Image.open(input)
colors = max(list(img1.getdata()))
print "Charset size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

#######################################
# Encode data to Charset file
charData = ''.join(EncodeTiles(img1, 8, 8))
padding = ''.join([chr(0)] * (4096-len(charData)))

f2 = io.open(outCHR, 'wb')
f2.write(charData)
f2.write(padding)
f2.close()

################################
# Convert character flags
flagData = [chr(0)] * 128
with open(input.replace('-nes.png', '.csv')) as csvfile:
    i = 0
    rows = csv.reader(csvfile, delimiter=',')
    for row in rows:
        for elt in row:
            flagData[i] = chr(int(elt))
            i += 1
flagData = ''.join(flagData)
            
#######################################
# Generate palette
dump = img1.getpalette()
palData = [ ]
for i in range(4):
    rgb = dump[i*3:i*3+3]
    palData.append(chr(GetPaletteIndex(rgb)))
palData = ''.join(palData)

f2 = io.open(outFLG, 'wb')
f2.write(flagData)
f2.write(palData)
f2.close()
