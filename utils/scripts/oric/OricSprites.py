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
for i in range(0, len(pixdata), 6):
    pow = 1
    byte = 64
    for j in range(6):
        byte += pow * pixdata[i+(5-j)]
        pow *= 2
    sprdata.append(chr(byte))
#print sprdata
        
##########################
# Write modified DAT file
f2 = io.open(output, 'wb')	
f2.write(''.join(sprdata))
f2.close()

