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
from NESTools import GetPaletteIndex, EncodeTiles

input = sys.argv[1]
outCHR = sys.argv[2]
outPAL = sys.argv[3]

#################################
# Read source bitmap and palette
img1 = Image.open(input)
colors = max(list(img1.getdata()))
print "Charset size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

#######################################
# Get palette RGB
dump = img1.getpalette()
pal = [ ]
for i in range(4):
    rgb = dump[i*3:i*3+3]
    pal.append(chr(GetPaletteIndex(rgb)))
pal = ''.join(pal)

#######################################
# Encode data to Charset
data = ''.join(EncodeTiles(img1, 8, 8))
padding = ''.join([chr(0)] * (4096-len(data)))
            
###########################
# Output binary files
f2 = io.open(outCHR, 'wb')
f2.write(data)
f2.write(padding)
f2.close()

f2 = io.open(outPAL, 'wb')
f2.write(pal)
f2.close()
