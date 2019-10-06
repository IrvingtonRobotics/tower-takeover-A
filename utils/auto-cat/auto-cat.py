#!/usr/bin/env python3

# example: python3 utils/auto-cat/auto-cat.py src/*
# or: python3 utils/auto-cat/auto-cat.py src/initialize* src/common* src/autonomous* src/opcontrol* src/controls* src/config* src/ports* src/drive* src/lift* src/intake* src/rails*

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('files', type=argparse.FileType('r'), nargs='*',
    help='files to concatenate together with filenames')
parser.add_argument('-o', '--output', type=argparse.FileType('w'), nargs="?",
    help='output file, default `cat.md`')

args = parser.parse_args()
s = "## Code Printout\n"
for file in args.files:
    s += f"`{file.name}`\n"
    s += f'```cpp\n{file.read()}```\n'
    s += "- - - - -\n"
output = args.output
if not output:
    output = open("cat.md", "w")
output.write(s)
