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

DHRBytes = [
	[0x00,0x00,0x00,0x00],
	[0x08,0x11,0x22,0x44],
	[0x11,0x22,0x44,0x08],
	[0x19,0x33,0x66,0x4C],
	[0x22,0x44,0x08,0x11],
	[0x2A,0x55,0x2A,0x55],
	[0x33,0x66,0x4C,0x19],
	[0x3B,0x77,0x6E,0x5D],
	[0x44,0x08,0x11,0x22],
	[0x4C,0x19,0x33,0x66],
	[0x55,0x2A,0x55,0x2A],
	[0x5D,0x3B,0x77,0x6E],
	[0x66,0x4C,0x19,0x33],
	[0x6E,0x5D,0x3B,0x77],
	[0x77,0x6E,0x5D,0x3B],
	[0x7F,0x7F,0x7F,0x7F]]

def SetColor(block, pixel, color):
    offset = 0
    if (pixel > 3):
        offset = 1

    if pixel == 0:
        block[0] &= 0x70
        block[0] |= (DHRBytes[color][0] & 0x0f)
    if pixel == 1:
        block[0] &= 0x0f
        block[0] |= (DHRBytes[color][0] & 0x70)
        block[2] &= 0x7e
        block[2] |= (DHRBytes[color][1] & 0x01)
    if pixel == 2:
        block[2] &= 0x61
        block[2] |= (DHRBytes[color][1] & 0x1e)
    if pixel == 3:
        block[2] &= 0x1f
        block[2] |= (DHRBytes[color][1] & 0x60)
        block[1] &= 0x7c
        block[1] |= (DHRBytes[color][2] & 0x03)
    if pixel == 4:
        block[1] &= 0x43
        block[1] |= (DHRBytes[color][2] & 0x3c)
    if pixel == 5:
        block[1] &= 0x3f
        block[1] |= (DHRBytes[color][2] & 0x40)
        block[3] &= 0x78
        block[3] |= (DHRBytes[color][3] & 0x07)
    if pixel == 6:
        block[3] &= 0x07
        block[3] |= (DHRBytes[color][3] & 0x78)


# Read source file
img1 = Image.open(sys.argv[1])
pixdata = list(img1.getdata())
#print pixdata

# Create sprite file
f1 = io.open(sys.argv[2], 'wb')

# Write load address ($AB00)
f1.write(chr(0));
f1.write(chr(171));

# Process in blocks of 7 pixels > 4 bytes
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, j, pixdata[i*7+j])
    for j in [0,1,2,3]:
        f1.write(chr(block[j]))

# Process in blocks of 7 pixels > 4 bytes (Shifted by 2 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        if j<2:
            SetColor(block, j, 5)
        else:
            SetColor(block, j, pixdata[i*7+j-2])
    for j in [0,1,2,3]:
        f1.write(chr(block[j]))            
        
# Process offset blocks of 7 pixels > 4 bytes (Shifted by 3.5 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        SetColor(block, (j+4)%7, pixdata[i*7+j])
    for j in [1,0,3,2]:
        f1.write(chr(block[j]))            

# Process offset blocks of 7 pixels > 4 bytes (Shifted by 5.5 pixels)
for i in range(len(pixdata)/7):
    block = [0,0,0,0]
    for j in range(7):
        if j<1:
            SetColor(block, (j+3)%7, 5)
        else:
            SetColor(block, (j+3)%7, pixdata[i*7+j-2])
    for j in [1,0,3,2]:
        f1.write(chr(block[j]))            

f1.close()
