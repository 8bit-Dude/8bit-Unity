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

from PIL import Image
import io, sys

input = sys.argv[1]
output = sys.argv[2]

##############################
# Set color of specific pixel
def SetColor(block, pixel, color):

    if pixel == 0:
        block[0] |= (color & 15)
    if pixel == 1:
        block[0] |= (color &  7) << 4
        block[1] |= (color &  8) >> 3
    if pixel == 2:
        block[1] |= (color & 15) << 1
    if pixel == 3:
        block[1] |= (color &  3) << 5
        block[2] |= (color & 12) >> 2
    if pixel == 4:
        block[2] |= (color & 15) << 2
    if pixel == 5:
        block[2] |= (color &  1) << 6
        block[3] |= (color & 14) >> 1
    if pixel == 6:
        block[3] |= (color & 15) << 3

###################
# Read source file
img1 = Image.open(input)
pixdata = list(img1.getdata())
#print pixdata

# Create sprite file
f1 = io.open(output, 'wb')

# Write load address ($A800)
f1.write(chr(0x00));
f1.write(chr(0xa8));

# Process in blocks of 7 pixels > 4 bytes
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, j, pixdata[i*7+j])
    for j in [0,2,1,3]:
        f1.write(chr(block[j]))

# Process in blocks of 7 pixels > 4 bytes (Shifted by 2 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, (j+2)%7, pixdata[i*7+j])
    for j in [0,2,1,3]:
        f1.write(chr(block[j]))            
        
# Process offset blocks of 7 pixels > 4 bytes (Shifted by 4 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, (j+4)%7, pixdata[i*7+j])
    for j in [2,0,3,1]:
        f1.write(chr(block[j]))            

# Process offset blocks of 7 pixels > 4 bytes (Shifted by 5 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, (j+5)%7, pixdata[i*7+j])
    for j in [2,0,3,1]:
        f1.write(chr(block[j]))            

f1.close()
