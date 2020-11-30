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

import io,struct

maps = ['arizona','arto','cramp','freeway','gta','island','mtcarlo','rally','river','stadium','suzuka']

for map in maps:
	#######################
	# Read ascii txt file
	f1 = io.open(map+'.txt', 'r')

	header = f1.readline()
	grid = [] 
	while True:
		data = f1.readline().split(',')
		if data == ['\n']:
			break
		grid.append(data)

	header = f1.readline()
	cyls = []
	while True:
		data = f1.readline().split(',')
		if data == ['\n']:
			break
		cyls.append(data)

	header = f1.readline()
	ramps = []
	while True:
		data = f1.readline().split(',')
		if data == ['']:
			break
		ramps.append(data)
        
	f1.close()
	
	#######################
	# Write binary nav file
	f2 = io.open(map+'.nav', 'wb')

	f2.write(struct.pack('B', len(grid)))	# Number of grid points
	for i in range(len(grid)):
		for j in range(0,3):
			f2.write(struct.pack('h', int(grid[i][j])))

	f2.write(struct.pack('B', len(cyls)))	# Number of waypoints
	for i in range(len(cyls)):
		for j in range(0,2):
			f2.write(struct.pack('h', 8*int(cyls[i][j])))   # Coords of waypoint * 8
		for j in range(2,6):
			f2.write(struct.pack('h', int(cyls[i][j])))     # In/Out vectors

	f2.write(struct.pack('B', len(ramps)))	# Number of ramps
	for i in range(len(ramps)):
		for j in range(0,4):
			f2.write(struct.pack('h', 8*int(ramps[i][j])))    # Coords of ramp * 8
            
	f2.close()

