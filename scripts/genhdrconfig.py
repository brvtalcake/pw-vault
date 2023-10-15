#!/usr/bin/env python3

import sys
from string import Template

template_define = Template("""\n#define ${mac} ${val}\n""")
template_define_ifndef = Template("""\n#ifndef ${mac}\n#define ${mac} ${val}\n#endif\n""")
template_force_define = Template("""\n#ifdef ${mac}\n\t#undef ${mac}\n#endif\n#define ${mac} ${val}\n""")
template_undefine = Template("""\n#undef ${mac}\n""")
template_ifdef = Template("""\n#ifdef ${mac}\n${expanded}\n#endif\n""")
template_ifdef_bool = Template("""\n#if ${mac}\n${expanded}\n#endif\n""")

def gendefine(define: str, value: int = None, forced: bool = True) -> str:
    if value:
        if forced:
            return template_force_define.substitute(mac = define, val = value)
        else:
            return template_define.substitute(mac = define, val = value)
    else:
        return template_undefine.substitute(mac = define)

def gendefine(define: str, value: str = None, forced: bool = True) -> str:
    if value:
        if forced:
            return template_force_define.substitute(mac = define, val = value)
        else:
            return template_define.substitute(mac = define, val = value)
    else:
        return template_undefine.substitute(mac = define)

def convertible_to_int(value: str) -> bool:
    try:
        int(value)
        return True
    except ValueError:
        return False

assert convertible_to_int("0"), "0 should be convertible to int"
assert convertible_to_int("12345"), "12345 should be convertible to int"
    
def main(cmdargs: list):
    if len(cmdargs) < 1:
        print("Usage: genhdrconfig.py <macro> [<value> [<forced> [<quoted>]]]")
        sys.exit(1)
    define = cmdargs[0]
    value = None
    forced = True
    quoted = False
    if len(cmdargs) == 1:
        print(gendefine(define))
    elif len(cmdargs) == 2:
        value = cmdargs[1]
        if convertible_to_int(value):
            value = int(value)
        print(gendefine(define, value))
    elif len(cmdargs) == 3 or len(cmdargs) == 4:
        value = cmdargs[1]
        if len(cmdargs) == 4:
            quoted = cmdargs[3]
            if str(quoted).strip().upper() == "TRUE" or str(quoted).strip() == "1":
                quoted = True
            elif str(quoted).strip().upper() == "FALSE" or str(quoted).strip() == "0":
                quoted = False
            else:
                print("Invalid quoted value: %s" % quoted)
                sys.exit(1)
        else:
            quoted = False
        if convertible_to_int(value) and not quoted:
            value = int(value)
        elif quoted:
            value = f'"{value}"'
        forced = cmdargs[2]
        if str(forced).strip().upper() == "TRUE" or str(forced).strip() == "1":
            forced = True
        elif str(forced).strip().upper() == "FALSE" or str(forced).strip() == "0":
            forced = False
        else:
            print("Invalid forced value: %s" % forced)
            sys.exit(1)
        print(gendefine(define, value, forced))


if __name__ == "__main__":
    main(sys.argv[1:])

sys.exit(0)
