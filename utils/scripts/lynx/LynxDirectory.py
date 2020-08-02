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
    # Add bitmap assets to Directory file
    with open(input, "rt") as fin:
        with open(output, "wt") as fout:
            for line in fin:
            
                fout.write(line)
                
                if '.import __RODATA_SIZE__' in line:
                    for i in range(bmpNum):
                        fout.write('\n')
                        fout.write('	.export _BMP' + str(i) + '_FILENR : absolute\n')
                        fout.write('	.import __BMP' + str(i) + '_START__\n')
                        fout.write('	.import __BMP' + str(i) + 'DATA_SIZE__\n')
                    for i in range(musNum):
                        fout.write('\n')
                        fout.write('	.export _MUS' + str(i) + '_FILENR : absolute\n')
                        fout.write('	.import __MUS' + str(i) + '_START__\n')
                        fout.write('	.import __MUS' + str(i) + 'DATA_SIZE__\n')
                    for i in range(shrNum):
                        fout.write('\n')
                        fout.write('	.export _SHR' + str(i) + '_FILENR : absolute\n')
                        fout.write('	.import __SHR' + str(i) + '_START__\n')
                        fout.write('	.import __SHR' + str(i) + 'DATA_SIZE__\n')
                    for i in range(shkNum):
                        fout.write('\n')
                        fout.write('	.export _SHK' + str(i) + '_FILENR : absolute\n')
                        fout.write('	.import __SHK' + str(i) + '_START__\n')
                        fout.write('	.import __SHK' + str(i) + 'DATA_SIZE__\n')
                        
                if 'entry __STARTOFDIRECTORY__' in line:
                    lastFilenr = 'MAIN'
                    lastEntry  = 'main'
                    for i in range(bmpNum):
                        fout.write('\n')
                        fout.write('_BMP' + str(i) + '_FILENR=_' + lastFilenr + '_FILENR+1\n')
                        fout.write('entry ' + lastEntry + 'off, ' + lastEntry + 'len, bmp' + str(i) + 'off, bmp' + str(i) + 
                                   'block, bmp' + str(i) + 'len, __BMP' + str(i) + 'DATA_SIZE__, __BMP' + str(i) + '_START__\n')
                        lastFilenr = 'BMP' + str(i)
                        lastEntry  = 'bmp' + str(i)
                    for i in range(musNum):
                        fout.write('\n')
                        fout.write('_MUS' + str(i) + '_FILENR=_' + lastFilenr + '_FILENR+1\n')
                        fout.write('entry ' + lastEntry + 'off, ' + lastEntry + 'len, mus' + str(i) + 'off, mus' + str(i) + 
                                   'block, mus' + str(i) + 'len, __MUS' + str(i) + 'DATA_SIZE__, __MUS' + str(i) + '_START__\n')
                        lastFilenr = 'MUS' + str(i)
                        lastEntry  = 'mus' + str(i)
                    for i in range(shrNum):
                        fout.write('\n')
                        fout.write('_SHR' + str(i) + '_FILENR=_' + lastFilenr + '_FILENR+1\n')
                        fout.write('entry ' + lastEntry + 'off, ' + lastEntry + 'len, shr' + str(i) + 'off, shr' + str(i) + 
                                   'block, shr' + str(i) + 'len, __SHR' + str(i) + 'DATA_SIZE__, __SHR' + str(i) + '_START__\n')
                        lastFilenr = 'SHR' + str(i)
                        lastEntry  = 'shr' + str(i)
                    for i in range(shkNum):
                        fout.write('\n')
                        fout.write('_SHK' + str(i) + '_FILENR=_' + lastFilenr + '_FILENR+1\n')
                        fout.write('entry ' + lastEntry + 'off, ' + lastEntry + 'len, shk' + str(i) + 'off, shk' + str(i) + 
                                   'block, shk' + str(i) + 'len, __SHK' + str(i) + 'DATA_SIZE__, __SHK' + str(i) + '_START__\n')
                        lastFilenr = 'SHK' + str(i)
                        lastEntry  = 'shk' + str(i)
except:
    print 'Error: cannot generate Lynx directory file'
