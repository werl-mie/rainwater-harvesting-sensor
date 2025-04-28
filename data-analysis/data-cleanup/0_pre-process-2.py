#!/usr/bin/env python3

import pandas as pd
import time
from collections import deque

# raw_data = open("18 al 26 de septiembre.TXT", "r")
raw_data = open("elena-may-2-gigastone.TXT", "r")
# raw_data = open("elena-may-1-kingston.TXT", "r")
# raw_data = open("18 agosto a 15 de diciembre 2023.TXT")
out_data = open("elena-may-2-gigastone.csv", "w")
# out_data = open("elena-may-1-kingston.csv", "w")

cb = deque("filller", maxlen=8)
filter = [
    "sd",
    "rtc",
    "am2320",
    "Float0LevelChange",
    "Float1LevelChange",
    "Float2LevelChange",
    # "Float3LevelChange",
    "BucketFirstCountTime",
]

out_data.writelines(
    "ts,id,temp_rtc,temp_am2320,vbat_raw,tank_raw,float0,float1,float2,bucket_counts\n"
)


latest_ts = pd.Timestamp("2023-08-18 18:39:15")



for i,line in enumerate(raw_data.readlines()):
    line = line.rstrip()

    if line == "SD card found...":
        # out_data.writelines(f"{latest_ts},reset,,,,,,,,\n")
        cb.append("sd")
    elif line.startswith("RTC"):
        cb.append("rtc")
    elif line.startswith("AM2320"):
        cb.append("am2320")
    else:
        line_as_list = line.split(",")
        try:
            latest_ts = pd.Timestamp(line_as_list[0])
        except ValueError:
            pass
        else:
            pass

        cb.append(line_as_list[1])

        if line_as_list[1] == "T_rtc": #// For SD 2
        # if line_as_list[1] == "T_am2320":
            out_data.writelines(
                f"{latest_ts},periodic_read,{line_as_list[2]},{line_as_list[4]},{line_as_list[6]},{line_as_list[8]},{line_as_list[10]},{line_as_list[12]},{line_as_list[14]},\n"
            )
        elif line_as_list[1] == "Float0LevelChange" and "sd" not in list(cb):
            out_data.writelines(f"{latest_ts},float0_change,,,,,{line_as_list[2]},,,\n")
        elif line_as_list[1] == "Float1LevelChange" and "sd" not in list(cb):
            out_data.writelines(f"{latest_ts},float1_change,,,,,,{line_as_list[2]},,\n")
        elif line_as_list[1] == "Float2LevelChange" and "sd" not in list(cb):
            out_data.writelines(f"{latest_ts},float2_change,,,,,,,{line_as_list[2]},\n")
        # elif line_as_list[1] == "Float3LevelChange" and "sd" not in list(cb):
        #     out_data.writelines(f"{latest_ts},float3_change,,,,,,,,{line_as_list[2]}\n")
        elif line_as_list[1] == "BucketFirstCountTime" and "sd" not in list(cb):
            out_data.writelines(f"{latest_ts},bucket_count,,,,,,,,{line_as_list[4]}\n")
        elif line_as_list[1] == "HealthCheck":
            out_data.writelines(f"{latest_ts},health_check,,,,,,,,\n")


raw_data.close()
out_data.close()
