#!/bin/env python

import sys
import re

with open(sys.argv[1], 'r') as file:
    output = file.read()
    output = re.sub(r'//(.*)\n', ';\\1\n', output)
    output = re.sub(r'#ifdef(.*)\n', '\n', output)
    output = re.sub(r'#ifndef(.*)\n', '\n', output)
    output = re.sub(r'#endif(.*)\n', '\n', output)
    output = output.replace('#define ', '%define ')

    with open (sys.argv[2], 'w') as outFile:
        outFile.write(output)
