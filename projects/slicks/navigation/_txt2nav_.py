import io,struct

maps = ['arizona','arto','cramp','freeway','gta','island','mtcarlo','rally','river','stadium']

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

