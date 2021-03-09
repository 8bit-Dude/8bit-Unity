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

input = sys.argv[1]
output = sys.argv[2]
width = int(sys.argv[3])
height = int(sys.argv[4])

#################################
# Read source bitmap and palette
img1 = Image.open(input)
rawdata = list(img1.getdata())
colors = max(rawdata)
print "Sprite sheet size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

#######################################
# Rearrange into Width * Height blocks
pixdata = []
for row in range(0, img1.size[1], height):
    for col in range(0, img1.size[0], width):
        for j in range(0, height):        
            for i in range(0, width):
                pixdata.append(rawdata[(row+j)*img1.size[0]+col+i])
            if (width%2):   # Fix odd number of cols
                pixdata.append(0)
if (width%2):
    width += 1

################################
# Convert pixel data to buffers 
block = width*height
frames = len(pixdata) / block
numBytes = frames*(height*(width/2+1)+1)
data = [chr(0)] * numBytes
i = 0
for f in range(frames):
    for h in range(0, height):
        data[i] = chr(width/2+1)
        i += 1
        for w in range(0, width/2):
            base = f*block + h*width + w*2
            data[i] = chr((pixdata[base+0]<<4) + (pixdata[base+1]<<0))
            i += 1
    data[i] = chr(0)
    i += 1
            
###########################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(data))
f2.close()
