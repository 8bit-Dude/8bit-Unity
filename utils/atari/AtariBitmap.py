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

######################
# Inter-player colors
palette = [ chr(0x00), chr(0x24), chr(0x86), chr(0xd8) ]

#####################
# Read source bitmap
img1 = Image.open(sys.argv[1])
pixdata = list(img1.getdata())

################################
# Convert pixel data to buffers 
buf1 = [0] * 8000
buf2 = [0] * 8000
for y in range(200):
    for x in range(160):
        # Convert PNG to INP index 
        shift = 6 - 2*(x%4)
        color = pixdata[y*320+x*2]
        if (x+y)%2:   # Alternate to create checker board
            col2 = color%4 << shift
            col1 = color/4 << shift
        else:
            col1 = color%4 << shift
            col2 = color/4 << shift
            
        # Assign bits in both buffers
        offset = y*40+x/4
        mask = 255 - (3<<shift)            
        buf1[offset] = (buf1[offset] & mask) | col1
        buf2[offset] = (buf2[offset] & mask) | col2  
        
#################
# Convert to CHR
for i in range(8000):
    buf1[i] = chr(buf1[i])
    buf2[i] = chr(buf2[i])
        
########################
# Write output INP file
f2 = io.open(output, 'wb')	
f2.write(''.join(palette))
f2.write(''.join(buf1))
f2.write(''.join(buf2))
f2.close()
