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
height = int(sys.argv[4])

try:
    ###################
    # Read source file
    img1 = Image.open(input)
    rawdata = list(img1.getdata())
    colors = max(rawdata)
    main = []; aux = []
    
    def AddOffsetBlock(offset):
        for row in range(0, img1.size[1], height):
            for col in range(0, img1.size[0], 7):
                for k in range(0, height):  
                    pixels = rawdata[(row+k)*img1.size[0]+col:(row+k)*img1.size[0]+col+7]
                    if mode == 'single':   
                        # Reduce palette?
                        if colors > 6:
                            pixels = RemapDHR2SHR(pixels)
                        res = AssignColorGroup(pixels)    
                        pixels = res[0]; block = res[1]
                        for j in range(7):
                            SetSHRColor(block, (j+offset)%7, pixels[j])
                        # Save blocks
                        if offset < 3:
                            order = [0,1]
                        else:
                            order = [1,0]
                        for j in order:
                            main.append(chr(block[j]))  
                          
                    else:
                        block = [0,0,0,0]
                        for j in range(7):
                            SetDHRColor(block, (j+offset)%7, pixels[j])
                        # Save blocks
                        if offset < 3:
                            orderAux = [0,2]
                            orderMain = [1,3]                        
                        else:
                            orderAux = [2,0]
                            orderMain = [3,1]                        
                        for j in orderAux:
                            aux.append(chr(block[j]))
                        for j in orderMain:
                            main.append(chr(block[j]))

    #############################################
    # Convert to offseted blocks (7 pixels wide)
    AddOffsetBlock(0)
    AddOffsetBlock(2)
    AddOffsetBlock(4)
    AddOffsetBlock(5)

    # Output sprite data
    f1 = io.open(output, 'wb')
    f1.write(chr(len(main)%256))
    f1.write(chr(len(main)/256))
    if mode == 'double':
        f1.write(''.join(aux))    
    f1.write(''.join(main))
    f1.close()

except:
    print "Error: cannot convert " + input + "... (is it a 140x192 PNG file with 6 or 16 color palette?)"
