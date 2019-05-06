
import io,struct, sys
from PIL import Image
from collections import Counter
from math import sqrt

input = sys.argv[1]
output = sys.argv[2]

try:
    # Read source file
    img1 = Image.open(sys.argv[1])
    pixdata = list(img1.getdata())

    # Force BW over WB order for grey colour
    pixdata = [pix if pix != 5 else 10 for pix in pixdata]

    # Get palette RGB
    rgb = []
    dump = img1.getpalette()
    for i in range(20):
        rgb.append(dump[i*3:i*3+3])

    # Inversion groups
    groups = [ [0,1,2,3,4],         # line 1 and 2 non-inverted
               [5,6,7,8,9],         # line 1 inverted
               [10,11,12,13,14],    # line 2 inverted
               [15,16,17,18,19] ]   # line 1 and 2 inverted
               
    # Convert to AIC format
    data = [0]*8000
    for y in range(100):
        # Set paper/ink
        data[(2*y+0)*40+0] = 3    # Ink: Yellow / Blue
        data[(2*y+1)*40+0] = 6    # Ink: Cyan / Red
        
        # Process in blocks of 3x1 pixels (encoded as 6x2)
        for x in range(0,39):
            # Find most frequent colour in block
            block = pixdata[y*117+x*3:y*117+x*3+3]
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
                        vec2 = rgb[groups[gsel][k]]
                        delta.append( sqrt( (vec1[0]-vec2[0])**2 + (vec1[1]-vec2[1])**2 + (vec1[2]-vec2[2])**2 ) )
                    swap = delta.index(min(delta))
                    block[i] = groups[gsel][swap]
            
            # Assign inversion bit
            byte1 = (2*y+0)*40+x+1
            byte2 = (2*y+1)*40+x+1
            if gsel == 0:
                data[byte1] = 64
                data[byte2] = 64
            if gsel == 1:
                data[byte1] = 192
                data[byte2] = 64
            if gsel == 2:
                data[byte1] = 64
                data[byte2] = 192
            if gsel == 3:
                data[byte1] = 192
                data[byte2] = 192
                
            # Assign bits
            for i in range(3):
                index = block[i] % 5
                if index == 1:
                    data[byte1] += 32 / 4**i
                    data[byte2] += 16 / 4**i
                if index == 2:
                    data[byte1] += (32+16) / 4**i
                if index == 3:
                    data[byte2] += (32+16) / 4**i
                if index == 4:
                    data[byte1] += (32+16) / 4**i
                    data[byte2] += (32+16) / 4**i
            
    # Convert to char
    for i in range(len(data)):
        data[i] = chr(data[i])
                
    #################
    # Write DAT file
    f2 = io.open(output, 'wb')	
    f2.write(''.join(data))
    f2.close()

except:
    print "Error: cannot convert " + input + "... (is it a 117x100 PNG file with 20 color palette?)"
