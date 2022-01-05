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

def FileBase(filepath, suffix):
    # Return asset file base
    name = os.path.basename(filepath).lower().replace(suffix, '')
    name = name.split("-")
    return name[0]
    
#############################################
# Set color of specific pixel (Single Hires)
# 0=BLACK, 1=VIOLET/BLUE, 2=GREEN/ORANGE, 3=WHITE
def SetSHRColor(block, pixel, color):
    
    if pixel == 0:
        block[0] |= (color & 3)
    if pixel == 1:
        block[0] |= (color & 3) << 2
    if pixel == 2:
        block[0] |= (color & 3) << 4
    if pixel == 3:
        block[0] |= (color & 1) << 6
        block[1] |= (color & 2) >> 1
    if pixel == 4:
        block[1] |= (color & 3) << 1
    if pixel == 5:
        block[1] |= (color & 3) << 3
    if pixel == 6:
        block[1] |= (color & 3) << 5

#############################################
# Set color of specific pixel (Double Hires)
def SetDHRColor(block, pixel, color):

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
        
#############################################
def RemapDHR2SHR(pixels):
    remap = [x+100 for x in pixels]                
    remap = [0 if x==100 else x for x in remap] # BLACK
    remap = [1 if x==101 else x for x in remap] # DBLUE -> PURPLE
    remap = [2 if x==102 else x for x in remap] # DGREEN -> GREEN
    remap = [3 if x==103 else x for x in remap] # BLUE
    remap = [4 if x==104 else x for x in remap] # BROWN -> ORANGE
    remap = [0 if x==105 else x for x in remap] # DGREY -> BLACK
    remap = [2 if x==106 else x for x in remap] # GREEN
    remap = [2 if x==107 else x for x in remap] # LGREEN -> GREEN
    remap = [0 if x==108 else x for x in remap] # DPURPLE -> BLACK
    remap = [1 if x==109 else x for x in remap] # PURPLE                
    remap = [3 if x==110 else x for x in remap] # LGREY -> BLUE                
    remap = [3 if x==111 else x for x in remap] # LBLUE -> BLUE
    remap = [4 if x==112 else x for x in remap] # ORANGE
    remap = [5 if x==113 else x for x in remap] # PINK -> WHITE
    remap = [2 if x==114 else x for x in remap] # YELLOW -> GREEN
    remap = [5 if x==115 else x for x in remap] # WHITE    
    return remap
    
#############################################
def AssignColorGroup(pixels): 
    # Find group (Purple/Green) or (Blue/Orange) with most occurence
    group1 = pixels.count(1)+pixels.count(2)
    group2 = pixels.count(3)+pixels.count(4)
    if group1 > group2:
        block = [0,0]
    else:
        block = [128,128] 

    # Re-assign all colors according to that group
    pixels = [1 if x==3 else x for x in pixels]
    pixels = [2 if x==4 else x for x in pixels]
    pixels = [3 if x==5 else x for x in pixels]
    return pixels, block

############################################
def PackApple(coords, pixdata, mode):

    # Process in blocks of 7 pixels > 4 bytes
    imgWidth  = coords[2]
    imgHeight = coords[3]
    colors = max(pixdata)    
    blocks = []
    for i in range(len(pixdata)/7):
        pixels = pixdata[i*7:(i+1)*7]
        if mode == 'single':
            # Reduce palette?
            if colors > 6:
                pixels = RemapDHR2SHR(pixels)
            res = AssignColorGroup(pixels)    
            pixels = res[0]; block = res[1]
            
            # Process in blocks of 7 pixels, to be packed into 2 bytes
            for k in range(7):
                SetSHRColor(block, k, pixels[k])
            blocks.append(block)
        else:
            # Process in blocks of 7 pixels, to be packed into 4 bytes
            block = [0,0,0,0]
            for k in range(7):
                SetDHRColor(block, k, pixels[k])        
            blocks.append(block)

    # Write to file line-by-line
    blocksPerLine = coords[2]/7
    if mode == 'single':
        i = 0; bufferMAIN = []
        while i < len(blocks):
            for j in range(blocksPerLine):
                bufferMAIN.append(blocks[i+j][0])
                bufferMAIN.append(blocks[i+j][1])
            i += blocksPerLine
        return bufferMAIN
    else:
        i = 0; bufferMAIN = []; bufferAUX = []
        while i < len(blocks):
            for j in range(blocksPerLine):
                bufferAUX.append(blocks[i+j][0])      
                bufferAUX.append(blocks[i+j][2])           
                bufferMAIN.append(blocks[i+j][1])        
                bufferMAIN.append(blocks[i+j][3])           
            i += blocksPerLine
        return bufferAUX, bufferMAIN


