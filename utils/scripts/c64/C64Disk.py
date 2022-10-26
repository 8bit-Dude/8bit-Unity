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
 
import os, sys, subprocess

# Sub-processes
if "nt" == os.name:
    cl15 = "utils\\scripts\\c64\\c1541.exe"
else:
    cl15 = "wine utils/scripts/c64/c1541.exe"

# Decode input
buildFolder = sys.argv[1]
diskname = sys.argv[2]
files = sys.argv[3:len(sys.argv)]

# Order files alphabetically
files.sort()

# Order files as "*.PRG" > "MENU.*" > REST
order = []
for i in range(len(files)):
    if i in order: continue
    if ".prg" in files[i]: order.append(i)
for i in range(len(files)):
    if i in order: continue
    if "menu." in files[i]: order.append(i)
for i in range(len(files)):
    if i in order: continue
    order.append(i)      
files = [files[i] for i in order]
    
# Get block size of each file
blocks = []
for file in files:
    size = os.path.getsize(buildFolder + '/c64/' + file)
    blocks.append(1+(size-1)/254)
if sum(blocks) > 664:
    suffix = "-d1"
else:
    suffix = ""
    
# Write disks
i = 0
while 1:
    # Open new disk
    command = cl15 + ' -format loader,666 d64 ' + buildFolder + '/' + diskname + '-c64' + suffix + '.d64 -attach ' + buildFolder + '/' + diskname + '-c64' + suffix + '.d64 '
    
    # Add files until full
    cumblocks = 0
    while i < len(files):
        j = 0
        filecommand = ""
        fileprefix = os.path.splitext(files[i])[0]
        
        # Find all files with same prefix
        while 1:
            if (i+j) == len(files):
                break
                
            if fileprefix != os.path.splitext(files[i+j])[0]:
                break
            
            cumblocks += blocks[i+j]
            if cumblocks > 664:
                break     

            filecommand += '-write ' + buildFolder + '/c64/' + files[i+j] + ' ' + files[i+j] + ' '                
            j += 1
        
        if cumblocks > 664:
            break
            
        command += filecommand
        i += j
            

    # Execute Command
    subprocess.call(command, shell=True)
    print("Closing File `" + diskname + '-c64' + suffix + ".d64'")
    
    # All done?
    if i == len(files):
        break
        
    # Update suffix (d2, d3, ...)
    suffix = suffix[0:-1] + chr(ord(suffix[-1])+1)
