#!/usr/bin/env python3

import pandas as pd
import time
from collections import deque

raw_data = open("18 al 26 de septiembre.TXT", "r")
out_data = open("out.csv", "w")

cb = deque("filller", maxlen=7)
filter = [
    "sd",
    "rtc",
    "am2320",
    "Float0LevelChange",
    "Float1LevelChange",
    "Float2LevelChange",
    "Float3LevelChange",
]

out_data.writelines("ts,id,temp,vbat_raw,tank_raw,float0,float1,float2,float3\n")


latest_ts = pd.Timestamp("2023-08-18 18:39:15")


for line in raw_data.readlines():
    line = line.rstrip()

    if line == "SD card found...":
        out_data.writelines(f"{latest_ts},reset,,,,,,,\n")
        cb.append("sd")
    elif line.startswith("RTC"):
        cb.append("rtc")
    elif line.startswith("AM2320"):
        cb.append("am2320")
    else:
        line_as_list = line.split(",")
        latest_ts = pd.Timestamp(line_as_list[0])
        cb.append(line_as_list[1])

        if line_as_list[1] == "T_am2320":
            out_data.writelines(
                f"{latest_ts},periodic_read,{line_as_list[2]},{line_as_list[4]},{line_as_list[6]},{line_as_list[8]},{line_as_list[10]},{line_as_list[12]},{line_as_list[14]}\n"
            )
        elif line_as_list[1] == "Float0LevelChange" and list(cb)[-4:] != filter[0:4]:
            out_data.writelines(f"{latest_ts},float0_change,,,,{line_as_list[2]},,,\n")
        elif line_as_list[1] == "Float1LevelChange" and list(cb)[-5:] != filter[0:5]:
            out_data.writelines(f"{latest_ts},float1_change,,,,,{line_as_list[2]},,\n")
        elif line_as_list[1] == "Float2LevelChange" and list(cb)[-6:] != filter[0:6]:
            out_data.writelines(f"{latest_ts},float2_change,,,,,,{line_as_list[2]},\n")
        elif line_as_list[1] == "Float3LevelChange" and list(cb)[-7:] != filter[0:7]:
            out_data.writelines(f"{latest_ts},float3_change,,,,,,,{line_as_list[2]}\n")
        elif line_as_list[1] == "HealthCheck":
            out_data.writelines(f"{latest_ts},reset,,,,,,,\n")


raw_data.close()
out_data.close()
