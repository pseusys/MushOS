"""
@file

@brief The script is used to build a common MushLib header (just like stdlib `bits` header).

The MushLib common header is built from concatenated stndard MushLib headers.
It can be linked not only with standard MushLib implementations, but also with custom (or `lib/extended`) ones.
During concatenations all includes and include protection `define`'s are stripped and explanatory comments are added instead.
The @ref generic.h header should always be first in concatenation (because of library-wide `define`'s and `typedef`'s).

@note
The script default behavior is **printing output to `stdout`**.
In most cases you will need to pipe it to some output file (e.g. like this: `[run_script] > output.h`).

The script should be called with this cammand:
```
python3 build_mushlib_header.py -l <GENERIC_HEADER> [<OTHER_HEADERS>]
```
where `GENERIC_HEADER` is path to MushLib `genric.h` header and `OTHER_HEADERS` are paths to other headers to include in the common header.
"""

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
