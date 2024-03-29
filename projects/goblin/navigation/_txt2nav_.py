"""
 * Copyright (c) 2021 Anthony Beaucamp.
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

import io, os, re, struct
from PIL import Image, ImageDraw

def append(text):
    # Check if string already exists
    global strings
    for string in strings:
        if text == string[0]:
            return string[1]        
    strings.append([text, string[2], string[2]+len(text)-text.count("\\n")])
    return string[2]

global strings

DISABLED  = 0
ACTIVE    = 1
PICKABLE  = 2
LOADSCENE = 4

scenes = [f.replace('.txt','') for f in os.listdir('.') if (f.endswith('.txt') and 'readme' not in f)]

for scene in scenes:
    #######################
    # Read ascii txt file
    print "Converting: " + scene
    f1 = io.open(scene+'.txt', 'r')
    
    # Initialize lists
    sid = 0
    music = 0
    startx = [0,0,0,0]
    starty = [0,0,0,0]
    definitions = {'MAX_POLYGON':64, 'MAX_CHUNK':8, 'MAX_ANIMATION':4, 'MAX_INTERACT':8, 'MAX_TRIGGER':8, 'MAX_MODIFIER':4, 'MAX_PATH':8}
    polygon = [[],[]]
    animations = []
    interacts = []
    triggers = []
    modifiers = []
    paths = []
    strings = [['\0', 0, 1]]

    while True:
        line = f1.readline()
        if not line:
            break
            
        if '#define' in line:
            define = line.split()
            definitions[define[1]] = int(define[2])
            continue

        if 'ID:' in line:
            sid = int(line[3:-1])
            continue
            
        if 'MUSIC:' in line:
            string = line[6:-1].split('"')[1]
            if len(string):
                music = append(string + '\0')
            continue
            
        if 'STARTX:' in line:
            flags = line[7:-1].split(';')
            for i in range (0,len(flags)):
                startx[i] = int(flags[i])   
            continue

        if 'STARTY:' in line:
            flags = line[7:-1].split(';')
            for i in range (0,len(flags)):
                starty[i] = int(flags[i])   
            continue
                        
        if 'X:' in line or 'Y:' in line:
            coords = line[2:-1].split(';')
            for i in range (0,len(coords)):
                coords[i] = int(coords[i])
            if 'X:' in line:
                polygon[0] = coords
            if 'Y:' in line:
                polygon[1] = coords
            continue
            
        if 'A:' in line:
            # Fetch animation
            animation = line[2:-1].split(';')
            
            # Decode animation
            for i in [0,1,2,3,4,5,6,7]:
                animation[i] = int(animation[i])
            animations.append(animation)                 
            continue            
            
        if 'I:' in line:
            # Fetch interact
            interact = line[2:-1].split(';')
            
            # Decode coordinates/chunks/paths
            for i in [1,2,3,4,5,7,8,9,10]:
                interact[i] = int(interact[i])
                
            # Decode flags
            flags = DISABLED
            if "ACTIVE" in interact[6]:
                flags |= ACTIVE
            if "PICKABLE" in interact[6]:
                flags |= PICKABLE
            if "LOADSCENE" in interact[6]:
                flags |= LOADSCENE
            interact[6] = flags
            
            # Clean strings
            for i in [0, 11, 12]:
                string = re.findall(r'"([^"]*)"', interact[i])
                interact[i] = append(string[0] + '\0')
            interacts.append(interact)                
            continue
                
        if 'T:' in line:
            # Fetch trigger
            trigger = line[2:-1].split(';')
            
            # Decode modifier/chunks
            for i in [3,4,5]:
                trigger[i] = int(trigger[i])

            # Clean strings
            for i in [0,1,2,6]:
                string = re.findall(r'"([^"]*)"', trigger[i])
                trigger[i] = append(string[0] + '\0')
            triggers.append(trigger)
            continue

        if 'M:' in line:
            # Fetch modifier
            modifier = line[2:-1].split(';')

            # Decode flags
            flags = DISABLED
            if "ACTIVE" in modifier[1]:
                flags |= ACTIVE
            if "PICKABLE" in modifier[1]:
                flags |= PICKABLE
            if "LOADSCENE" in modifier[1]:
                flags |= LOADSCENE                
            modifier[1] = flags                
            
            # Decode Chunks/Path
            for i in [2,3,4]:
                modifier[i] = int(modifier[i])
                
            # Clean strings
            for i in [0,5,6]:
                string = re.findall(r'"([^"]*)"', modifier[i])
                modifier[i] = append(string[0] + '\0')
            modifiers.append(modifier)
            continue
                
        if 'P:' in line:
            # Fetch path
            path = line[2:-1].split(';')
            
            # Decode coordinates/speed/frame/next
            for i in [0,1,2,3,4]:
                path[i] = int(path[i])                                
            paths.append(path)
                            
    f1.close()  
    	
	#######################
	# Write binary nav file
    f2 = io.open(scene+'.nav', 'wb')

    # Scene settings
    f2.write(struct.pack('B', sid))
    f2.write(struct.pack('B', music))
    for i in range(4):
        f2.write(struct.pack('H', startx[i]))
    for i in range(4):
        f2.write(struct.pack('H', starty[i]))
    
    # Number of available items
    f2.write(struct.pack('B', len(polygon[0])))
    f2.write(struct.pack('B', len(animations)))
    f2.write(struct.pack('B', len(interacts)))
    f2.write(struct.pack('B', len(triggers)))
    f2.write(struct.pack('B', len(modifiers)))
    f2.write(struct.pack('B', len(paths)))
    
    # Polygons
    for i in range(definitions['MAX_POLYGON']):
        if i<len(polygon[0]):
            f2.write(struct.pack('h', polygon[0][i])) # X
        else:
            f2.write(struct.pack('H', 0))
    for i in range(definitions['MAX_POLYGON']):
        if i<len(polygon[1]):
            f2.write(struct.pack('h', polygon[1][i])) # Y
        else:
            f2.write(struct.pack('H', 0))

    # Animations
    for i in range(definitions['MAX_ANIMATION']):
        if i<len(animations):
            animation = animations[i]
            f2.write(struct.pack('B', animation[0]))  # Chunk 1
            f2.write(struct.pack('B', animation[1]))  # Chunk 2
            f2.write(struct.pack('B', animation[2]))  # Chunk 3
            f2.write(struct.pack('B', animation[3]))  # BGG
            f2.write(struct.pack('B', animation[4]))  # Time 1
            f2.write(struct.pack('B', animation[5]))  # Time 2
            f2.write(struct.pack('B', animation[6]))  # Time 3
            f2.write(struct.pack('B', animation[7]))  # Time BGG
            f2.write(struct.pack('B', 0))
            f2.write(struct.pack('B', 0))
        else:
            for i in range(10):
                f2.write(struct.pack('B', 0))
                
    # Interacts
    for i in range(definitions['MAX_INTERACT']):
        if i<len(interacts):
            interact = interacts[i]
            f2.write(struct.pack('H', interact[0]))  # Label
            f2.write(struct.pack('H', interact[1]))  # Area X
            f2.write(struct.pack('H', interact[2]))  # Area Y
            f2.write(struct.pack('H', interact[3]))  # Area R
            f2.write(struct.pack('H', interact[4]))  # Move X
            f2.write(struct.pack('H', interact[5]))  # Move Y
            f2.write(struct.pack('B', interact[6]))  # Flags
            f2.write(struct.pack('B', interact[7]))  # CHK
            f2.write(struct.pack('B', interact[8]))  # BCG
            f2.write(struct.pack('B', interact[9]))  # Frame
            f2.write(struct.pack('B', interact[10])) # Path
            f2.write(struct.pack('H', interact[11])) # Question
            f2.write(struct.pack('H', interact[12])) # Answer
        else:
            for i in range(21):
                f2.write(struct.pack('B', 0))
    
    # Triggers
    for i in range(definitions['MAX_TRIGGER']):
        if i<len(triggers):
            trigger = triggers[i]
            f2.write(struct.pack('H', trigger[0]))   # Item
            f2.write(struct.pack('H', trigger[1]))   # Target
            f2.write(struct.pack('H', trigger[2]))   # convert
            f2.write(struct.pack('B', trigger[3]))   # Modifier
            f2.write(struct.pack('B', trigger[4]))   # CHK
            f2.write(struct.pack('B', trigger[5]))   # BCG
            f2.write(struct.pack('H', trigger[6]))   # Answer
        else:
            for i in range(11):
                f2.write(struct.pack('B', 0))

    # Modifiers
    for i in range(definitions['MAX_MODIFIER']):
        if i<len(modifiers):
            modifier = modifiers[i]
            f2.write(struct.pack('H', modifier[0]))  # Interact
            f2.write(struct.pack('B', modifier[1]))  # New Flags
            f2.write(struct.pack('B', modifier[2]))  # New CHK
            f2.write(struct.pack('B', modifier[3]))  # New BCG
            f2.write(struct.pack('B', modifier[4]))  # New Path
            f2.write(struct.pack('H', modifier[5]))  # Question
            f2.write(struct.pack('H', modifier[6]))  # Answer
        else:
            for i in range(10):
                f2.write(struct.pack('B', 0))
            
    # Paths
    for i in range(definitions['MAX_PATH']):
        if i<len(paths):
            path = paths[i]
            f2.write(struct.pack('H', path[0]))   # To X            
            f2.write(struct.pack('H', path[1]))   # To Y   
            f2.write(struct.pack('B', path[2]))   # Speed   
            f2.write(struct.pack('B', path[3]))   # Frame   
            f2.write(struct.pack('B', path[4]))   # Next Path  
        else:
            for i in range(7):
                f2.write(struct.pack('B', 0))
        
    # Text
    for string in strings:
        blocks = string[0].split("\\n")
        for i in range(len(blocks)):
            block = blocks[i]
            f2.write(block)
            if i<len(blocks)-1:
                f2.write(chr(10))
    f2.close()

    print "<< SETTINGS >>"
    print "ID:", str(sid), "/ Music:", strings[music][0]
    for i in range(4):
        print "Player", str(i), ":", startx[i], starty[i]
    print "<< ANIMATIONS >>"
    for animation in animations:
        print animation
    print "<< INTERACTS >>"
    for interact in interacts:
        print interact
    print "<< TRIGGERS >>"
    for trigger in triggers:
        print trigger
    print "<< MODIFIERS >>"
    for modifier in modifiers:
        print modifier
    print "<< PATHS >>"
    for path in paths:
        print path
    print "<< STRINGS >>"
    for string in strings:
        print string
        
	#############################
	# Write image for debugging
    try:
        im = Image.open("../bitmaps/" + scene + ".png")
        draw = ImageDraw.Draw(im)
        
        # Draw motion polygon
        for i in range(0,len(polygon[0])):
            if i < (len(polygon[0])-1):
                draw.line([(polygon[0][i],polygon[1][i]),(polygon[0][i+1],polygon[1][i+1])], (0,255,0))
            else:
                draw.line([(polygon[0][i],polygon[1][i]),(polygon[0][0],polygon[1][0])], (0,255,0))
                
        # Draw interacts
        for interact in interacts:
            # Area of interact
            draw.arc([interact[1]-interact[3], interact[2]-interact[3], interact[1]+interact[3], interact[2]+interact[3]], 0, 360, (0,0,255)) 
            
            # Move target
            draw.line([(interact[4]-3,interact[5]-3),(interact[4]+3,interact[5]+3)], (0,0,255))
            draw.line([(interact[4]+3,interact[5]-3),(interact[4]-3,interact[5]+3)], (0,0,255))

        # Draw paths
        for path in paths:
            draw.arc([path[0]-5, path[1]-5, path[0]+5, path[1]+5], 0, 360, (255,0,0))
            
        im.save(scene + ".png", "PNG")   
    except:
        pass
