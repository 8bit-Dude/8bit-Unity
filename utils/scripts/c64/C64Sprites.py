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
 
import io,struct, sys
from PIL import Image

input = sys.argv[1]
output = sys.argv[2]

#################################
# Read source bitmap and correct indices
img1 = Image.open(input)
pixdata = list(img1.getdata())
for i in range(len(pixdata)):
    if pixdata[i] == 1: 
        pixdata[i] = 2
        continue
    if pixdata[i] == 2: 
        pixdata[i] = 1
        continue

################################
# Convert pixel data to buffers 
block = 12*21
frames = len(pixdata) / block
sprdata = [chr(0)] * (frames*64)
for f in range(frames):
    for i in range(0, block, 4):
        sprdata[f*64+i/4] = chr((pixdata[f*block+i+3]) + (pixdata[f*block+i+2]<<2) + (pixdata[f*block+i+1]<<4) + (pixdata[f*block+i]<<6))

###########################
# Write output binary file
f2 = io.open(output, 'wb')	
f2.write(''.join([chr(0),chr(0xc7)]))
f2.write(''.join(sprdata))
f2.close()
