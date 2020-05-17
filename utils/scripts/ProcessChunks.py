#
# Process Chunk definition file
#
# Usage: Input, Output, Coords
#
#   Input:    'file.png' input image (with correct size and palette for platform)
#	Output:	  'file.dat' output containing the chunk data
# 	Coords:   [X, Y, W, H] on PNG file
#
# 	Coordinates restrictions:
#   	Apple:  X/W must be multiples of 7 (e.g. 0,7,14,21...) |              No restrictions
#   	Atari:  X/W must be multiples of 4 (e.g. 0,4,8,12... ) |              No restrictions
#   	C64:    X/W must be multiples of 4 (e.g. 0,4,8,12... ) |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
#   	Lynx:   X/W must be multiples of 2 (e.g. 0,2,4,6... )  |              No restrictions
#   	Oric:   X/W must be multiples of 3 (e.g. 0,3,6,9...)   |  Y/H must be multiples of 2  (e.g. 0,2,4,6...)  
#
#
# Output file contains 4 byte header for chunk position and dimension (X, Y, W, H) 
# followed by platform specific graphic data
#
 
import io, os, struct, sys
from PIL import Image
from collections import Counter
from math import sqrt

############################################
def ExportApple(filename, coords, pixdata):

    # Process in blocks of 7 pixels > 4 bytes
    imgWidth  = (coords[2]-coords[0])
    imgHeight = (coords[3]-coords[1]) 
    blocks = []
    for i in range(len(pixdata)/7):
        block = [0,0,0,0]
        for pixel in range(7):
            color = pixdata[i*7+pixel]
            if pixel == 0:
                block[0] |= (color & 15)
            if pixel == 1:
                block[0] |= (color &  7) << 4
                block[1] |= (color &  8) >> 3
            if pixel == 2:
                block[1] |= (color & 15) << 1
            if pixel == 3:
                block[1] |= (color &  3) << 5
                block[2] |= (color & 12) >> 2
            if pixel == 4:
                block[2] |= (color & 15) << 2
            if pixel == 5:
                block[2] |= (color &  1) << 6
                block[3] |= (color & 14) >> 1
            if pixel == 6:
                block[3] |= (color & 15) << 3            
        blocks.append(block)

    # Write to file line-by-line
    blocksPerLine = (coords[2]-coords[0])/7
    output = io.open(filename, 'wb')
    output.write(chr(coords[0]))
    output.write(chr(coords[1]))
    output.write(chr(imgWidth))
    output.write(chr(imgHeight))
    i = 0
    while i < len(blocks):
        # Write MAIN data
        for j in range(blocksPerLine):
            output.write(chr(blocks[i+j][0]))           
            output.write(chr(blocks[i+j][2]))           
        # Write AUX data
        for j in range(blocksPerLine):
            output.write(chr(blocks[i+j][1]))           
            output.write(chr(blocks[i+j][3]))           
        i += blocksPerLine
    output.close() 


############################################
def ExportAtari(filename, coords, pixdata):

    # Process to double frame buffer
    imgWidth  = (coords[2]-coords[0])
    imgHeight = (coords[3]-coords[1]) 
    imgSize   = (imgHeight*imgWidth/4)
    buoutput = [0] * imgSize
    buf2 = [0] * imgSize
    for y in range(imgHeight):
        for x in range(imgWidth):
            # Convert PNG to INP index 
            color = pixdata[y*imgWidth+x]
            shift = 6 - 2*(x%4)
            if (x+y)%2:   # Alternate to create checker board
                col2 = color%4 << shift
                col1 = color/4 << shift
            else:
                col1 = color%4 << shift
                col2 = color/4 << shift
                
            # Assign bits in both buffers
            offset = y*(imgWidth/4)+x/4
            mask = 255 - (3<<shift)            
            buoutput[offset] = (buoutput[offset] & mask) | col1
            buf2[offset] = (buf2[offset] & mask) | col2  
            
    #################
    # Convert to CHR
    for i in range(imgSize):
        buoutput[i] = chr(buoutput[i])
        buf2[i] = chr(buf2[i])
            
    ########################
    # Write output INP file
    output = io.open(filename, 'wb')
    output.write(chr(coords[0]))
    output.write(chr(coords[1]))
    output.write(chr(imgWidth))
    output.write(chr(imgHeight))
    output.write(''.join(buoutput))
    output.write(''.join(buf2))
    output.close()
        
        
