import argparse

parser = argparse.ArgumentParser(
    description='Converts images to the various dimensions and palettes required by 8bit-Unity.')
parser.add_argument('input_files', nargs='+', help='Input images. Accepts Unix styled wildcards.')
parser.add_argument('--platform', action='append', choices=['apple', 'atari', 'c64', 'lynx', 'oric'],
    help='Platform to generate images for. Can be informed more than once. If ommited, generates for all plaforms.')
args = parser.parse_args()