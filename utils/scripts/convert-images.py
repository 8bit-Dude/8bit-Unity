import argparse
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

for input_file in args.input_files:
    print("Processing " + input_file)
    original = Image.open(input_file)
    target = original.copy()
