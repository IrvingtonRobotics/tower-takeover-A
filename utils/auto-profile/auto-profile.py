#!/usr/bin/env python3

from generate_motion_profile import generate
import argparse

parser = argparse.ArgumentParser(description='Autogenerate motion profile from SVG.')
parser.add_argument('file', type=argparse.FileType('r'), nargs="?",
    help="Input file (.svg): overrides specified body")
parser.add_argument('-b', '--body', type=str, help="Input body: overriden by contents of file")
parser.add_argument('-s', '--scale', type=float, default=2,
    help="The field is 144 inches wide. If the drawing is 288 px, then the scale would be 2")

args = parser.parse_args()
try:
    body = args.file.read()
except:
    body = args.body
print(generate(body, args.scale))