############################################
def ExportC64(filename, coords, pixdata, paldata):

    # Extract RGB data
    rgb = []
    for i in range(16):
        rgb.append(paldata[i*3:i*3+3])
        
    # Process in blocks of 4 x 8 pixels
    imgWidth  = (coords[2]-coords[0])
    imgHeight = (coords[3]-coords[1])
    bmp = [0] * (imgWidth*imgHeight/4)
    scr = [0] * (imgWidth*imgHeight/32)
    col = [0] * (imgWidth*imgHeight/32)
    for y in range(0, imgHeight, 8):
        for x in range(0, imgWidth, 4):
            # Find most abundant colors in block
            block = []
            for i in range(8):
                for j in range(4):
                 block.append(pixdata[(y+i)*imgWidth+(x+j)])             
            
            # Find 3 most abundant colors (except black)
            abundant = (value for value in block if value != 0)
            count = Counter(abundant)        
            choice = count.most_common(3)
            
            # Set these 3 colors in SCREENRAM and COLORRAM
            translate = { 0:0 }
            for i in range(1,len(choice)+1):
                color = choice[i-1][0]                        
                translate[color] = i
                offset = (y/8)*(imgWidth/4)+(x/4);
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
                    offset = (imgWidth/4)*((y+i)&248)+((y+i)&7)+((2*(x+j))&504);
                    bmp[offset] |= index << shift

    #################
    # Convert to CHR
    for i in range(len(bmp)):
        bmp[i] = chr(bmp[i])
    for i in range(len(scr)):    
        scr[i] = chr(scr[i])
    for i in range(len(col)):    
        col[i] = chr(col[i])
            
    ########################
    # Write output INP file
    output = io.open(filename, 'wb')
    output.write(chr(coords[0]))
    output.write(chr(coords[1]))
    output.write(chr(imgWidth))
    output.write(chr(imgHeight))
    output.write(''.join(bmp))
    output.write(''.join(scr))
    output.write(''.join(col))
    output.close()    
    
    
############################################
def ExportLynx(filename, coords, pixdata):

    # Process to double frame buffer
    imgWidth  = (coords[2]-coords[0])
    imgHeight = (coords[3]-coords[1]) 
    imgSize   = (imgHeight*imgWidth/2)
    buffer = [0] * imgSize
    offset = 0
    for y in range(imgHeight):
        for x in range(0, imgWidth, 2):
            buffer[offset] = pixdata[y*imgWidth+x]*16 + (pixdata[y*imgWidth+x+1])
            offset += 1
            
    # Convert to CHR
    for i in range(imgSize):
        buffer[i] = chr(buffer[i])
            
    # Write output file
    output = io.open(filename, 'wb')
    output.write(chr(coords[0]))
    output.write(chr(coords[1]))
    output.write(chr(imgWidth))
    output.write(chr(imgHeight))
    output.write(''.join(buffer))
    output.close()    
    
    
