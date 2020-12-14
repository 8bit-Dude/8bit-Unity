import argparse, ntpath, os
from PIL import Image, ImagePalette


def create_pil_palette(rgb_palette):
    """Converts a list of (r, g, b) tuples into a PIL-compatible palette."""
    pal = []
    for (r, g, b) in rgb_palette:
        pal += [r, g, b]
    # For classic PIL, the palette size *must* be 256 * 3
    pal += [0] * (256 * 3 - len(pal))

    return pal


def create_info(w, h, left_margin, palette):
    return ((w, h), left_margin, palette)


PLATFORM_NAMES = ['apple', 'atari', 'c64', 'lynx', 'oric']
PLATFORM_INFOS = {
    'apple': create_info(140, 192, 0,
        [(0, 0, 0), (49, 51, 222), (0, 116, 6), (30, 150, 230), 
         (119, 89, 1), (112, 112, 91), (87, 186, 2), (148, 237, 154), 
         (141, 18, 100), (155, 71, 253), (137, 150, 170), (150, 182, 255), 
         (232, 112, 46), (222, 126, 163), (243, 225, 1), (255, 255, 255)]),
    'atari': create_info(160, 200, 0,
        [(0, 0, 0), (75, 20, 11), (22, 60, 104), (67, 80, 0), 
         (75, 20, 11), (150, 39, 22), (97, 79, 115), (142, 100, 11), 
         (22, 60, 104), (97, 79, 115), (44, 119, 207), (89, 140, 104), 
         (67, 80, 0), (142, 100, 11), (89, 140, 104), (133, 160, 0)]),
    'c64': create_info(160, 200, 0,
        [(0, 0, 0), (255, 255, 255), (104, 55, 43), (112, 164, 178), 
         (111, 61, 134), (88, 141, 67), (53, 40, 121), (184, 199, 111), 
         (111, 79, 37), (67, 57, 0), (154, 103, 89), (68, 68, 68), 
         (108, 108, 108), (154, 210, 132), (108, 94, 181), (149, 149, 149)]),
    'lynx': create_info(160, 102, 0,
        [(165, 16, 198), (66, 49, 181), (66, 132, 198), (49, 198, 198), 
         (49, 198, 82), (33, 132, 33), (33, 82, 82), (115, 82, 33), 
         (231, 115, 82), (247, 231, 0), (247, 148, 165), (214, 49, 66), 
         (132, 33, 66), (247, 247, 247), (132, 132, 132), (0, 0, 0)]),
    'oric': create_info(240, 200, 6,
        [(0, 0, 0), (111, 156, 101), (130, 132, 3), (3, 123, 121), 
         (131, 226, 124), (133, 135, 132), (70, 117, 176), (3, 6, 135), 
         (131, 225, 227), (2, 126, 229), (133, 135, 132), (173, 121, 66), 
         (227, 225, 122), (125, 4, 2), (227, 132, 0), (229, 227, 226), 
         (173, 117, 179), (125, 123, 228), (228, 133, 134), (125, 1, 133)])
}


def parse_command_line():
    """Parses command line arguments and returns the parsed results."""
    parser = argparse.ArgumentParser(
        description='Converts images to the various dimensions and palettes required by 8bit-Unity.')
    parser.add_argument('input_files', nargs='+', help='Paths to the input images.')
    parser.add_argument('--platform', action='append', choices=PLATFORM_NAMES, default=PLATFORM_NAMES,
        help='Platform to generate images for. Can be informed more than once. If ommited, generates for all plaforms.')
    parser.add_argument('--resample', choices=['nearest', 'bilinear', 'bicubic', 'antialias'], default='nearest',
        help='Resampling filter to use when resizing image. Default is "nearest".')
    parser.add_argument('--dither', choices=['none', 'errordiffusion'], default='none',
        help='Dithering to use, if any.')
    return parser.parse_args()


