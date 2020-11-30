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
 
import io, os, sys, csv, subprocess
from PIL import Image

charFile = sys.argv[1]
output   = sys.argv[2]
dither   = sys.argv[3]

pictOricIn = output.replace('.dat', '-in.png')
pictOricOut = output.replace('.dat', '-out.dat')
flagFile = charFile.replace('-oric.png', '.csv')

#############################
# Read char and font bitmaps
charImg = Image.open(charFile)
charRaw = list(charImg.getdata())
print "Charset size: {%i,%i}" % (charImg.size[0], charImg.size[1])

############################
# Prepare image for PictOric
charImg = charImg.convert("RGB")
result = Image.new(charImg.mode, (240, 200), (0,0,0))
result.paste(charImg, (6, 0))
result.save(pictOricIn)

###################
# Call PictOric
subprocess.call(["luajit.exe", "PictOric.lua", dither, pictOricIn, pictOricOut])

#####################
# Trim PictOric File
f1 = io.open(pictOricOut, 'rb')
data = f1.read()
f1.close()
charData = []
for pix in range(8):
    for row in range(4):
        for col in range(32):
            charData.append(data[row*8*40+pix*40+col+1])

#######################
# Read character flags
flagData = [chr(0)] * 128
with open(flagFile) as csvfile:
    i = 0
    rows = csv.reader(csvfile, delimiter=',')
    for row in rows:
        for elt in row:
            flagData[i] = chr(int(elt))
            i += 1
            
############################
# Write output binary file
f2 = io.open(output, 'wb')
f2.write(''.join(charData))
f2.write(''.join(flagData))
f2.close()
