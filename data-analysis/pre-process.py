#!/usr/bin/env python3

import pandas as pd
import time

raw_data = open("18 al 26 de septiembre.TXT", "r")
test = open("testfile.txt")

eof = False

while not eof:
    line = raw_data.readline().rstrip()

    if line == "":
        eof = True
        print("eof")
    elif line == "SD card found...":
        # Discard useless diagnostic data
        raw_data.readline()
        raw_data.readline()


raw_data.close()
