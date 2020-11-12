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
import io, sys

mode = sys.argv[1]
input = sys.argv[2]
output = sys.argv[3]

try:
    ###################
    # Read source file
    img1 = Image.open(input)
    rawdata = list(img1.getdata())
    colors = max(rawdata)

    # Create sprite file
    f1 = io.open(output, 'wb')

    # Write load address ($A800)
    f1.write(chr(0x00));
    f1.write(chr(0xa8));

    # Process in blocks of 7 pixels > 4 bytes
    for i in range(len(rawdata)/7):
        pixels = rawdata[i*7:(i+1)*7]
        if mode == 'single':   
            # Reduce palette?
            if colors > 6:
                pixels = RemapDHR2SHR(pixels)
            res = AssignColorGroup(pixels)    
            pixels = res[0]; block = res[1]
            for j in range(7):
                SetSHRColor(block, j, pixels[j])
            for j in [0,1]:
                f1.write(chr(block[j]))  
              
        else:
            block = [0,0,0,0]
            for j in range(7):
                SetDHRColor(block, j, pixels[j])
            for j in [0,2,1,3]:
                f1.write(chr(block[j]))

    # Process in blocks of 7 pixels > 4 bytes (Shifted by 2 pixels)
    for i in range(len(rawdata)/7):
        pixels = rawdata[i*7:(i+1)*7]
        if mode == 'single':        
            # Reduce palette?
            if colors > 6:
                pixels = RemapDHR2SHR(pixels)
            res = AssignColorGroup(pixels)    
            pixels = res[0]; block = res[1]
            for j in range(7):
                SetSHRColor(block, (j+2)%7, pixels[j])
            for j in [0,1]:
                f1.write(chr(block[j]))  
              
        else:
            block = [0,0,0,0]
            for j in range(7):
                SetDHRColor(block, (j+2)%7, pixels[j])
            for j in [0,2,1,3]:
                f1.write(chr(block[j]))            
            
    # Process offset blocks of 7 pixels > 4 bytes (Shifted by 4 pixels)
    for i in range(len(rawdata)/7):
        pixels = rawdata[i*7:(i+1)*7]
        if mode == 'single':        
            # Reduce palette?
            if colors > 6:
                pixels = RemapDHR2SHR(pixels)
            res = AssignColorGroup(pixels)    
            pixels = res[0]; block = res[1]
            for j in range(7):
                SetSHRColor(block, (j+4)%7, pixels[j])
            for j in [1,0]:
                f1.write(chr(block[j]))  
              
        else:
            block = [0,0,0,0]
            for j in range(7):
                SetDHRColor(block, (j+4)%7, pixels[j])
            for j in [2,0,3,1]:
                f1.write(chr(block[j]))            

    # Process offset blocks of 7 pixels > 4 bytes (Shifted by 5 pixels)
    for i in range(len(rawdata)/7):
        pixels = rawdata[i*7:(i+1)*7]
        if mode == 'single':        
            # Reduce palette?
            if colors > 6:
                pixels = RemapDHR2SHR(pixels)
            res = AssignColorGroup(pixels)    
            pixels = res[0]; block = res[1]
            for j in range(7):
                SetSHRColor(block, (j+5)%7, pixels[j])
            for j in [1,0]:
                f1.write(chr(block[j]))  
              
        else:
            block = [0,0,0,0]
            for j in range(7):
                SetDHRColor(block, (j+5)%7, pixels[j])
            for j in [2,0,3,1]:
                f1.write(chr(block[j]))            

    f1.close()

except:
    print "Error: cannot convert " + input + "... (is it a 14x? PNG file with 6 or 16 color palette?)"
