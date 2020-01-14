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
pixdata = list(img1.getdata())
colors = max(pixdata)
print "Number of Colors: %i" % (colors)

################################
# Convert pixel data to buffers 
block = 8*height
frames = len(pixdata) / block
numBytes = (colors*frames*height)
sprdata = [chr(0)] * numBytes
for v in range(1,max(pixdata)+1):
    for f in range(frames):
        for i in range(0, block, 8):
            sprdata[(v-1)*frames*height+f*height+i/8] = \
                chr(((pixdata[f*block+i+7]==v)<<0) + ((pixdata[f*block+i+6]==v)<<1) + ((pixdata[f*block+i+5]==v)<<2) + ((pixdata[f*block+i+4]==v)<<3)
                  + ((pixdata[f*block+i+3]==v)<<4) + ((pixdata[f*block+i+2]==v)<<5) + ((pixdata[f*block+i+1]==v)<<6) + ((pixdata[f*block+i+0]==v)<<7))

###########################
# Write output binary file
f2 = io.open(output, 'wb')
begLow = chr(0x00)
begHig = chr(0x97)
endLow = chr((0x9700+numBytes-1)%256)
endHig = chr((0x9700+numBytes-1)/256)
f2.write(''.join([chr(0xff),chr(0xff),begLow,begHig,endLow,endHig]))
f2.write(''.join(sprdata))
f2.close()
