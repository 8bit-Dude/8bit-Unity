"""
 * Copyright (c) 2020 Anthony Beaucamp.
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

###########################
# Hi-res Lines Array
HiresLines = [
    0x0000, 0x0400, 0x0800, 0x0C00, 0x1000, 0x1400, 0x1800, 0x1C00,
    0x0080, 0x0480, 0x0880, 0x0C80, 0x1080, 0x1480, 0x1880, 0x1C80,
    0x0100, 0x0500, 0x0900, 0x0D00, 0x1100, 0x1500, 0x1900, 0x1D00,
    0x0180, 0x0580, 0x0980, 0x0D80, 0x1180, 0x1580, 0x1980, 0x1D80,
    0x0200, 0x0600, 0x0A00, 0x0E00, 0x1200, 0x1600, 0x1A00, 0x1E00,
    0x0280, 0x0680, 0x0A80, 0x0E80, 0x1280, 0x1680, 0x1A80, 0x1E80,
    0x0300, 0x0700, 0x0B00, 0x0F00, 0x1300, 0x1700, 0x1B00, 0x1F00,
    0x0380, 0x0780, 0x0B80, 0x0F80, 0x1380, 0x1780, 0x1B80, 0x1F80,
    0x0028, 0x0428, 0x0828, 0x0C28, 0x1028, 0x1428, 0x1828, 0x1C28,
    0x00A8, 0x04A8, 0x08A8, 0x0CA8, 0x10A8, 0x14A8, 0x18A8, 0x1CA8,
    0x0128, 0x0528, 0x0928, 0x0D28, 0x1128, 0x1528, 0x1928, 0x1D28,
    0x01A8, 0x05A8, 0x09A8, 0x0DA8, 0x11A8, 0x15A8, 0x19A8, 0x1DA8,
    0x0228, 0x0628, 0x0A28, 0x0E28, 0x1228, 0x1628, 0x1A28, 0x1E28,
    0x02A8, 0x06A8, 0x0AA8, 0x0EA8, 0x12A8, 0x16A8, 0x1AA8, 0x1EA8,
    0x0328, 0x0728, 0x0B28, 0x0F28, 0x1328, 0x1728, 0x1B28, 0x1F28,
    0x03A8, 0x07A8, 0x0BA8, 0x0FA8, 0x13A8, 0x17A8, 0x1BA8, 0x1FA8,
    0x0050, 0x0450, 0x0850, 0x0C50, 0x1050, 0x1450, 0x1850, 0x1C50,
    0x00D0, 0x04D0, 0x08D0, 0x0CD0, 0x10D0, 0x14D0, 0x18D0, 0x1CD0,
    0x0150, 0x0550, 0x0950, 0x0D50, 0x1150, 0x1550, 0x1950, 0x1D50,
    0x01D0, 0x05D0, 0x09D0, 0x0DD0, 0x11D0, 0x15D0, 0x19D0, 0x1DD0,
    0x0250, 0x0650, 0x0A50, 0x0E50, 0x1250, 0x1650, 0x1A50, 0x1E50,
    0x02D0, 0x06D0, 0x0AD0, 0x0ED0, 0x12D0, 0x16D0, 0x1AD0, 0x1ED0,
    0x0350, 0x0750, 0x0B50, 0x0F50, 0x1350, 0x1750, 0x1B50, 0x1F50,
    0x03D0, 0x07D0, 0x0BD0, 0x0FD0, 0x13D0, 0x17D0, 0x1BD0, 0x1FD0 ]
    
#############################################
# Set color of specific pixel (Single Hires)
# 0=BLACK, 1=VIOLET/BLUE, 2=GREEN/ORANGE, 3=WHITE
def SetSHRColor(block, pixel, color):
	
	if pixel == 0:
		block[0] |= (color & 3)
	if pixel == 1:
		block[0] |= (color & 3) << 2
	if pixel == 2:
		block[0] |= (color & 3) << 4
	if pixel == 3:
		block[0] |= (color & 1) << 6
		block[1] |= (color & 2) >> 1
	if pixel == 4:
		block[1] |= (color & 3) << 1
	if pixel == 5:
		block[1] |= (color & 3) << 3
	if pixel == 6:
		block[1] |= (color & 3) << 5

#############################################
# Set color of specific pixel (Double Hires)
def SetDHRColor(block, pixel, color):

	if pixel == 0:
		block[0] |= (color & 15)
	if pixel == 1:
		block[0] |= (color &  7) << 4
		block[1] |= (color &  8) >> 3
	if pixel == 2:
		block[1] |= (color & 15) << 1
	if pixel == 3:
		block[1] |= (color &  3) << 5
		block[2] |= (color & 12) >> 2
	if pixel == 4:
		block[2] |= (color & 15) << 2
	if pixel == 5:
		block[2] |= (color &  1) << 6
		block[3] |= (color & 14) >> 1
	if pixel == 6:
		block[3] |= (color & 15) << 3
		
#############################################
def RemapDHR2SHR(pixels):
	remap = [x+100 for x in pixels]                
	remap = [0 if x==100 else x for x in remap] # BLACK
	remap = [1 if x==101 else x for x in remap] # DBLUE -> PURPLE
	remap = [2 if x==102 else x for x in remap] # DGREEN -> GREEN
	remap = [3 if x==103 else x for x in remap] # BLUE
	remap = [4 if x==104 else x for x in remap] # BROWN -> ORANGE
	remap = [0 if x==105 else x for x in remap] # DGREY -> BLACK
	remap = [2 if x==106 else x for x in remap] # GREEN
	remap = [2 if x==107 else x for x in remap] # LGREEN -> GREEN
	remap = [0 if x==108 else x for x in remap] # DPURPLE -> BLACK
	remap = [1 if x==109 else x for x in remap] # PURPLE                
	remap = [3 if x==110 else x for x in remap] # LGREY -> BLUE                
	remap = [3 if x==111 else x for x in remap] # LBLUE -> BLUE
	remap = [4 if x==112 else x for x in remap] # ORANGE
	remap = [5 if x==113 else x for x in remap] # PINK -> WHITE
	remap = [2 if x==114 else x for x in remap] # YELLOW -> GREEN
	remap = [5 if x==115 else x for x in remap] # WHITE    
	return remap
	
#############################################
def AssignColorGroup(pixels): 
	# Find group (Purple/Green) or (Blue/Orange) with most occurence
	group1 = pixels.count(1)+pixels.count(2)
	group2 = pixels.count(3)+pixels.count(4)
	if group1 > group2:
		block = [0,0]
	else:
		block = [128,128] 

	# Re-assign all colors according to that group
	pixels = [1 if x==3 else x for x in pixels]
	pixels = [2 if x==4 else x for x in pixels]
	pixels = [3 if x==5 else x for x in pixels]
	return pixels, block
