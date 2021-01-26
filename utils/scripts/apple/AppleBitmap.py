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

from AppleHires import *
from PIL import Image
import io, os, sys
import subprocess

resolution = sys.argv[1]
compress = sys.argv[2]
input = sys.argv[3]
output = sys.argv[4]

try:
    ###################
    # Read source file
    img1 = Image.open(input)
    rawdata = list(img1.getdata())
    colors = max(rawdata)

    # Prepare data lists
    main = [chr(0)]*8192
    if resolution == 'double':
        aux = [chr(0)]*8192

    # Convert bitmap
    for i in range(192):
        for j in range(0,140,7):
            pixels = rawdata[i*140+j:i*140+j+7]
            if resolution == 'single':
                # Reduce palette?
                if colors > 6:
                    pixels = RemapDHR2SHR(pixels)
                res = AssignColorGroup(pixels)    
                pixels = res[0]; block = res[1]
                
                # Process in blocks of 7 pixels, to be packed into 2 bytes
                for k in range(7):
                    SetSHRColor(block, k, pixels[k])
                main[HiresLines[i]+(j*2)/7+0] = (chr(block[0]))
                main[HiresLines[i]+(j*2)/7+1] = (chr(block[1]))
                
            else:
                # Process in blocks of 7 pixels, to be packed into 4 bytes
                block = [0,0,0,0]
                for k in range(7):
                    SetDHRColor(block, k, pixels[k])
                aux [HiresLines[i]+(j*2)/7+0] = (chr(block[0]))
                main[HiresLines[i]+(j*2)/7+0] = (chr(block[1]))
                aux [HiresLines[i]+(j*2)/7+1] = (chr(block[2]))
                main[HiresLines[i]+(j*2)/7+1] = (chr(block[3]))

    # Write to file
    if compress == 'crunch':
        # Write raw data (target address $2000-$4000)
        if resolution == 'double':
            f = io.open(output.replace('.img','.aux'), 'wb')
            f.write(''.join([chr(0x00), chr(0x20)])); 
            f.write(''.join(aux)); 
            f.close()
        f = io.open(output.replace('.img','.main'), 'wb')
        f.write(''.join([chr(0x00), chr(0x20)])); 
        f.write(''.join(main)); 
        f.close()
        
        # Compress data
        if resolution == 'double':
            subprocess.call(["utils/scripts/exomizer-3.1.0.exe", "mem", "-lnone", output.replace('.img','.aux'), "-o", output.replace('.img','.aux.sfx')])
        subprocess.call(["utils/scripts/exomizer-3.1.0.exe", "mem", "-lnone", output.replace('.img','.main'), "-o", output.replace('.img','.main.sfx')])
        
        # Read back compressed data
        if resolution == 'double':
            f = io.open(output.replace('.img','.aux.sfx'), 'rb')
            aux = f.read(); 
            f.close()
        f = io.open(output.replace('.img','.main.sfx'), 'rb')
        main = f.read(); 
        f.close()
        
        # Clean-up
        if resolution == 'double':
            os.remove(output.replace('.img','.aux'))
            os.remove(output.replace('.img','.aux.sfx'))
        os.remove(output.replace('.img','.main'))
        os.remove(output.replace('.img','.main.sfx'))

        # Consolidate to single file
        f = io.open(output, 'wb')
        if resolution == 'double':
            f.write(''.join([chr(len(aux)%256), chr(len(aux)/256)])); f.write(aux);
        f.write(''.join([chr(len(main)%256), chr(len(main)/256)])); f.write(main);
        f.close()
    
    else:
        # Just write raw data
        f = io.open(output, 'wb')
        if resolution == 'double':
            f.write(''.join(aux))
        f.write(''.join(main))
        f.close()

except:
    print "Error: cannot convert " + input + "... (is it a 140x192 PNG file with 6 or 16 color palette?)"
