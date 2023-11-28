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


# convert bucket counts to mm and resample
df["bucket_mm"] = df["bucket_counts"] * 0.38
td = pd.Timedelta(1, "h")
rs = df["bucket_mm"].resample("1h").sum().rename("bucket_mm_h")
df = pd.merge(df, rs, how="outer", on="ts")

df.bucket_counts = df.bucket_counts.fillna(0)
df.bucket_mm = df.bucket_mm.fillna(0)
df.bucket_mm_h = df.bucket_mm_h.fillna(0)

df = df.interpolate()
# print(df)


df.to_csv("out2_clean.csv")
