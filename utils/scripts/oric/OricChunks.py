
import io, os, struct, sys, subprocess

############################################    
# Process chunks definition file
chunkDefs = sys.argv[1]
outfolder = sys.argv[2]
dithering = sys.argv[3]

script = open(chunkDefs, "r")
lines = script.readlines() 
script.close()

listing = open(outfolder+'chunks.lst', "w")
sourceindex = 0;
sources = dict()

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

    # Convert file to oric graphic
    if infile in sources:    
        sourcefile = sources[infile]
    else:
        sourcefile = outfolder + "/source" + str(sourceindex) + ".dat"
        sourcefile = sourcefile.replace('//','/')
        sourceindex += 1
        subprocess.call(["luajit.exe", "PictOric.lua", dithering, infile, sourcefile])
        sources[infile] = sourcefile

    # Read the entire image
    f1 = io.open(sourcefile, 'rb')
    data = f1.read()
    f1.close()
    
    # Only keep the relevant block of converted file
    f2 = io.open(outfile, 'wb')
    f2.write(chr(coords[0]))
    f2.write(chr(coords[1]))
    f2.write(chr(coords[2]))
    f2.write(chr(coords[3]))   
    for y in range(coords[1], coords[1]+coords[3], 1):
        for x in range(coords[0], coords[0]+coords[2], 6):
            f2.write(data[y*40+x/6])
    f2.close()

    # Add file to list
    listing.write(outfile.replace("../../../","")+'\n')
    
listing.close()
