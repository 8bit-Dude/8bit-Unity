
import io, os, struct, sys, subprocess

def FileBase(filepath, suffix):
    # Return asset file base
    name = os.path.basename(filepath).lower().replace(suffix, '')
    name = name.split("-")
    return name[0]

############################################    
# Process chunks definition file
chunkfile = sys.argv[1]
outfolder = sys.argv[2]
dithering = sys.argv[3]
imgfile = chunkfile[0:-4] + ".png"

# Convert file to oric graphic
datfile = outfolder + FileBase(imgfile, ".png") + ".tmp"
datfile = datfile.replace('//','/')
subprocess.call(["luajit.exe", "PictOric.lua", dithering, imgfile, datfile])

# Read converted image
f1 = io.open(datfile, 'rb')
data = f1.read()
f1.close()

# Process Script
script = open(chunkfile, "r")
lines = script.readlines() 
script.close()

coorLst = []
sizeLst = []

for line in lines:
    if line[0] == '\'':
        # Parse charset name
        offset1 = 1
        offset2 = 1    
        while line[offset2] != '\'':
            offset2 += 1
        chrfile = outfolder + '/' + line[offset1:offset2]
        chrfile = chrfile.replace('//','/')
    
    if line[0] == '[':
        # Parse coordinates
        offset1 = 1
        offset2 = 1
        while line[offset2] != ']':
            offset2 += 1
        coords = [int(s) for s in line[offset1:offset2].split(',')]
        coorLst.append(coords)
        
        # Compute size of chunk
        sizeLst.append(4+coords[2]*coords[3]/6)
        print 'Adding Chunk ', coords
    
#######################################
# Output chunk data
fb = FileBase(chunkfile, '.txt')
f2 = io.open(outfolder + fb + '.dat', 'wb')

# Write header information
f2.write(chr(len(sizeLst)))
for i in range(len(sizeLst)):
    f2.write(struct.pack('H', sizeLst[i]))
 
# Write coords/names/attributes
for i in range(len(sizeLst)):

    # Write binary data
    cor = ''.join([chr(b) for b in coorLst[i]])
    f2.write(cor)
    coords = coorLst[i]
    for y in range(coords[1], coords[1]+coords[3], 1):
        for x in range(coords[0], coords[0]+coords[2], 6):
            f2.write(data[y*40+x/6])

f2.close()
