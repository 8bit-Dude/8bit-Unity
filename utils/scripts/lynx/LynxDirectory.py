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
                        
                if 'entry __STARTOFDIRECTORY__' in line:
                    for i in range(bmpNum):
                        fout.write('\n')     
                        if i == 0:
                            fout.write('_BMP' + str(i) + '_FILENR=_MAIN_FILENR+1\n')
                            fout.write('entry mainoff, mainlen, bmp' + str(i) + 'off, bmp' + str(i) + 
                                       'block, bmp' + str(i) + 'len, __BMP' + str(i) + 'DATA_SIZE__, __BMP' + str(i) + '_START__\n')
                        else:
                            fout.write('_BMP' + str(i) + '_FILENR=_BMP' + str(i-1) + '_FILENR+1\n')
                            fout.write('entry bmp' + str(i-1) + 'off, bmp' + str(i-1) + 'len, bmp' + str(i) + 'off, bmp' + str(i) + 
                                       'block, bmp' + str(i) + 'len, __BMP' + str(i) + 'DATA_SIZE__, __BMP' + str(i) + '_START__\n')                        
                    for i in range(musNum):
                        fout.write('\n')     
                        if i == 0:
                            fout.write('_MUS' + str(i) + '_FILENR=_BMP' + str(bmpNum-1) + '_FILENR+1\n')
                            fout.write('entry bmp' + str(bmpNum-1) + 'off, bmp' + str(bmpNum-1) + 'len, mus' + str(i) + 'off, mus' + str(i) + 
                                       'block, mus' + str(i) + 'len, __MUS' + str(i) + 'DATA_SIZE__, __MUS' + str(i) + '_START__\n')
                        else:
                            fout.write('_MUS' + str(i) + '_FILENR=_MUS' + str(i-1) + '_FILENR+1\n')
                            fout.write('entry mus' + str(i-1) + 'off, mus' + str(i-1) + 'len, mus' + str(i) + 'off, mus' + str(i) + 
                                       'block, mus' + str(i) + 'len, __MUS' + str(i) + 'DATA_SIZE__, __MUS' + str(i) + '_START__\n')                        
             
except:
    print 'Error: cannot generate Lynx directory file'
