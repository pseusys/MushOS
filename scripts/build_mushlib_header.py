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
            header_result = match(r"#define\ MUSHLIB(.*)", line)
            if not line.strip():
                print()
            elif header_result:
                print(f"\n\n// HEADER: {header_result.group(1).split('_')[1]} //")
            elif match(r"#include.*|#ifndef.*|#endif.*", line):
                continue
            else:
                print(line, end="")

print("\n#endif // MUSHLIB")
