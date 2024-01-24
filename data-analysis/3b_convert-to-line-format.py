#!/usr/bin/env python3

import pandas as pd

meas_name = "daq-reading"
sensor_id = "daq1"


df = pd.read_csv(
    "./3_output/sensor_rainfall_merged2.csv", index_col="ts", parse_dates=True
)

lines_per_file = len(df) / 15
f_no = 1
i = 0

outf = open(f"./lineformat/data{f_no}.line", "w")

for index, row in df.iterrows():

    if i > lines_per_file * f_no:
        outf.close()
        f_no += 1

        outf = open(f"./lineformat/data{f_no}.line", "w")

    i += 1

    ts_unix_ns = (row.name - pd.Timestamp("1970-01-01")) // pd.Timedelta("1ns")

    # line = (
    #     f'{meas_name},device=daq1 temp="{row.temp}",vbat_raw="{row.vbat_raw}",tank_raw="{row.tank_raw}"'
    #     + f',tlq_level="{row.tlq_level}",cistern_level="{row.cistern_level}",rain_mm="{row.bucket_mm}"'
    #     + f" {ts_unix_ns}\n"
    # )

    line = (
        f"{meas_name},device=daq1 temp={row.temp},vbat_raw={row.vbat_raw},tank_raw={row.tank_raw}"
        + f",tlq_level={row.tlq_level},cistern_level={row.cistern_level},bucket_mm={row.bucket_mm}"
        + f",bucket_mm_h={row.bucket_mm_h},temp-open-meteo={row.temp_2m},rain-open-meteo={row.rain_mm}"
        + f" {ts_unix_ns}\n"
    )

    outf.write(line)
