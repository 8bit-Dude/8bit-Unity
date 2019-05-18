import io, sys
from PIL import Image

input = sys.argv[1]
output = sys.argv[2]

# Read source file
img1 = Image.open(sys.argv[1])
pixdata = list(img1.getdata())
#print pixdata

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

