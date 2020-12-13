import argparse, ntpath, os
from PIL import Image

PLATFORM_NAMES = ['apple', 'atari', 'c64', 'lynx', 'oric']

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

        target = original.resize((160, 200), resample)
        target.save(target_name)
