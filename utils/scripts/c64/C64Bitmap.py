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
from collections import Counter
from math import sqrt
import io,os, sys
import subprocess

compress = sys.argv[1]
input = sys.argv[2]
output = sys.argv[3]

try:
    #################################
    # Read source bitmap and palette
    img1 = Image.open(input)
    pixdata = list(img1.getdata())
    rgb = []
    dump = img1.getpalette()
    for i in range(16):
        rgb.append(dump[i*3:i*3+3])

    ################################
    # Convert pixel data to buffers 
    bmp = [0] * 8000
    scr = [0] * 1000
    col = [0] * 1000
    for y in range(0, 200, 8):
        for x in range(0, 160, 4):
            # List colors in block
            block = []
            for i in range(8):
                for j in range(4):
                 block.append(pixdata[(y+i)*160+(x+j)])             
            
            # Find 3 most abundant colors (except black)
            abundant = (value for value in block if value != 0)
            count = Counter(abundant)        
            choice = count.most_common(3)
            
            # Set these 3 colors in SCREENRAM and COLORRAM
            translate = { 0:0 }
            for i in range(1,len(choice)+1):
                color = choice[i-1][0]                        
                translate[color] = i
                offset = (y/8)*40+(x/4);
                if i == 1:
                    # SCREENRAM (Upper bits)
                    scr[offset] |= color << 4
                if i == 2:
                    # SCREENRAM (Lower bits)
                    scr[offset] |= color
                if i == 3:
                    # COLORRAM (Lower bits)
                    col[offset] = color
                  
            # Assign colours not in palette to nearest colour
            for i in range(8):
                for j in range(4):
                    if block[i*4+j] not in translate.keys():
                        delta = []
                        vec1 = rgb[block[i*4+j]]
                        for k in range(1,4):
                            vec2 = rgb[translate.keys()[k]]
                            delta.append( sqrt( (vec1[0]-vec2[0])**2 + (vec1[1]-vec2[1])**2 + (vec1[2]-vec2[2])**2 ) )
                        swap = delta.index(min(delta))+1
                        block[i*4+j] = translate.keys()[swap]
                                              
                  
            # Set colour indices (0-3)
            for i in range(8):
                for j in range(4):
                    color = block[i*4+j]
                    index = translate[color]
                    shift = 2*(3-(x+j)%4);
                    offset = 40*((y+i)&248)+((y+i)&7)+((2*(x+j))&504);
                    bmp[offset] |= index << shift

    #################
    # Convert to CHR
    for i in range(8000):
        bmp[i] = chr(bmp[i])
    for i in range(1000):    
        scr[i] = chr(scr[i])
    for i in range(1000):    
        col[i] = chr(col[i])
            
    ########################
    # Write output IMG file
    if compress == 'crunch':
        # Write raw data files
        f = io.open(output.replace('.img','.raw1'), 'wb')
        f.write(''.join([chr(0x00), chr(0xE0)]))     # Load Address: $E000  
        f.write(''.join(scr))
        f.close()
        f = io.open(output.replace('.img','.raw2'), 'wb')
        f.write(''.join([chr(0x00), chr(0xE0)]))     # Load Address: $E000  
        f.write(''.join(bmp))
        f.close() 
        
        # Crunch data and read back
        subprocess.call(["utils/scripts/exomizer-3.1.0.exe", "mem", "-lnone", output.replace('.img','.raw1'), "-B", "-o", output.replace('.img','.sfx1')])
        subprocess.call(["utils/scripts/exomizer-3.1.0.exe", "mem", "-lnone", output.replace('.img','.raw2'), "-B", "-o", output.replace('.img','.sfx2')])

        # Read back compressed data
        f = io.open(output.replace('.img','.sfx1'), 'rb')
        sfx1 = f.read(); 
        f.close()
        f = io.open(output.replace('.img','.sfx2'), 'rb')
        sfx2 = f.read(); 
        f.close()
        
        # Consolidate to single file
        f = io.open(output, 'wb')	
        f.write(''.join([chr(len(sfx1)%256), chr(len(sfx1)/256)])); f.write(sfx1)
        f.write(''.join([chr(len(sfx2)%256), chr(len(sfx2)/256)])); f.write(sfx2)
        f.write(''.join(col))
        f.close()
        
        # Clean-up
        os.remove(output.replace('.img','.raw1'))
        os.remove(output.replace('.img','.raw2'))
        os.remove(output.replace('.img','.sfx1'))
        os.remove(output.replace('.img','.sfx2'))
    else:
        f = io.open(output, 'wb')	
        f.write(''.join(bmp))
        f.write(''.join(scr))
        f.write(''.join(col))
        f.close()

except:
    print "Error: cannot convert " + input + "... (is it a 160x200 PNG file with 16 color palette?)"
