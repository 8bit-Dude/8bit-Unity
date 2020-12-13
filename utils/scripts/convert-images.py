import argparse, ntpath, os
from PIL import Image, ImagePalette

def create_info(w, h, palette):
    pal = []
    for (r, g, b) in palette:
        pal += [r, g, b]
    # For classic PIL, the palette size *must* be 256 * 3
    pal += [0] * (256 * 3 - len(pal))
    return ((w, h), pal, palette)

PLATFORM_NAMES = ['apple', 'atari', 'c64', 'lynx', 'oric']
PLATFORM_INFOS = {
    'apple': create_info(140, 192, [(0, 1, 2), (250, 251, 252), (255, 0, 0), (0, 255, 0), (0, 0, 255)]),
    'atari': create_info(160, 200, [(0, 1, 2), (250, 251, 252)]),
    'c64': create_info(160, 200, [(0, 1, 2), (250, 251, 252)]),
    'lynx': create_info(160, 102, [(0, 1, 2), (250, 251, 252)]),
    'oric': create_info(117, 100, [(0, 1, 2), (250, 251, 252)])
}

parser = argparse.ArgumentParser(
    description='Converts images to the various dimensions and palettes required by 8bit-Unity.')
parser.add_argument('input_files', nargs='+', help='Input images. Accepts Unix styled wildcards.')
parser.add_argument('--platform', action='append', choices=PLATFORM_NAMES, default=PLATFORM_NAMES,
    help='Platform to generate images for. Can be informed more than once. If ommited, generates for all plaforms.')
parser.add_argument('--resample', choices=['nearest', 'box', 'bilinear', 'hamming', 'bicubic', 'lanczos'], default='nearest',
    help='Resampling filter to use when resizing image. Default is "nearest".')
args = parser.parse_args()

resample = getattr(Image, args.resample.upper())
print(resample)

for input_name in args.input_files:
    print('Processing ' + input_name)    

    (input_dir, input_filename) = ntpath.split(input_name)
    (input_base_name, input_ext) = ntpath.splitext(input_filename)
    print(input_dir, input_filename, input_base_name, input_ext)

    original = Image.open(input_name)

    for platform in PLATFORM_NAMES:
        target_name = ntpath.join(input_dir, input_base_name + '-' + platform + '.png')
        target_name = ntpath.abspath(target_name)
        print('Generating ' + target_name)

        (target_size, pil_palette, rgb_palette) = PLATFORM_INFOS.get(platform)
        resized = original.resize(target_size, resample)

        newimage = Image.new('P', target_size)
        newimage.putpalette(pil_palette)        

        # The paste() method should have been enough, but, with the PIL version used on 8bit-Unity,
        # it seems to be ignoring the palette set by putpalette().
        # So, manual implementation, it will be.
        color_cache = {}
        for x in range(0, target_size[0]):
            for y in range(0, target_size[1]):
                coord = (x, y)
                orig = resized.getpixel(coord)
                index = 0
                if orig in color_cache:
                    index = color_cache[orig]
                else:
                    best = -1
                    ro = orig[0]
                    go = orig[1]
                    bo = orig[2]
                    for n, (r, g, b) in enumerate(rgb_palette):
                        distance = (ro - r) ** 2 + (go - g) ** 2 + (bo - b) ** 2
                        if best == -1 or distance < best:
                            index = n
                            best = distance
                    
                    color_cache[orig] = index
                
                newimage.putpixel(coord, index)

        newimage.save(target_name)
