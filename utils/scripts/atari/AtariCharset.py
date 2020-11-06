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

input = sys.argv[1]
output = sys.argv[2]

#################################
# Read source bitmap and palette
img1 = Image.open(input)
rawdata = list(img1.getdata())
colors = max(rawdata)
print "Charmap size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

###################################
# Rearrange into 4 * 8 blocks
pixdata = []
for row in range(0, img1.size[1], 8):
    for col in range(0, img1.size[0], 4):
        for j in range(0, 8):
            for i in range(0, 4):
                pixdata.append(rawdata[(row+j)*img1.size[0]+col+i])

################################
# Convert pixel data to charset 
sprdata = [chr(0)] * (len(pixdata)/4)
for i in range(0, len(pixdata), 4):
    sprdata[i/4] = chr((pixdata[i+0]<<6) + (pixdata[i+1]<<4) + (pixdata[i+2]<<2) + (pixdata[i+3]<<0))

################################
# Read character flags
flagdata = [chr(0)] * 256
with open(input.replace('-atari.png', '.csv')) as csvfile:
    i = 0
    rows = csv.reader(csvfile, delimiter=',')
    for row in rows:
        for elt in row:
            flagdata[i] = chr(int(elt))
            i += 1

###########################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(sprdata))
f2.write(''.join(flagdata))
f2.close()
