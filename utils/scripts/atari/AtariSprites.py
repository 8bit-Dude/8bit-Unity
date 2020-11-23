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
height = int(sys.argv[3])

#################################
# Read source bitmap and palette
img1 = Image.open(input)
rawdata = list(img1.getdata())
colors = max(rawdata)
print "Sprite sheet size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

###################################
# Rearrange into 8 * Height blocks
pixdata = []
for row in range(0, img1.size[1], height):
    for col in range(0, img1.size[0], 8):
        for j in range(0, height):        
            for i in range(0, 8):
                pixdata.append(rawdata[(row+j)*img1.size[0]+col+i])

################################
# Convert pixel data to buffers 
block = 8*height
frames = len(pixdata) / block
numBytes = (colors*frames*height)
data = [chr(0)] * numBytes
for color in range(1,colors+1):
    for frame in range(frames):
        for i in range(0, block, 8):
            data[(color-1)*frames*height+frame*height+i/8] = \
                chr(((pixdata[frame*block+i+7]==color)<<0) + ((pixdata[frame*block+i+6]==color)<<1) + 
                    ((pixdata[frame*block+i+5]==color)<<2) + ((pixdata[frame*block+i+4]==color)<<3) + 
                    ((pixdata[frame*block+i+3]==color)<<4) + ((pixdata[frame*block+i+2]==color)<<5) + 
                    ((pixdata[frame*block+i+1]==color)<<6) + ((pixdata[frame*block+i+0]==color)<<7))

###########################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(chr(len(data)%256))
f2.write(chr(len(data)/256))
f2.write(''.join(data))
f2.close()
