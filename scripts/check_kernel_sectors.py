"""
@file

@brief The script is used to check kernel 32bit executable (in `.elf`) format.

The kernel executable should have a special structure in order to be loaded correctly into RAM.
The detailed RAM map is shown in @ref md_docs_sources_memory_map.
All of the specified below sectors should be present, located at defined positions and offsets and not exceed defined sizes.

The executable should have the following structure:
<table>
<caption>Executable structure</caption>
<tr><th>Sector name<th>Physical position<th>Virtual position<th>Size
<tr><td>`.GOT.PLT`<td>0x0<td>0x8000<td>0x100
<tr><td>`.DATA`<td>0x100<td>0x8100<td>0x400
<tr><td>`.BSS`<td>0x500<td>0x8500<td>0x1000
<tr><td>`.RODATA`<td>0x1500<td>0x9500<td>0xB00
<tr><td>`.TEXT`<td>0x2000<td>0xA000<td>0x26000
</table>

The script should be called with this cammand:
```
python3 check_kernel_sectors.py <SECTOR_NAME> <VIRTUAL_OFFSET> <MAX_SIZE> <EXECUTABLE_PATH>
```
where `SECTOR_NAME` is one of the sections to check, `VIRTUAL_OFFSET` is expected offset, `MAX_SIZE` is maximum expected size and `EXECUTABLE_PATH` is path to kernel executable.
"""

from lief import parse

from os.path import isfile
from argparse import ArgumentParser

from _utils import error


parser = ArgumentParser()
parser.add_argument("section", help="Section name to check")
parser.add_argument("address", help="Expected virtual address of the section (hex)")
parser.add_argument("size", help="Maximum size available for the section (hex)")
parser.add_argument("file", help="ELF file to extract the section from")

args = vars(parser.parse_args())

if not isfile(args["file"]):
    error(f"File `{args['file']}` can not be opened")
binary = parse(args["file"])

if not binary.has_section(args["section"]):
    error(f"Section `{args['section']}` is not present")
section = binary.get_section(args["section"])

address = int(args["address"], base=16)
if section.virtual_address != address:
    error(f"Section `{args['section']}` has unexpected address: 0x{hex(section.virtual_address)}")

size = int(args["size"], base=16)
if section.size >= size:
    error(f"Section `{args['section']}` size too big: {hex(section.size)} >= 0x{args['size']}")
