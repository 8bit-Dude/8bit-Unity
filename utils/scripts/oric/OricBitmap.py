"""
 * Copyright (c) 2020 Anthony Beaucamp.
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

import io,struct, sys, subprocess
from PIL import Image
from collections import Counter
from math import sqrt

imgFile = sys.argv[1]
rawFile = sys.argv[2]
dither  = sys.argv[3]
try:
    enforce = [int(n) for n in sys.argv[4].split(',')]
except:
    enforce = []
    
###################
# Call PictOric
subprocess.call(["luajit.exe", "PictOric.lua", dither, imgFile, rawFile])


# Process RAW data again to enforce colors
if len(enforce) > 0:
    # Read source file
    img1 = Image.open(imgFile)
    pixdata = list(img1.getdata())
    
    # Read binary files
    f1 = io.open(rawFile, 'rb')
    data = [c for c in f1.read()]
    f1.close()   
    
    # Get palette RGB
    rgb = []
    dump = img1.getpalette()
    for i in range(20):
        rgb.append(dump[i*3:i*3+3])

    # Inversion groups
    groups = [ [0,1,2,3,4],         # line 1 and 2 non-inverted
               [5,6,7,8,9],         # line 1 inverted
               [10,11,12,13,14],    # line 2 inverted
               [15,16,17,18,19] ]   # line 1 and 2 inverted
               
    # Convert to AIC format
    for y in range(100):       
        # Process in blocks of 3x1 pixels (encoded as 6x2)
        for x in range(0,39):
            # Extract block
            location = y*2*240+x*6+6
            block = [pixdata[location], pixdata[location+2], pixdata[location+4]]            

            # Does block contain enforced color?
            if len([i for i in block if i in enforce]) == 0:
                continue
            
            # Find most frequent colour in block
            gcount = []
            for i in block:
                for g in range(len(groups)):
                    if i in groups[g]:
                        gcount.append(g)
            count = Counter(gcount)
            gsel = count.most_common(1)[0][0]
                
            # Process other colors
            for i in range(3):            
                # In same group?
                if block[i] not in groups[gsel]:            
                    # Find nearest color in group
                    delta = []
                    vec1 = rgb[block[i]]
                    for k in range(len(groups[gsel])):
                        index = groups[gsel][k]
                        if index in enforce:
                            delta.append( 999999 )
                        else:
                            vec2 = rgb[index]
                            delta.append( sqrt( (vec1[0]-vec2[0])**2 + (vec1[1]-vec2[1])**2 + (vec1[2]-vec2[2])**2 ) )
                    swap = delta.index(min(delta))
                    block[i] = groups[gsel][swap]
            
            # Assign inversion bit
            byte1 = (2*y+0)*40+x+1
            byte2 = (2*y+1)*40+x+1
            if gsel == 0:
                value1 = 64
                value2 = 64
            if gsel == 1:
                value1 = 192
                value2 = 64
            if gsel == 2:
                value1 = 64
                value2 = 192
            if gsel == 3:
                value1 = 192
                value2 = 192
                
            # Assign bits
            for i in range(3):
                index = block[i] % 5
                if index == 1:
                    value1 += 32 / 4**i
                    value2 += 16 / 4**i
                if index == 2:
                    value1 += (32+16) / 4**i
                if index == 3:
                    value2 += (32+16) / 4**i
                if index == 4:
                    value1 += (32+16) / 4**i
                    value2 += (32+16) / 4**i
                    
            # Save data
            data[byte1] = chr(value1)
            data[byte2] = chr(value2)
            
                
    #################
    # Write DAT file
    f2 = io.open(rawFile, 'wb')	
    f2.write(''.join(data))
    f2.close()
