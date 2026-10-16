#!/usr/bin/env python3
"""merge_stub_apboot.py - merge the V3F stub and the V5F AP_Bootloader
images into a single CH32H417 CodeFlash image (DBMODE=1, 960KB):

    offset 0x00000000 : 4KB V3F stub (core0, parked after release)
    offset 0x00040000 : V5F AP_Bootloader image (waf --bootloader output)
    the remainder stays erased (0xFF)

usage:
    merge_stub_apboot.py <stub.bin> <apboot.bin> <codeflash_kb> <v5f_offset_kb> <output.bin>
"""

import sys
import struct


def read_bin(path):
    with open(path, 'rb') as f:
        return f.read()


def main():
    if len(sys.argv) != 6:
        print(__doc__)
        sys.exit(1)

    stub_bin = read_bin(sys.argv[1])
    boot_bin = read_bin(sys.argv[2])
    codeflash_size = int(sys.argv[3]) * 1024
    v5f_offset = int(sys.argv[4]) * 1024
    out_path = sys.argv[5]

    # sanity checks
    if v5f_offset % 1024 != 0:
        sys.exit('V5F offset must be 1024-aligned (PFIC wake requirement)')
    if len(stub_bin) > v5f_offset:
        sys.exit('stub image does not fit before the V5F window')
    if v5f_offset + len(boot_bin) > codeflash_size:
        sys.exit('bootloader exceeds the CodeFlash window')

    image = bytearray(b'\xff' * (v5f_offset + len(boot_bin)))
    image[0:len(stub_bin)] = stub_bin
    image[v5f_offset:v5f_offset + len(boot_bin)] = boot_bin

    with open(out_path, 'wb') as f:
        f.write(bytes(image))
    print('merged %s (+%d bytes stub) and %s (+%d bytes bootloader) into %s'
          % (sys.argv[1], len(stub_bin), sys.argv[2], len(boot_bin), out_path))


if __name__ == '__main__':
    main()
