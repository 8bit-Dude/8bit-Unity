"""
 * Copyright (c) 2022 Anthony Beaucamp.
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

import os,sys

output = sys.argv[1]
bmpList = sys.argv[2]
mapList = sys.argv[3]
chrList = sys.argv[4]
musList = sys.argv[5]
shrList = sys.argv[6]
chkList = sys.argv[7]

# Prepare file information
bmpList = bmpList.split(',') if len(bmpList) else []
mapList = mapList.split(',') if len(mapList) else []
chrList = chrList.split(',') if len(chrList) else []
musList = musList.split(',') if len(musList) else []
shrList = shrList.split(',') if len(shrList) else []
chkList = chkList.split(',') if len(chkList) else []
fileNum = str(len(bmpList) + len(chrList) + len(mapList) + len(shrList) + len(chkList) + len(musList))

with open(output, "wt") as fp:            
    # Generate assembly file with list of read-only data
    fp.write('.global _fileNum\n')
    fp.write('.global _fileSizes\n')
    fp.write('.global _fileBanks\n')
    fp.write('.global _fileNames\n')
    fp.write('.global _fileDatas\n')
    fp.write(';\n')
            
    # Num and sizes of files
    fp.write('.segment "RODATA"\n')
    fp.write('_fileNum: .byte ' + fileNum + '\n')  

    # List of file names and data
    if fileNum > 0:
        # Assign File Sizes and Bank Numbers (fill-up each $4000 block)
        fileSizes = ''
        fileBanks = ''
        curBank = 2
        cumSize = 0
        for file in bmpList+chrList+mapList+shrList+chkList:
            filepath = os.path.dirname(output) + '/' + file
            curSize = os.path.getsize(filepath)
            cumSize += curSize
            if cumSize >= 16384:
                cumSize = curSize
                curBank += 1
            fileSizes += str(curSize) + ','
            fileBanks += str(curBank) + ','

        # Add music files
        for i in range(len(musList)):
            fileSizes += '10,'
            fileBanks += '6,'         

        # Trim last comma
        fileSizes = fileSizes[0:-1]
        fileBanks = fileBanks[0:-1]
            
        # Declare all Bitmap, Shared and Chunk files
        fp.write('_fileNames: .addr ')
        counter = 0
        for i in range(len(bmpList)):
            if counter > 0:
                fp.write(',')
            fp.write('_bmpName' + str(i).zfill(2))
            counter += 1
        for i in range(len(chrList)):
            if counter > 0:
                fp.write(',')
            fp.write('_chrName' + str(i).zfill(2))
            counter += 1
        for i in range(len(mapList)):
            if counter > 0:
                fp.write(',')
            fp.write('_mapName' + str(i).zfill(2))
            counter += 1
        for i in range(len(shrList)):
            if counter > 0:
                fp.write(',')
            fp.write('_shrName' + str(i).zfill(2))
            counter += 1
        for i in range(len(chkList)):
            if counter > 0:
                fp.write(',')
            fp.write('_chkName' + str(i).zfill(2))
            counter += 1
        for i in range(len(musList)):
            if counter > 0:
                fp.write(',')
            fp.write('_musName' + str(i).zfill(2))
            counter += 1
        fp.write('\n')
        
        # Create list of data addresses               
        fp.write('.segment "BANK1"\n')  
        fp.write('_fileSizes: .word ' + fileSizes + '\n')
        fp.write('_fileBanks: .byte ' + fileBanks + '\n')
        fp.write('_fileDatas: .addr ')
        counter = 0
        for i in range(len(bmpList)):
            if counter > 0:
                fp.write(',')
            fp.write('_bmpData' + str(i).zfill(2))
            counter += 1
        for i in range(len(chrList)):
            if counter > 0:
                fp.write(',')
            fp.write('_chrData' + str(i).zfill(2))
            counter += 1
        for i in range(len(mapList)):
            if counter > 0:
                fp.write(',')
            fp.write('_mapData' + str(i).zfill(2))
            counter += 1
        for i in range(len(shrList)):
            if counter > 0:
                fp.write(',')
            fp.write('_shrData' + str(i).zfill(2))
            counter += 1
        for i in range(len(chkList)):
            if counter > 0:
                fp.write(',')
            fp.write('_chkData' + str(i).zfill(2))
            counter += 1
        for i in range(len(musList)):
            if counter > 0:
                fp.write(',')
            fp.write('_musData' + str(i).zfill(2))
            counter += 1
        fp.write('\n')

        # Write list of Asset Files
        for i in range(len(bmpList)):
            fp.write('_bmpName' + str(i).zfill(2) + ': .byte "' + bmpList[i] + '",0\n')
        for i in range(len(chrList)):
            fp.write('_chrName' + str(i).zfill(2) + ': .byte "' + chrList[i][0:-4] + '.chr' + '",0\n')
        for i in range(len(mapList)):
            fp.write('_mapName' + str(i).zfill(2) + ': .byte "' + mapList[i] + '",0\n')
        for i in range(len(shrList)):
            fp.write('_shrName' + str(i).zfill(2) + ': .byte "' + shrList[i] + '",0\n')
        for i in range(len(chkList)):
            fp.write('_chkName' + str(i).zfill(2) + ': .byte "' + chkList[i] + '",0\n')
        for i in range(len(musList)):
            fp.write('_musName' + str(i).zfill(2) + ': .byte "' + musList[i][0:-2] + '.mus' + '",0\n')
        
        # Link asset files to PRG banks
        counter = 0
        for i in range(len(bmpList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_bmpData' + str(i).zfill(2) + ': .incbin "' + bmpList[i] + '"\n')
            counter += 2
        for i in range(len(chrList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_chrData' + str(i).zfill(2) + ': .incbin "' + chrList[i] + '"\n')
            counter += 2
        for i in range(len(mapList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_mapData' + str(i).zfill(2) + ': .incbin "' + mapList[i] + '"\n')
            counter += 2
        for i in range(len(shrList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_shrData' + str(i).zfill(2) + ': .incbin "' + shrList[i] + '"\n')
            counter += 2        
        for i in range(len(chkList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_chkData' + str(i).zfill(2) + ': .incbin "' + chkList[i] + '"\n')
            counter += 2            
        for i in range(len(musList)):
            fp.write('.segment "BANK' + fileBanks[counter] + '"\n')
            fp.write('_musData' + str(i).zfill(2) + ': .include "' + musList[i] + '"\n')
            counter += 2            
            
    else:
        fp.write('_fileSizes: .word 0\n')
        fp.write('_fileBanks: .byte 0\n')
        fp.write('_fileNames: .word 0\n')
        fp.write('_fileDatas: .word 0\n')
        
    # Write list of CHR pages (including default font and sprites)
    fp.write('.segment "CHARS"\n')
    fp.write('_tileset00: .incbin "font.chr"\n')
    fp.write('_tileset01: .incbin "sprites.chr"\n')
    counter = 2
    for i in range(len(bmpList)):
        fp.write('_tileset' + str(counter).zfill(2) + ': .incbin "' + bmpList[i][0:-4] + '.chr"\n')                
        counter += 1
    for i in range(len(chrList)):
        fp.write('_tileset' + str(counter).zfill(2) + ': .incbin "' + chrList[i][0:-4] + '.chr"\n')                
        counter += 1
    fp.write(';\n')
    