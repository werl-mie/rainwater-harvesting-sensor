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
        "bucket_mm_h",
        "tlq_level",
        "cistern_level",
    ],
)

df = df.rename(
    columns={
        "bucket_counts": "rain (bucket_counts)",
        "float0": "cistern_level_top",
        "float1": "cistern_level_bottom",
        "float2": "tlq_level_bottom",
        "float3": "tlq_level_top",
    }
)


# labels = [
#     "temp",
#     "cistern_level_hi",
#     "cistern_level_lo",
#     "tlq_level_lo",
#     "tlq_level_high",
#     "rain_gauge",
#     "rain_open-meteo",
#     "tank_raw",
#     "vbat",
#     "test",
# ]

axes = df.plot(subplots=[("temp_2m", "temp"), ("bucket_mm", "rain_mm")], sharex=True)
# axes = df[
#     [
#         # "cistern_level_top",
#         # "cistern_level_bottom",
#         # "tlq_level_top",
#         # "tlq_level_bottom",
#         "tlq_level",
#         "cistern_level",
#     ]
# ].plot(subplots=True, sharex=True, yticks=(0, 1, 2))


# axes = df.plot(subplots=True, sharex=True)

# for i, ax in enumerate(axes):
#     ax.set_ylabel(labels[i], rotation="horizontal", x=1000)

plt.gcf().autofmt_xdate()
plt.show()