############################################
def PackAtari(coords, pixdata):

    # Process to double frame buffer
    imgWidth  = coords[2]
    imgHeight = coords[3]
    imgSize   = (imgHeight*imgWidth/4)
    buf1 = [0] * imgSize
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
            buf1[offset] = (buf1[offset] & mask) | col1
            buf2[offset] = (buf2[offset] & mask) | col2  
            
    return buf1+buf2
        
        
############################################
def PackC64(coords, pixdata, paldata):

    # Extract RGB data
    rgb = []
    for i in range(16):
        rgb.append(paldata[i*3:i*3+3])
        
    # Process in blocks of 4 x 8 pixels
    imgWidth  = coords[2]
    imgHeight = coords[3]
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

    return bmp+scr+col
    
    
############################################
def PackLynx(coords, pixdata):

    # Process to double frame buffer
    imgWidth  = coords[2]
    imgHeight = coords[3]
    imgSize   = (imgHeight*imgWidth/2)
    buffer = [0] * imgSize
    offset = 0
    for y in range(imgHeight):
        for x in range(0, imgWidth, 2):
            buffer[offset] = pixdata[y*imgWidth+x]*16 + (pixdata[y*imgWidth+x+1])
            offset += 1
    
    return buffer
    
    
############################################    
def WriteChunks(fid, sizeLst, coorLst, dataLst):

    # Write header information
    fid.write(chr(len(sizeLst)))

    # Write coords/size/data
    for i in range(len(sizeLst)):
        fid.write(''.join([chr(b) for b in coorLst[i]]))
        fid.write(struct.pack('H', sizeLst[i]))
        fid.write(''.join([chr(b) for b in dataLst[i]]))   
    
    
############################################    
#               MAIN CODE                  #
############################################    


# Process chunks definition file
platform = sys.argv[1]
chunkfile = sys.argv[2]
outfolder = sys.argv[3]
imgfile = chunkfile[0:-4] + ".png"

script = open(chunkfile, "r")
lines = script.readlines() 
script.close()

############################
# Create list of chunks
sizeLst  = []
coorLst = []
if platform == 'apple-double':
    dataLst = [[],[]]
else:
    dataLst = []

for line in lines:
    # Skip comments
    if line[0] != '[':
        continue
        
    # Parse coordinates
    offset1 = 1
    offset2 = 1
    while line[offset2] != ']':
        offset2 += 1 
    coords = [int(s) for s in line[offset1:offset2].split(',')]
    coorLst.append(coords)
    
    # Crop required section of image
    img = Image.open(imgfile)
    crop = img.crop((coords[0], coords[1], coords[0]+coords[2], coords[1]+coords[3]))
    pixdata = list(crop.getdata())
    paldata = crop.getpalette()

    # Pack data to required format    
    if platform == 'apple-double':
        dataAUX, dataMAIN = PackApple(coords, pixdata, 'double')
        sizeLst.append(6+len(dataAUX)) 
        dataLst[0].append(dataAUX)
        dataLst[1].append(dataMAIN)
    else:
        if platform == 'apple-single':
            data = PackApple(coords, pixdata, 'single')
        if platform == 'atari':
            data = PackAtari(coords, pixdata)
        if platform == 'c64':
            data = PackC64(coords, pixdata, paldata)
        if platform == 'lynx':
            data = PackLynx(coords, pixdata)
        sizeLst.append(6+len(data)) 
        dataLst.append(data)
    
    print 'Packing Chunk ', coords
   
#######################################
# Write chunk data   
fb = FileBase(chunkfile, '.txt')
f2 = io.open(outfolder + fb + '.chk', 'wb')
if platform == 'apple-double':
    f2.write(struct.pack('H', sum(sizeLst) + 1))
    WriteChunks(f2, sizeLst, coorLst, dataLst[0])
    WriteChunks(f2, sizeLst, coorLst, dataLst[1])
else:
    WriteChunks(f2, sizeLst, coorLst, dataLst)
f2.close() 

