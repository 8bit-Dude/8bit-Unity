
import io, os, struct, sys, subprocess
from PIL import Image

def FileBase(filepath, suffix):
    # Return asset file base
    name = os.path.basename(filepath).lower().replace(suffix, '')
    name = name.split("-")
    return name[0]

# Retrieve command params
chunkfile = sys.argv[1]
outfolder = sys.argv[2]
dithering = sys.argv[3]

# Sub-processes
if "nt" == os.name:
    luaj = ["luajit.exe"]
else:
    luaj = ["wine","luajit.exe"]

# Open Script File
script = open(chunkfile, "r")
lines = script.readlines() 
script.close()

coorLst = []
sizeLst = []

for line in lines:
    # Parse source image
    if line[0] == '"':
        path = os.path.dirname(chunkfile)
        imgFile = path + '/' + line.split('"')[1]
        
        # Add black band on left-side
        padFile = outfolder + FileBase(imgFile, "")
        padFile = padFile.replace('//','/')
        img1 = Image.open(imgFile)
        padding = Image.new("RGB", (240, 200), (0, 0, 0))
        padding.paste(img1, (6, 0))
        padding.save(padFile, "PNG") 
            
        # Call PictOric
        datFile = outfolder + FileBase(imgFile, ".png") + ".tmp"
        datFile = datFile.replace('//','/')
        subprocess.call(luaj + ["PictOric.lua", dithering, padFile, datFile])

        # Read converted image
        f1 = io.open(datFile, 'rb')
        data = f1.read()
        f1.close()        
    
    if line[0] == '[':
        # Parse coordinates
        offset1 = 1
        offset2 = 1
        while line[offset2] != ']':
            offset2 += 1
        coords = [int(s) for s in line[offset1:offset2].split(',')]
        coorLst.append(coords)
        
        # Compute size of chunk
        sizeLst.append(6+coords[3]*(coords[2]/6))
        print 'Adding Chunk ', coords

#######################################
# Output chunk data
f2 = io.open(outfolder + FileBase(chunkfile, '.txt') + '.dat', 'wb')

# Write header information
f2.write(chr(len(sizeLst)))
 
# Write coords/sizes/data
for i in range(len(sizeLst)):

    # Write binary data
    f2.write(''.join([chr(b) for b in coorLst[i]]))
    f2.write(struct.pack('H', sizeLst[i]))
    coords = coorLst[i]
    for y in range(coords[1], coords[1]+coords[3], 1):
        for x in range(coords[0], coords[0]+coords[2], 6):
            f2.write(data[y*40+x/6])

f2.close()
