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
    f2 = io.open(map+'.nav', 'wb')

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