############################################
def ExportOric(filename, coords, pixdata, paldata):    

    # Process in blocks of 3 x 1 pixels > encoded to 6 x 2 pixels (2 bytes)
    imgWidth  = (coords[2]-coords[0])
    imgHeight = (coords[3]-coords[1]) 
    imgSize   = (2*imgHeight*imgWidth/3)    
    buffer    = [0] * imgSize
    noRemap   = [19]

    # Force BW over WB order for grey colour
    pixdata = [pix if pix != 5 else 10 for pix in pixdata]

    # Get RGB components of palette
    rgb = []
    for i in range(20):
        rgb.append(paldata[i*3:i*3+3])

    # Inversion groups
    groups = [ [0,1,2,3,4],         # line 1 and 2 non-inverted
               [5,6,7,8,9],         # line 1 inverted
               [10,11,12,13,14],    # line 2 inverted
               [15,16,17,18,19] ]   # line 1 and 2 inverted
               
    # Convert to AIC format
    for y in range(imgHeight):        
        # Process in blocks of 3x1 pixels (encoded as 6x2)
        for x in range(imgWidth/3):
            # Find most frequent colour in block
            block = pixdata[y*imgWidth+x*3:y*imgWidth+x*3+3]
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
                        if index in noRemap:
                            delta.append( 999999 )
                        else:
                            vec2 = rgb[index]
                            delta.append( sqrt( (vec1[0]-vec2[0])**2 + (vec1[1]-vec2[1])**2 + (vec1[2]-vec2[2])**2 ) )
                    swap = delta.index(min(delta))
                    block[i] = groups[gsel][swap]
            
            # Assign inversion bit
            byte1 = (2*y+0)*(imgWidth/3)+x
            byte2 = (2*y+1)*(imgWidth/3)+x
            if gsel == 0:
                buffer[byte1] = 64
                buffer[byte2] = 64
            if gsel == 1:
                buffer[byte1] = 192
                buffer[byte2] = 64
            if gsel == 2:
                buffer[byte1] = 64
                buffer[byte2] = 192
            if gsel == 3:
                buffer[byte1] = 192
                buffer[byte2] = 192
                
            # Assign bits
            for i in range(3):
                index = block[i] % 5
                if index == 1:
                    buffer[byte1] += 32 / 4**i
                    buffer[byte2] += 16 / 4**i
                if index == 2:
                    buffer[byte1] += (32+16) / 4**i
                if index == 3:
                    buffer[byte2] += (32+16) / 4**i
                if index == 4:
                    buffer[byte1] += (32+16) / 4**i
                    buffer[byte2] += (32+16) / 4**i
            
    # Convert to char
    for i in range(len(buffer)):
        buffer[i] = chr(buffer[i])
                
    #################
    # Write DAT file
    output = io.open(filename, 'wb')
    output.write(chr(coords[0]))
    output.write(chr(coords[1]))
    output.write(chr(imgWidth))
    output.write(chr(imgHeight))
    output.write(''.join(buffer))
    output.close()
    
    
############################################    
# Process chunks definition file
platform = sys.argv[1]
chunkDefs = sys.argv[2]
outfolder = sys.argv[3]

script = open(chunkDefs, "r")
lines = script.readlines() 
script.close()

listing = open(outfolder+'chunks.lst', "w")

for line in lines:
    # Skip comments
    if line[0] != '\'':
        continue
        
    # Parse chunk filename
    offset1 = 1
    offset2 = 1
    while line[offset2] != '\'':
        offset2 += 1
    infile = os.path.dirname(chunkDefs) + '/' + line[offset1:offset2]

    # Skip to next '
    offset2 += 1
    while line[offset2] != '\'':
        offset2 += 1
    offset2 += 1

    # Parse output filename
    offset1 = offset2
    while line[offset2] != '\'':
        offset2 += 1
    outfile = outfolder + '/' + line[offset1:offset2]
    outfile = outfile.replace('//','/')

    # Skip to next [
    while line[offset2] != '[':
        offset2 += 1
    offset2 += 1    
    
    # Parse coordinates
    offset1 = offset2
    while line[offset2] != ']':
        offset2 += 1
    coords = [int(s) for s in line[offset1:offset2].split(',')]
    coords[2] = coords[0]+coords[2]
    coords[3] = coords[1]+coords[3]
    
    # Process chunk    
    image = Image.open(infile)
    chunkdata = image.crop(coords)
    pixdata = list(chunkdata.getdata())
    paldata = chunkdata.getpalette()
    
    # Export to required format    
    if platform == 'apple':
        ExportApple(outfile, coords, pixdata)
    if platform == 'atari':
        ExportAtari(outfile, coords, pixdata)
    if platform == 'c64':
        ExportC64(outfile, coords, pixdata, paldata)
    if platform == 'lynx':
        ExportLynx(outfile, coords, pixdata)
    if platform == 'oric':
        ExportOric(outfile, coords, pixdata, paldata)

    # Print some info
    coords[2] -= coords[0]
    coords[3] -= coords[1]
    print 'Generated ', outfile, coords

    # Add file to list
    listing.write(outfile+'\n')
    
listing.close()
