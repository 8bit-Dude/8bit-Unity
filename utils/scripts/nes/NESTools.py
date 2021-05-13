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

palette = [ [0x00, 82, 82, 82],
            [0x01,  1, 26, 81],
            [0x02, 15, 15,101],
            [0x03, 35,  6, 99],
            [0x04, 54,  3, 75],
            [0x05, 64,  4, 38],
            [0x06, 63,  9,  4],
            [0x07, 50, 19,  0],
            [0x08, 31, 32,  0],
            [0x09, 11, 42,  0],
            [0x0a,  0, 47,  0],
            [0x0b,  0, 46, 10],
            [0x0c,  0, 38, 45],
            [0x0f,  0,  0,  0],
            [0x10,160,160,160],
            [0x11, 30, 74,157],
            [0x12, 56, 55,188],
            [0x13, 88, 40,184],
            [0x14,117, 33,148],
            [0x15,132, 35, 92],
            [0x16,130, 46, 36],
            [0x17,111, 63,  0],
            [0x18, 81, 82,  0],
            [0x19, 49, 99,  0],
            [0x1a, 26,107,  5],
            [0x1b, 14,105, 46],
            [0x1c, 16, 92,104],
            [0x20,254,255,255],
            [0x21,105,158,252],
            [0x22,137,135,255],
            [0x23,174,118,255],
            [0x24,206,109,241],
            [0x25,224,112,178],
            [0x26,222,124,112],
            [0x27,200,145, 62],
            [0x28,166,167, 37],
            [0x29,129,186, 40],
            [0x2a, 99,196, 70],
            [0x2b, 84,193,125],
            [0x2c, 86,179,192],
            [0x2d, 60, 60, 60],
            [0x30,254,255,255],
            [0x31,190,214,253],
            [0x32,204,204,255],
            [0x33,221,196,255],
            [0x34,234,192,249],
            [0x35,242,193,223],
            [0x36,241,199,194],
            [0x37,232,208,170],
            [0x38,217,218,157],
            [0x39,201,226,158],
            [0x3a,188,230,174],
            [0x3b,180,229,199],
            [0x3c,181,223,228],
            [0x3d,169,169,169], ]
            
            
def RGB2HSV(rgb):
    r, g, b = rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0
    mx = max(r, g, b)
    mn = min(r, g, b)
    df = mx-mn
    if mx == mn:
        h = 0
    elif mx == r:
        h = (60 * ((g-b)/df) + 360) % 360
    elif mx == g:
        h = (60 * ((b-r)/df) + 120) % 360
    elif mx == b:
        h = (60 * ((r-g)/df) + 240) % 360
    if mx == 0:
        s = 0
    else:
        s = (df/mx)*100
    v = mx*100
    return [h, s, v]

            
def GetPaletteIndex(rgb):
    index = 0; best = 99999999999999999999
    #hsv = RGB2HSV(rgb)
    for i in range(len(palette)):
        pal = palette[i][1:4]
        norm = abs(rgb[0]-pal[0]) + abs(rgb[1]-pal[1]) + abs(rgb[2]-pal[2])
        #pal = RGB2HSV(palette[i][1:4])
        #norm = abs(hsv[0]-pal[0]) + abs(hsv[1]-pal[1]) + abs(hsv[2]-pal[2])
        if norm < best:
            best = norm
            index = i
    return palette[index][0]


def RLECompression(input):

    size = len(input)
        
    # Record value distribution
    stat = [ 0 ] * 256;
    for i in range(size):
        stat[input[i]] += 1

    # Find 0 occurance byte
    tag=-1;
    for i in range(256):
        if(not stat[i]):
            tag=i;
            break;
    if(tag<0): 
        return None;

    rep=1
    sym_prev=-1
    output = [ tag ]

    for i in range(size):

        sym=input[i]

        if (sym_prev != sym or rep>=255 or i==size-1):
            if(rep>1):
                if(rep==2):
                    output.append(sym_prev);
                else:
                    output.append(tag)
                    output.append(rep-1)

            output.append(sym)
            sym_prev=sym
            rep = 1

        else:
            rep += 1

    output.append(tag)	#end of file marked with zero length rle
    output.append(0)
    return output


def TileCompare(tile1, tile2):

    match = 0
    for i in range(8):
        match += bin(255-tile1[i]^tile2[i]).count("1")
        match += bin(255-tile1[i+8]^tile2[i+8]).count("1")
    return match
    
    
#
# Bitmap to multi-console CHR converter using Pillow, the
# Python Imaging Library
#
# Copyright 2014-2015 Damian Yerrick
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty
# provided the copyright notice and this notice are preserved.
# This file is offered as-is, without any warranty.
#

def EncodeChar(tile, planemap, hflip=False, little=False):
    """Turn a tile into bitplanes.
    Planemap opcodes:
    10 -- bit 1 then bit 0 of each tile
    0,1 -- planar interleaved by rows
    0;1 -- planar interlaved by planes
    0,1;2,3 -- SNES/PCE format
    """
    hflip = 7 if hflip else 0
    if (tile.size != (8, 8)):
        return None
    pixels = list(tile.getdata())
    pixelrows = [pixels[i:i + 8] for i in range(0, 64, 8)]
    if hflip:
        for row in pixelrows:
            row.reverse()
    out = bytearray()

    planemap = [[[int(c) for c in row] 
                 for row in plane.split(',')]
                 for plane in planemap.split(';')]
    # format: [tile-plane number][plane-within-row number][bit number]

    # we have five (!) nested loops
    # outermost: separate planes
    # within separate planes: pixel rows
    # within pixel rows: row planes
    # within row planes: pixels
    # within pixels: bits
    for plane in planemap:
        for pxrow in pixelrows:
            for rowplane in plane:
                rowbits = 1
                thisrow = bytearray()
                for px in pxrow:
                    for bitnum in rowplane:
                        rowbits = (rowbits << 1) | ((px >> bitnum) & 1)
                        if rowbits >= 0x100:
                            thisrow.append(rowbits & 0xFF)
                            rowbits = 1
                out.extend(thisrow[::-1] if little else thisrow)
    return bytes(out)
    

def EncodeTiles(im, tileWidth=8, tileHeight=8, formatTile=lambda im: EncodeChar(im, "0;1")):
    """Convert a bitmap image into a list of byte strings representing tiles."""
    im.load()
    (w, h) = im.size
    outdata = []
    for mt_y in range(0, h, tileHeight):
        for mt_x in range(0, w, tileWidth):
            metatile = im.crop((mt_x, mt_y, mt_x + tileWidth, mt_y + tileHeight))
            for tile_y in range(0, tileHeight, 8):
                for tile_x in range(0, tileWidth, 8):
                    tile = metatile.crop((tile_x, tile_y, tile_x + 8, tile_y + 8))
                    data = formatTile(tile)
                    outdata.append(data)
    return outdata
