#!/usr/bin/env python3

import pandas as pd


def tank_level(float_high, float_low):
    # float_high | float_low | tank_level
    #     0      |     0     |    N/A
    #     0      |     1     |     2
    #     1      |     0     |     0
    #     1      |     1     |     1

    tank_level = None  # 0 == empty, 1 == filling, 2 == full
    tank_code = (int(float_high) << 1) | int(float_low)

    if tank_code == 1:
        tank_level = 2
    elif tank_code == 2:
        tank_level = 0
    elif tank_code == 3:
        tank_level = 1

    return tank_level


df = pd.read_csv("elena-may-1-kingston.csv",index_col="ts",parse_dates=True)



df.bucket_counts = df.bucket_counts.fillna(0)
df.temp_am2320 = df.temp_am2320.fillna(0)
# df.temp = df.temp.fillna(0)

df.dropna()


df = df.interpolate()

print(df)

# Won't interpolate the vbat yet, because it is not numeric
# Need to cast afterwards because can't cast the uninterpolated gap NaNs into int...

df = df.astype(
    {
        # "temp_rtc": float,
        "temp_am2320": float,
        # "temp": float,
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
df = pd.merge(df, rs, how="outer", on="ts").sort_index()

df.id = df.id.fillna("resample")
df.bucket_mm_h = df.bucket_mm_h.fillna(0)

# Delete first row
df = df.drop(df.iloc[0].name)

df = df.interpolate()

# df.bucket_counts = df.bucket_counts.fillna(0)
# df.bucket_mm = df.bucket_mm.fillna(0)

# print(df)

# df.to_csv("out2_clean_buckets.csv")


# df["tlq_level"] = df.apply(lambda row: tank_level(row["float3"], row["float2"]), axis=1)
# df["cistern_level"] = df.apply(
#     lambda row: tank_level(row["float0"], row["float1"]), axis=1
# )

df.to_csv("out2_clean.csv")
