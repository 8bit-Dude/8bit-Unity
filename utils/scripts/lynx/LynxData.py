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
keyboard = int(sys.argv[2])
bmpList = sys.argv[3]
mapList = sys.argv[4]
chrList = sys.argv[5]
musList = sys.argv[6]
shrList = sys.argv[7]
chkList = sys.argv[8]
sprList = sys.argv[9]

# Prepare file information
bmpList = bmpList.split(',') if len(bmpList) else []
mapList = mapList.split(',') if len(mapList) else []
chrList = chrList.split(',') if len(chrList) else []
musList = musList.split(',') if len(musList) else []
shrList = shrList.split(',') if len(shrList) else []
chkList = chkList.split(',') if len(chkList) else []
fileNum = str(len(bmpList) + len(mapList) + len(chrList) + len(musList) + len(shrList) + len(chkList))

with open(output, "wt") as fp:            
    # Generate assembly file with list of read-only data
    fp.write('.global _fileNum\n')
    fp.write('.global _fileSizes\n')
    fp.write('.global _fileNames\n')
    fp.write('.global _spriteData\n')
    if keyboard:
        fp.write('.global _cursorData\n')
        fp.write('.global _keybrdData\n')
    fp.write(';\n')
    
    # Num and sizes of files
    fp.write('.segment "RODATA"\n')
    fp.write('_fileNum: .byte ' + fileNum + '\n')  

    # List of file names and data
    if fileNum > 0:
        # Prepare file size list
        fileSizes = ''
        for file in bmpList+mapList+chrList+musList+shrList+chkList:
            filepath = os.path.dirname(output) + '/' + file
            fileSizes += str(os.path.getsize(filepath)) + ','
        if len(fileSizes): 
            fileSizes = fileSizes[0:-1]
            
        # Rename music files
        for i in range(len(musList)):
            musList[i] = musList[i][0:-4] + '.mus'

        # Declare all Bitmap, Shared and Chunk files
        fp.write('_fileSizes: .word ' + fileSizes + '\n')
        fp.write('_fileNames: .addr ')
        counter = 0
        for i in range(len(bmpList)):
            if counter > 0:
                fp.write(',')
            fp.write('_bmpName' + str(i).zfill(2))
            counter += 1
        for i in range(len(mapList)):
            if counter > 0:
                fp.write(',')
            fp.write('_mapName' + str(i).zfill(2))
            counter += 1
        for i in range(len(chrList)):
            if counter > 0:
                fp.write(',')
            fp.write('_chrName' + str(i).zfill(2))
            counter += 1
        for i in range(len(musList)):
            if counter > 0:
                fp.write(',')
            fp.write('_musName' + str(i).zfill(2))
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
        fp.write('\n')

        # Write list of Asset Files
        for i in range(len(bmpList)):
            fp.write('_bmpName' + str(i).zfill(2) + ': .byte "' + bmpList[i] + '",0\n')
        for i in range(len(mapList)):
            fp.write('_mapName' + str(i).zfill(2) + ': .byte "' + mapList[i] + '",0\n')
        for i in range(len(chrList)):
            fp.write('_chrName' + str(i).zfill(2) + ': .byte "' + chrList[i] + '",0\n')
        for i in range(len(musList)):
            fp.write('_musName' + str(i).zfill(2) + ': .byte "' + musList[i] + '",0\n')
        for i in range(len(shrList)):
            fp.write('_shrName' + str(i).zfill(2) + ': .byte "' + shrList[i] + '",0\n')
        for i in range(len(chkList)):
            fp.write('_chkName' + str(i).zfill(2) + ': .byte "' + chkList[i] + '",0\n')
        fp.write(';\n')
            
        # Link list of bitmaps
        for i in range(len(bmpList)):
            fp.write('.segment "BMP' + str(i) + 'DATA"\n')
            fp.write('_bmpData' + str(i).zfill(2) + ': .incbin "' + bmpList[i] + '"\n')                    

        # Link list of charmaps
        for i in range(len(mapList)):
            fp.write('.segment "BMP' + str(len(bmpList)+i) + 'DATA"\n')
            fp.write('_mapData' + str(i).zfill(2) + ': .incbin "' + mapList[i] +'"\n')

        # Link list of charsets
        for i in range(len(chrList)):
            fp.write('.segment "BMP' + str(len(bmpList)+len(mapList)+i) + 'DATA"\n')
            fp.write('_chrData' + str(i).zfill(2) + ': .incbin "' + chrList[i] + '"\n')                    

        # Link list of musics
        for i in range(len(musList)):
            fp.write('.segment "MUS' + str(i) + 'DATA"\n')
            fp.write('.import _musData' + str(i).zfill(2) + '\n')
            
        # Link list of shared
        for i in range(len(shrList)):
            fp.write('.segment "SHR' + str(i) + 'DATA"\n')
            fp.write('_shrData' + str(i).zfill(2) + ': .incbin "' + shrList[i] + '"\n')

        # Link list of chunks
        for i in range(len(chkList)):
            fp.write('.segment "CHK' + str(i) + 'DATA"\n')
            fp.write('_chkData' + str(i).zfill(2) + ': .incbin "' + chkList[i] + '"\n')                    
                                                    
    else:
        fp.write('_fileSizes: .word 0\n')
        fp.write('_fileNames: .addr _dummyName\n')
        fp.write('_dummyName: .byte 0\n')
    fp.write(';\n')
    
    # Sprite Data 
    fp.write('.segment "RODATA"\n')                
    if len(sprList) > 0:            
        fp.write('_spriteData: .incbin "' + sprList + '"\n')                    
    else:
        fp.write('_spriteData: .byte 0\n')

    # Keyboard Data
    if keyboard:
        fp.write('_cursorData: .incbin "cursor.dat"\n')             
        fp.write('_keybrdData: .incbin "keyboard.dat"\n')                                                 
            
