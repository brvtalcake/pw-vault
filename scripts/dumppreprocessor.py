#!/usr/bin/env python3

import sys
import re
import time

dump_pragma = "#pragma dump point "
dump_pragma_regex = "[ \t]*#[ \t]*pragma[ \t]+dump[ \t]+point[ \t]+([0-9]+)[ \t]*"

#def remove_from_to(input: str, from_index: int, to_index: int) -> str:
#    return input[:from_index] + input[to_index:]

def get_next_dump_location(input: str) -> (int, int, int):
    match = re.search(dump_pragma_regex, input)
    if match:
        return (match.span(0)[0], match.span(0)[1], int(match.group(1)))
    else:
        return None

def dump(input: str, to_dump: list) -> str:
    """Replace all dump locations with the corresponding to_dump values."""
    output: str = ""
    while input:
        next_dump_location: (int, int, int) = get_next_dump_location(input)
        if next_dump_location:
            output += input[:next_dump_location[0]]
            output += to_dump[next_dump_location[2]]
            input = input[next_dump_location[1]:]
        else:
            output += input
            input = None
    return output

def main(cmdargs: list):
    if len(cmdargs) < 2:
        print("Usage: dumppreprocessor.py <file> <to_dump1> [<to_dump2> ...]")
        sys.exit(1)
    filename_in: str = cmdargs[0]
    if not filename_in.endswith(".in"):
        print("Input file must end with .in")
        sys.exit(1)
    filename_out: str = filename_in[:-3]
    to_dump: list = cmdargs[1:]
    whole_input: str = ""
    print(f'Reading from {filename_in}')
    with open(filename_in, "r") as file_in:
        whole_input = file_in.read()
        file_in.close()
    whole_output: str = dump(whole_input, to_dump)
    print(f'Writing to {filename_out}')
    with open(filename_out, "w") as file_out:
        file_out.write(whole_output)
        file_out.close()
    print("Done.")

if __name__ == "__main__":
    time_start = time.time()
    main(sys.argv[1:])
    time_end = time.time()
    print(f"Time elapsed: {time_end - time_start} seconds")

sys.exit(0)
