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
 
import io, os
from PIL import Image

maps = [f for f in os.listdir('.') if f.endswith('.png')]

for map in maps:
    # Read source
    img1 = Image.open(map)
    pixdata = list(img1.getdata())

    # Convert to binary data (8 pixels per byte)
    data = []
    for i in range(0, len(pixdata), 4):
        data.append( chr( (pixdata[i+3]<<6) + (pixdata[i+2]<<4) + (pixdata[i+1]<<2) + (pixdata[i]) ) )
        
    # Write output
    f = io.open(map.replace('.png','.msk'), 'wb')	
    f.write(''.join(data))
    f.close()

