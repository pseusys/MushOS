from lief import parse

from sys import exit
from os.path import isfile
from argparse import ArgumentParser


def error(message: str):
    exit(f"\u001b[1;31m{message}\u001b[0m")


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
