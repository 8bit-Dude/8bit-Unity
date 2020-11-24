"""
 * Copyright (c) 2020 Anthony Beaucamp.
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

import io,struct

maps = ['level1']

types = ['KEY', 'GOBLIN', 'SKELETON'];
stances = ['LEFT', 'RIGHT'];

for map in maps:
    #######################
    # Read ascii txt file
    f1 = io.open(map+'.txt', 'r')

    header = f1.readline()
    locations = f1.readline().split(',')
    space = f1.readline()

    header = f1.readline()
    actors = []
    while True:
        data = f1.readline().split(',')
        if data == ['\n']:
            break
        actors.append(data)
        
    f1.close()

    #######################
    # Write binary nav file
    f2 = io.open(map+'.act', 'wb')

    # Entrance and Exit
    for i in range(len(locations)):
        f2.write(struct.pack('B', int(locations[i])))

    # List of Actors
    f2.write(struct.pack('B', len(actors)))
    for i in range(len(actors)):
        f2.write(struct.pack('B', types.index(actors[i][0].strip())))
        f2.write(struct.pack('B', stances.index(actors[i][1].strip())))
        f2.write(struct.pack('B', int(actors[i][2])))
        f2.write(struct.pack('B', int(actors[i][3])))
        f2.write(struct.pack('B', int(actors[i][4])))            
    f2.close()

