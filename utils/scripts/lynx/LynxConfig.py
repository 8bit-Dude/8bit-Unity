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

import sys

input = sys.argv[1]
output = sys.argv[2]
bmpNum = int(sys.argv[3])
musNum = int(sys.argv[4])
shrNum = int(sys.argv[5])
shkNum = int(sys.argv[6])

try:
    # Add bitmap assets to Config file
    with open(input, "rt") as fin:
        with open(output, "wt") as fout:
            for line in fin:
            
                if 'size = 1*8' in line:
                    line = line.replace('1*8', str(1+bmpNum+musNum+shrNum+shkNum) + '*8')
                fout.write(line)
                
                if 'size = __MAINSIZE__' in line:
                    for i in range(bmpNum):
                        fout.write('	BMP' + str(i) + ':	file = %O, define = yes, start = $C038 - __BMPSIZE__, size = __BMPSIZE__;\n')
                    for i in range(musNum):
                        fout.write('	MUS' + str(i) + ':	file = %O, define = yes, start = $C038 - __BMPSIZE__ - __MUSSIZE__, size = __MUSSIZE__;\n')
                    for i in range(shrNum):
                        fout.write('	SHR' + str(i) + ':	file = %O, define = yes, start = $C038 - __BMPSIZE__ - __MUSSIZE__ - __SHRSIZE__, size = __SHRSIZE__;\n')
                    for i in range(shkNum):
                        fout.write('	SHK' + str(i) + ':	file = %O, define = yes, start = $C038 - __BMPSIZE__ - __MUSSIZE__ - __SHRSIZE__, size = __SHRSIZE__;\n')
                        
                if 'BSS:       load = MAIN' in line:
                    for i in range(bmpNum):
                        fout.write('	BMP' + str(i) + 'DATA:  load = BMP' + str(i) + ',	  type = rw,  define = yes;\n')
                    for i in range(musNum):
                        fout.write('	MUS' + str(i) + 'DATA:  load = MUS' + str(i) + ',	  type = rw,  define = yes;\n')
                    for i in range(shrNum):
                        fout.write('	SHR' + str(i) + 'DATA:  load = SHR' + str(i) + ',	  type = rw,  define = yes;\n')
                    for i in range(shkNum):
                        fout.write('	SHK' + str(i) + 'DATA:  load = SHK' + str(i) + ',	  type = rw,  define = yes;\n')
                
except:
    print 'Error: cannot generate Lynx config file'