def create_color_finder(rgb_palette):
    """Receives a list of (r, g, b) tuples, and returns a function that converts (r, g, b) tuples into a palette index."""
    color_cache = {}
    def find_color(rgb):
        """Receives a (r, g, b) tuple, and returns the index of the closest corresponding palette entry."""
        index = 0
        if rgb in color_cache:        
            index = color_cache[rgb]
        else:
            best = -1
            ro = rgb[0]
            go = rgb[1]
            bo = rgb[2]
            for n, (r, g, b) in enumerate(rgb_palette):
                distance = (ro - r) ** 2 + (go - g) ** 2 + (bo - b) ** 2
                if best == -1 or distance < best:
                    index = n
                    best = distance
            
            color_cache[rgb] = index

        return index

    return find_color


def create_indexed_image_non_dither(original, rgb_palette):
    """Create a indexed image given the original image and the palette, without dithering."""
    pil_palette = create_pil_palette(rgb_palette)

    newimage = Image.new('P', original.size)
    newimage.putpalette(pil_palette)        

    # The paste() method should have been enough, but, with the PIL version used on 8bit-Unity,
    # it seems to be ignoring the palette set by putpalette().
    # So, manual implementation, it will be.
    find_color = create_color_finder(rgb_palette)
    for x in range(0, original.size[0]):
        for y in range(0, original.size[1]):
            coord = (x, y)
            orig = original.getpixel(coord)
            index = find_color(orig)                
            newimage.putpixel(coord, index)
    
    return newimage


def create_indexed_image_error_diffusion(original, rgb_palette):
    """Create a indexed image given the original image and the palette, with error diffusion dithering."""
    pil_palette = create_pil_palette(rgb_palette)

    newimage = Image.new('P', original.size)
    newimage.putpalette(pil_palette)        

    def clamp_byte(n):
        return max(min(255, n), 0)

    # The paste() method should have been enough, but, with the PIL version used on 8bit-Unity,
    # it seems to be ignoring the palette set by putpalette().
    # So, manual implementation, it will be.
    find_color = create_color_finder(rgb_palette)
    dith_err = [0, 0, 0]
    for y in range(0, original.size[1]):
        # Will run from left to right on even y, and right to left in odd y.
        xrange = range(0, original.size[0])
        if y % 2 != 0:
            xrange = reversed(xrange)

        for x in xrange:
            coord = (x, y)
            orig = original.getpixel(coord)
            with_err = [0, 0, 0]
            clamped = [0, 0, 0]
            for i in [0, 1, 2]:
                with_err[i] = orig[i] + dith_err[i]
                clamped[i] = clamp_byte(with_err[i])

            index = find_color(tuple(clamped))
            newimage.putpixel(coord, index)

            actual = rgb_palette[index]
            for i in [0, 1, 2]:
                dith_err[i] = with_err[i] - actual[i]
    
    return newimage


def create_indexed_image(original, rgb_palette, dither):
    if dither == 'errordiffusion':
        return create_indexed_image_error_diffusion(original, rgb_palette)
    else:
        return create_indexed_image_non_dither(original, rgb_palette)


def convert_image_file(input_name, plaforms=PLATFORM_NAMES, resample='nearest', dither='none'):
    """Reads the input image and converts it to the specified platforms."""
    print('Processing ' + input_name)    

    (input_dir, input_filename) = ntpath.split(input_name)
    input_base_name = ntpath.splitext(input_filename)[0]

    original = Image.open(input_name)
    rgb_image = original if original.mode == 'RGB' else original.convert('RGB')

    for platform in plaforms:
        target_name = ntpath.join(input_dir, input_base_name + '-' + platform + '.png')
        target_name = ntpath.abspath(target_name)
        print('Generating ' + target_name)

        ((target_w, target_h), left_margin, rgb_palette) = PLATFORM_INFOS.get(platform)

        resized = rgb_image.resize((target_w - left_margin, target_h), resample)

        with_margin = resized
        if left_margin:
            with_margin = Image.new('RGB', (target_w, target_h))
            with_margin.paste(resized, (left_margin, 0))

        newimage = create_indexed_image(with_margin, rgb_palette, dither)
        newimage.save(target_name)


if __name__ == "__main__":
    args = parse_command_line()
    resample = getattr(Image, args.resample.upper())

    for input_name in args.input_files:
        convert_image_file(input_name, args.platform, resample, args.dither)
