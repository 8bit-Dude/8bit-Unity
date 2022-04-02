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
import io, os, sys
import subprocess

# Retrieve command params
resolution = sys.argv[1]
compress = sys.argv[2]
input = sys.argv[3]
output = sys.argv[4]

# Sub-processes
if "nt" == os.name:
    ex31 = "utils/scripts/exomizer-3.1.0.exe"
else:
    ex31 = "wine utils/scripts/exomizer-3.1.0.exe"

try:
    ##############################
    # Interleaved-player colors
    palette = [ chr(0x00), chr(0x24), chr(0x86), chr(0xd8) ]

    #####################
    # Read source bitmap
    img1 = Image.open(input)
    pixdata = list(img1.getdata())

    ################################
    # Convert pixel data to buffers 
    buf1 = [0] * 8000
    buf2 = [0] * 8000
    for y in range(200):
        for x in range(160):
            # Convert PNG to INP index 
            color = pixdata[y*160+x]            
            offset = y*40+x/4
            shift = 6 - 2*(x%4)
            mask = 255 - (3<<shift)            
            if (x+y)%2:   # Alternate to create checker board
                col2 = color%4 << shift
                col1 = color/4 << shift
            else:
                col1 = color%4 << shift
                col2 = color/4 << shift
            buf1[offset] = (buf1[offset] & mask) | col1
            buf2[offset] = (buf2[offset] & mask) | col2  
    
    #################
    # Convert to CHR
    for i in range(8000):
        buf1[i] = chr(buf1[i])
        buf2[i] = chr(buf2[i])
            
    ########################
    # Write output INP file
    if compress == 'crunch':
        # Write raw data files
        f = io.open(output.replace('.img','.raw1'), 'wb')
        f.write(''.join([chr(0x10), chr(0xa0)]))     # Load Address: $a010         
        f.write(''.join(buf1))
        f.close()    
        if resolution == 'double':
            f = io.open(output.replace('.img','.raw2'), 'wb')	
            f.write(''.join([chr(0x10), chr(0x70)]))     # Load Address: $7010  
            f.write(''.join(buf2))
            f.close()    
        
        # Crunch data and read back
        subprocess.call([ex31, "mem", "-lnone", output.replace('.img','.raw1'), "-B", "-o", output.replace('.img','.sfx1')])
        if resolution == 'double':
            subprocess.call([ex31, "mem", "-lnone", output.replace('.img','.raw2'), "-B", "-o", output.replace('.img','.sfx2')])
        
        # Read back compressed data
        f = io.open(output.replace('.img','.sfx1'), 'rb')
        sfx1 = f.read(); 
        f.close()
        if resolution == 'double':
            f = io.open(output.replace('.img','.sfx2'), 'rb')
            sfx2 = f.read(); 
            f.close()
        
        # Consolidate to single file
        f = io.open(output, 'wb')	
        f.write(''.join(palette))
        f.write(''.join([chr(len(sfx1)%256), chr(len(sfx1)/256)])); f.write(sfx1)
        if resolution == 'double':
            f.write(''.join([chr(len(sfx2)%256), chr(len(sfx2)/256)])); f.write(sfx2)
        f.close()    
        
        # Clean-up
        os.remove(output.replace('.img','.raw1'))
        os.remove(output.replace('.img','.sfx1'))
        if resolution == 'double':
            os.remove(output.replace('.img','.raw2'))        
            os.remove(output.replace('.img','.sfx2'))        
        
    else:
        # Just write raw data
        f = io.open(output, 'wb')	
        f.write(''.join(palette))
        f.write(''.join(buf1))
        if resolution == 'double':
            f.write(''.join(buf2))
        f.close()    

except:
    print "Error: cannot convert " + input + "... (is it a 160x200 PNG file with 16 color palette?)"
