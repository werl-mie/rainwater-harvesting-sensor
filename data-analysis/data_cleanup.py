#!/usr/bin/env python3

import pandas as pd

df = pd.read_csv("out2.csv", index_col="ts", parse_dates=True)

df.bucket_counts = df.bucket_counts.fillna(0)

df = df.interpolate()

# Won't interpolate the vbat yet, because it is not numeric
# Need to cast afterwards because can't cast the uninterpolated gap NaNs into int...

df = df.astype(
    {
        "temp": float,
        "vbat_raw": int,
        "tank_raw": int,
        "float0": int,
        "float1": int,
        "float2": int,
        "float3": int,
        "bucket_counts": int,
    }
)

df = df.interpolate()

df.to_csv("out2_clean.csv")
