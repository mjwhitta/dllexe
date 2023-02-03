#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys

def main(options):
    print("[*] Reading bin file...")
    try:
        with open(cli.bin, "rb") as f:
            sc_bytes = f.read()
    except:
        sys.exit(f"[-] Failed to read {cli.bin}")

    if (cli.key):
        print(f"[*] XORing shellcode with \"{cli.key}\"...")
    else:
        print("[-] Not performing XOR...")
    xored_sc_bytes = mem_encode(sc_bytes, cli.key)

    print(f"[*] Writing to {cli.output}...")
    with open("payload.h", "w") as f:
        content = f"""#pragma once

#include <string>

std::wstring sckey = L"{cli.key}";

unsigned char sc[] =
"""

        sc_lines = xored_sc_bytes.splitlines()

        for i, line in enumerate(sc_lines):
            content += f'"{line}"'
            if i != (len(sc_lines) - 1):
                content += "\n"

        content += ";\n"
        f.write(content)

    print("[*] Done.")

def mem_encode(orig_bytes, x_key):
    count = 0
    key_bytes = bytearray(x_key, encoding="UTF-8")
    xored_bytes = ""

    if len(key_bytes) == 0:
        key_bytes = bytes([0])

    for i in range(0, len(orig_bytes)):
        if (count % 17 == 0) and (count != 0):
            xored_bytes += "\n"
        xored_bytes += "\\x{0:02x}".format(
            orig_bytes[i] ^ key_bytes[i % len(key_bytes)]
        )
        count += 1

    return xored_bytes

def parse():
    parser = argparse.ArgumentParser(
        add_help=False,
        description="XORs a bin file and generates C++ header file",
        usage=sys.argv[0] + " [OPTIONS] <bin>"
    )
    parser.add_argument(
        "-h",
        "--help",
        action="store_true",
        help="Display this help message"
    )
    parser.add_argument(
        "-k",
        "--key",
        help="Specify key to XOR with shellcode (default: none)",
        metavar="STR",
        type=str
    )
    parser.add_argument(
        "-o",
        "--output",
        help="Specify output file (default: payload.h)",
        metavar="STR",
        type=str
    )
    parser.add_argument(
        "bin",
        help="Specify the bin file",
        metavar="bin",
        nargs="?",
        type=str
    )
    cli = parser.parse_args()

    if (cli.help):
        parser.print_help()
        sys.exit(0)

    if (not cli.bin):
        parser.print_help()
        sys.exit(1)

    if cli.key == "none":
        cli.key = ""

    if (not cli.output):
        cli.output = "payload.h"

    return cli

if __name__ == "__main__":
    cli = parse()
    main(cli)
