import io, sys
from PIL import Image

input = sys.argv[1]
output = sys.argv[2]
height = int(sys.argv[3])

# Read source file
img1 = Image.open(sys.argv[1])
rawdata = list(img1.getdata())

###################################
# Rearrange into 12*8 blocks
pixdata = []
for row in range(0, img1.size[1], height):
    for col in range(0, img1.size[0], 12):
        for j in range(0, height):        
            for i in range(0, 12):
                pixdata.append(rawdata[(row+j)*img1.size[0]+col+i])

#########################
# Fix paper color
sprdata = []

# Left-shifted
for i in range(0, len(pixdata), 12):
    block = []
    block.append(pixdata[i+1:i+7])
    block.append(pixdata[i+7:i+12] + [0])
    for j in range(2):
        pow = 1
        byte = 64
        for k in range(6):
            byte += pow * block[j][5-k]
            pow *= 2
        sprdata.append(chr(byte))

# Right-shifted
for i in range(0, len(pixdata), 12):
    block = []
    block.append([0] + pixdata[i:i+5])
    block.append(pixdata[i+5:i+11])
    for j in range(2):
        pow = 1
        byte = 64
        for k in range(6):
            byte += pow * block[j][5-k]
            pow *= 2
        sprdata.append(chr(byte))        
#print sprdata
        
##########################
# Write modified DAT file
f2 = io.open(output, 'wb')	
f2.write(''.join(sprdata))
f2.close()

