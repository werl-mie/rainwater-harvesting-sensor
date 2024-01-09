#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv(
    "sensor_rainfall_merged2.csv",
    parse_dates=True,
    index_col="ts",
    usecols=[
        "ts",
        "temp_2m",
        "rain_mm",
        "temp",
        "vbat_raw",
        "tank_raw",
        "float0",
        "float1",
        "float2",
        "float3",
        "bucket_counts",
        "bucket_mm",
    ],
)

df = df.rename(columns={"bucket_counts": "rain (bucket_counts)"})

fig, axs = plt.subplots(2, 1, sharex=True)

axes = df.subplots(subplots=[("temp_2m", "temp")], sharex=True)


plt.gcf().autofmt_xdate()
plt.show()
