from re import match
from os.path import isfile
from argparse import ArgumentParser

from _utils import error


parser = ArgumentParser()
parser.add_argument("-l", "--headers", nargs='+', help="Headers to merge into MushLib header (sorted!)", required=True)

args = vars(parser.parse_args())
print("#ifndef MUSHLIB\n#define MUSHLIB")

for file in args["headers"]:
    if not isfile(file):
        error(f"File `{file}` can not be opened")

    with open(file, 'r') as header:
        for line in header.readlines():
            if not line.strip():
                print()
            elif match(r"(#include.*|#ifndef.*|#define\ MUSHLIB.*|#endif.*)", line):
                print("\n\n// HEADER: someheader //\n\n")
            else:
                print(line, end="")

print("\n#endif // MUSHLIB")
