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

import io,struct, sys
from PIL import Image

input = sys.argv[1]
output = sys.argv[2]

try:
    # Slightly modify ASM file output by Chipper
    with open(input, "rt") as fin:
        with open(output, "wt") as fout:
            for line in fin:
                line = line.replace('soundbs.mac', 'chipper.s')
                line = line.replace('musicptr', '_musicData')
                fout.write(line)
                
except:
    print 'Error: cannot convert ' + input + '... (was it exported from Chipper as cc65 "remake"?)'