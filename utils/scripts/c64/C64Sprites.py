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

#################################
# Read source bitmap and palette
img1 = Image.open(input)
rawdata = list(img1.getdata())
colors = max(rawdata)
print "Sprite sheet size: {%i,%i}; Number of colors: %i" % (img1.size[0], img1.size[1], colors)

################################
# Rearrange into 12 * 21 blocks
griddata = []
for row in range(0, img1.size[1], 21):
    for col in range(0, img1.size[0], 12):
        for j in range(0, 21):        
            for i in range(0, 12):
                griddata.append(rawdata[(row+j)*img1.size[0]+col+i])

#############################
# Split into 4 colors layers
layers = 0
layerdata = []
while (layers+3) <= max(griddata):
    pixdata = []
    
    # Transcribe unique colors to different layers
    for i in range(len(griddata)):
        # Transparent or shared color?
        if (griddata[i] in [0,1]): 
            color = griddata[i]
        elif (griddata[i] == 2): 
            color = 3        
        # Is it the unique color of layer?
        elif (griddata[i] == layers+3): 
            color = 2
        # Otherwise use transparent
        else:
            color = 0
        pixdata.append(color)
        
    # Add to layer data
    layerdata.append(pixdata)
    layers += 1

####################################
# Convert 4bit pixel data to buffers 
block = 12*21
frames = len(griddata)/block
layersize = frames*64
sprdata = [chr(0)] * (layers*layersize)
for layer in range(layers):
    for frame in range(frames):
        for i in range(0, block, 4):
            sprdata[layer*layersize+frame*64+i/4] = \
                        chr((layerdata[layer][frame*block+i+3]<<0) + 
                            (layerdata[layer][frame*block+i+2]<<2) + 
                            (layerdata[layer][frame*block+i+1]<<4) + 
                            (layerdata[layer][frame*block+i]  <<6))

###########################
# Write output binary file
f2 = io.open(output, 'wb')	
f2.write(''.join([chr(0),chr(0xc4)]))
f2.write(''.join(sprdata))
f2.close()
