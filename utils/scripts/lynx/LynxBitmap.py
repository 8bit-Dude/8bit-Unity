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
 
import io, os, sys, csv
from PIL import Image

input = sys.argv[1]
output = sys.argv[2]

#################################
# Read source bitmap and palette
img1 = Image.open(input)
pixdata = list(img1.getdata())
print "Bitmap size: {%i,%i}; Colors: %i" % (img1.size[0], img1.size[1], max(pixdata))

############################################
# Convert image data to Lynx format
data = [chr(0)] * 8364
for y in range(102):
    data[y*82] = chr(82)
    for x in range(80):
        base = y*160 + x*2
        data[y*82+1+x] = chr((pixdata[base+0]<<4) + (pixdata[base+1]<<0))                

###########################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(data))
f2.close()
