"""
 * Copyright (c) 2022 Anthony Beaucamp.
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

import io, os, struct, subprocess, sys

# Retrieve command params
xexFile = sys.argv[1]
rawFile = xexFile[0:-4] + ".raw"
sfxFile = rawFile+".zx0"
options = sys.argv[2]

# Sub-processes
if "nt" == os.name:
    zx0 = "utils\scripts\zx0.exe "
else:
    zx0 = "wine utils/scripts/zx0.exe "
    
# Read input file
fin = io.open(xexFile, 'rb')
data = fin.read()
fin.close()

# Create output file and include decompressor
fout = io.open(xexFile, 'wb')

# Included decompressor program
if 'incDec' in options:
    f = io.open('utils/scripts/atari/cex_zx0.obx', 'rb')
    dec = f.read()
    f.close()   
    fout.write(dec)

# 2 bytes atari header (i.e $ff $ff)
fout.write(''.join([chr(0xFF),chr(0xFF)])) 

# Write blocks as raw files and compress them
i = 2
blocks = []
while (i<len(data)):
    try:
        # Read header
        start = struct.unpack('H', data[i+0:i+2])[0]
        end   = struct.unpack('H', data[i+2:i+4])[0]
        size  = end-start+1
        blocks.append([start,end])
        
        # Write raw data
        f = io.open(rawFile, 'wb')
        f.write(data[i+4:i+4+size])
        f.close()
        
        # Compress raw data
        subprocess.call(zx0 + rawFile, shell=True)
        f = io.open(sfxFile, 'rb')
        sfx = f.read()
        f.close()        
        
        # Append to compressed file
        fout.write(data[i+0:i+2])   # Load address
        fout.write(''.join([chr(0x00),chr(0x00),chr(0x02)])) # Compressor header
        fout.write(sfx)
        fout.write(data[i+4+size:i+4+size+6])   # Run code
        
        #Clean-up
        os.remove(rawFile)
        os.remove(sfxFile)
        
        # Move to next block
        i = i+4+size+6
    except:
        break
        
fout.close()
