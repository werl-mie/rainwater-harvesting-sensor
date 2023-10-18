#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv(
    "out_clean.csv",
    parse_dates=True,
    index_col="ts",
    usecols=[
        "ts",
        "temp",
        "vbat_raw",
        "tank_raw",
        "float0",
        "float1",
        "float2",
        "float3",
    ],
)

df.plot(subplots=True, sharex=True)
plt.gcf().autofmt_xdate()
plt.show()
